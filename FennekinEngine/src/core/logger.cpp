#include "logger.hpp"

#include <fstream>
#include <iostream>

Logger::Logger(): m_exitFlag(false)
{
	// Launch the worker thread that will handle the log messages
	m_worker = std::thread([this] { processQueue(); });
}

Logger::~Logger()
{
	m_exitFlag = true;			// Set the flag that signals the worker thread to exit
	m_condition.notify_one();	// Notify the worker thread that it can exit
	if (m_worker.joinable()) { m_worker.join(); }	// Wait for the worker thread to finish
}

// get a Singleton instance
Logger& Logger::instance()
{
	static Logger instance;
	return instance;
}

//
void Logger::flush()
{
	std::atomic waitForFlush = true;
	while (waitForFlush) {
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			waitForFlush = !m_logQueue.empty();
		}
		if (waitForFlush)
			std::this_thread::yield(); // Allow worker thread to process messages
	}
}

// Worker thread function to process the log messages
void Logger::processQueue()
{
	// Keep processing messages until the exit flag is set and the queue is empty
	while (!m_exitFlag || !m_logQueue.empty()) {
		std::unique_lock lock(m_queueMutex);	

		// Wait for a new log message to be added or for the exit flag to be set
		m_condition.wait(lock, [this] { return m_exitFlag || !m_logQueue.empty(); });

		// In case of a spurious wakeup, if the queue is empty, continue to the next iteration
		if (m_logQueue.empty())
			continue;

		auto msg = m_logQueue.front();
		m_logQueue.pop();
		lock.unlock();

		handleLogMessage(msg);
	}
}

void Logger::handleLogMessage(const LogMessage& t_msg)
{
	std::lock_guard lock(m_logMutex);

	const auto now = std::chrono::system_clock::now();
	const std::time_t tt = std::chrono::system_clock::to_time_t(now);
	std::tm tm;
	localtime_s(&tm, &tt);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S")
		<< " [" << toString(t_msg.level) << "] "
		<< " File: " << t_msg.filename
		<< " Line: " << t_msg.line
		<< " - "
		<< t_msg.message << std::endl;

	writeLogToFile(oss.str());
	writeLogToConsole(t_msg.level, oss.str());
}

std::string Logger::toString(const ELogLevel t_level)
{
	switch (t_level) {
	case ELogLevel::INFO:     return "INFO";
	case ELogLevel::WARNING:  return "WARNING";
	case ELogLevel::ERROR:    return "ERROR";
	case ELogLevel::CRITICAL: return "CRITICAL";
	}
	return "UNKNOWN";
}

void Logger::writeLogToFile(const std::string& t_logStr)
{
	// Ensure logs directory exists
	if (!std::filesystem::exists("logs") || !std::filesystem::is_directory("logs")) {
		std::filesystem::create_directory("logs");
	}

	// Generate filename on first log message
	if (m_logFileName.empty()) {
		const auto now = std::chrono::system_clock::now();
		const auto tt = std::chrono::system_clock::to_time_t(now);
		std::tm tm;
		localtime_s(&tm, &tt);
		std::ostringstream oss;
		oss << "logs/" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
		m_logFileName = oss.str();
	}

	// Write log message to file
	std::ofstream logFile(m_logFileName, std::ios::app);
	if (logFile) {
		logFile << t_logStr;
	}
}

void Logger::writeLogToConsole(const ELogLevel t_level, const std::string& t_logStr)
{
#ifdef PLATFORM_WINDOWS
	// Change console color via Windows API handle because Windows command prompt
	// only supports ANSI color escape codes when the program is run through an IDE

	// Grab console handle
	const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

	// Save current color
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	const WORD savedAttributes = consoleInfo.wAttributes;

	// Switch color based on log level
	switch (t_level) {
	case ELogLevel::INFO:
		SetConsoleTextAttribute(hConsole, 0x0008); // grey text
		break;
	case ELogLevel::WARNING:
		SetConsoleTextAttribute(hConsole, 0x0004 | 0x0002); // gold text
		break;
	case ELogLevel::ERROR:
		SetConsoleTextAttribute(hConsole, 0x0004); // red text
		break;
	case ELogLevel::CRITICAL:
		SetConsoleTextAttribute(hConsole, 0x004f); // Red background + white text
		break;
	}
	std::cout << t_logStr;

	// Restore original color
	SetConsoleTextAttribute(hConsole, savedAttributes);

#else
		// For UNIX & LINUX variants, use ANSI escape codes to change console text color
		switch (level) {
		case ELogLevel::INFO:
			std::cout << "\033[37m"; // grey
			break;
		case ELogLevel::WARNING:
			std::cout << "\033[33m"; // gold
			break;
		case ELogLevel::ERROR:
			std::cout << "\033[31m"; // red
			break;
		case ELogLevel::CRITICAL:
			std::cout << "\033[41;37m"; // red background + white text
			break;
		default:
			break;
	}

		std::cout << logStr;

		// Reset color
		std::cout << "\033[0m";
#endif
}
