#include "logger.hpp"

#include <fstream>
#include <iostream>

#include "io/color.hpp"

/**
 * @brief Constructor for Logger. Launches the worker thread that will handle the log messages.
 */
Logger::Logger() : m_exitFlag(false)
{
    m_worker = std::thread([this] { processQueue(); });
}

/**
 * @brief Destructor for Logger. Signals the worker thread to exit and waits for it to finish.
 */
Logger::~Logger()
{
    m_exitFlag = true;
    m_condition.notify_one();
    if (m_worker.joinable()) { m_worker.join(); }
}

/**
 * @brief Returns a singleton instance of Logger.
 * @return Logger& - The singleton instance.
 */
Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

/**
 * @brief Flushes the log queue.
 */
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

/**
 * @brief Worker thread function to process the log messages.
 */
void Logger::processQueue()
{
	while (!m_exitFlag || !m_logQueue.empty()) {
		std::unique_lock lock(m_queueMutex);

		m_condition.wait(lock, [this] { return m_exitFlag || !m_logQueue.empty(); });

		if (m_logQueue.empty()) {
			if (m_flushInProgress) {
				m_flushInProgress = false;
				m_flushCV.notify_one();
			}

			continue;
		}

		auto msg = m_logQueue.front();
		m_logQueue.pop();
		lock.unlock();

		handleLogMessage(msg);
	}
}

/**
 * @brief Handles a log message.
 * @param t_msg - The log message to handle.
 */
void Logger::handleLogMessage(const LogMessage& t_msg)
{
    std::lock_guard lock(m_logMutex);

    std::ostringstream oss;
    oss << " [" << toString(t_msg.level) << "] "
        << " File: " << t_msg.filename
        << " Line: " << t_msg.line
        << " - "
        << t_msg.message << std::endl;

    writeLogToFile(oss.str());
    writeLogToConsole(t_msg.level, oss.str());
}

/**
 * @brief Converts a log level to a string.
 * @param t_level - The log level to convert.
 * @return std::string - The string representation of the log level.
 */
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

/**
 * @brief Writes a log message to a file.
 * @param t_logStr - The log message to write.
 */
void Logger::writeLogToFile(const std::string& t_logStr)
{
    if (!std::filesystem::exists("logs") || !std::filesystem::is_directory("logs")) {
        std::filesystem::create_directory("logs");
    }

    if (m_logFileName.empty()) {
        const auto now = std::chrono::system_clock::now();
        const auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_s(&tm, &tt);
        std::ostringstream oss;
        oss << "logs/" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
        m_logFileName = oss.str();
    }

    std::ofstream logFile(m_logFileName, std::ios::app);
    if (logFile) {
        logFile << t_logStr;
        logFile.close();
    }
}

/**
 * @brief Writes a log message to the console.
 * @param t_level - The log level of the message.
 * @param t_logStr - The log message to write.
 */
void Logger::writeLogToConsole(const ELogLevel t_level, const std::string& t_logStr)
{
#ifdef PLATFORM_WINDOWS
    const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    const WORD savedAttributes = consoleInfo.wAttributes;

    switch (t_level) {
    case ELogLevel::INFO:
        SetConsoleTextAttribute(hConsole, FG_BRIGHT_BLACK); // Grey text
        break;
    case ELogLevel::WARNING:
        SetConsoleTextAttribute(hConsole, FG_YELLOW); // Yellow
        break;
    case ELogLevel::ERROR:
        SetConsoleTextAttribute(hConsole, FG_RED); // Red text
        break;
    case ELogLevel::CRITICAL:
        SetConsoleTextAttribute(hConsole, FG_WHITE | BG_RED); // Red background + white text
        break;
    }
    std::cout << t_logStr;

    SetConsoleTextAttribute(hConsole, savedAttributes); // Reset text color 

#else
    switch (t_level) {
    case ELogLevel::INFO:
        std::cout << FG_BRIGHT_BLACK; // Grey
        break;
    case ELogLevel::WARNING:
        std::cout << FG_YELLOW; // Yellow
        break;
    case ELogLevel::ERROR:
        std::cout << FG_RED; // Red
        break;
    case ELogLevel::CRITICAL:
        std::cout << BG_RED << FG_WHITE; // Red background + white text
        break;
    default:
        break;
    }

    std::cout << t_logStr;
    std::cout << DEFAULT;
#endif
}
