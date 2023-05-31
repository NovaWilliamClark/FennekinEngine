#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <format>
#include <thread>
#include <latch>

#include "platform/platform.hpp"


enum class ELogLevel{INFO, WARNING, ERROR, CRITICAL};

class Logger {
public:
	static Logger& instance() {
		static Logger instance;
		return instance;
	}

	template<typename... Args>
	void log(ELogLevel level, const std::string& file, int line, const std::string& format, Args&&... args) {
		std::lock_guard<std::mutex> lock(m_logMutex);
		auto now = std::chrono::system_clock::now();
		std::time_t tt = std::chrono::system_clock::to_time_t(now);
		std::tm tm;
		localtime_s(&tm, &tt);

		auto msg = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));

		std::ostringstream oss;
		oss << "[" << toString(level) << "] "
			<< std::put_time(&tm, "%d-%m-%Y %H:%M:%S")
			<< " || File: " << file
			<< " || Line: " << line
			<< " --- "
			<< msg << std::endl;

		writeLogToFile(oss.str());
		writeLogToConsole(level, oss.str());
	}

private:
	Logger() = default;
	~Logger() = default;

	static std::string toString(const ELogLevel t_level) {
		switch (t_level) {
		case ELogLevel::INFO:     return "INFO";
		case ELogLevel::WARNING:  return "WARNING";
		case ELogLevel::ERROR:    return "ERROR";
		case ELogLevel::CRITICAL: return "CRITICAL";
		default:                  return "UNKNOWN";
		}
	}

	static void writeLogToFile(const std::string& t_logStr) {
		std::ofstream logFile("session.log", std::ios::app);
		if (logFile) {
			logFile << t_logStr;
		}
	}

	static void writeLogToConsole(ELogLevel t_level, const std::string& t_logStr) {
#if defined(PLATFORM_WINDOWS)
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		WORD savedAttributes;

		// Save current color
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		savedAttributes = consoleInfo.wAttributes;

		switch (t_level) {
		case ELogLevel::INFO:
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
			break;
		case ELogLevel::WARNING:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
			break;
		case ELogLevel::ERROR:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
			break;
		case ELogLevel::CRITICAL:
			SetConsoleTextAttribute(hConsole, BACKGROUND_RED | FOREGROUND_INTENSITY);
			break;
		default:
			break;
		}
		std::cout << t_logStr;

		// Restore original color
		SetConsoleTextAttribute(hConsole, savedAttributes);
#else
		switch (level) {
		case ELogLevel::INFO:
			std::cout << "\033[32m"; // Green
			break;
		case ELogLevel::WARNING:
			std::cout << "\033[33m"; // Yellow
			break;
		case ELogLevel::ERROR:
			std::cout << "\033[31m"; // Red
			break;
		case ELogLevel::CRITICAL:
			std::cout << "\033[41m"; // Red background
			break;
		default:
			break;
	}

		std::cout << logStr;

		// Reset color
		std::cout << "\033[0m";
#endif
	}

	std::mutex m_logMutex;
};

#define LOG_INFO(...)    Logger::instance().log(ELogLevel::INFO,    __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) Logger::instance().log(ELogLevel::WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   Logger::instance().log(ELogLevel::ERROR,   __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRITICAL(...) Logger::instance().log(ELogLevel::CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
