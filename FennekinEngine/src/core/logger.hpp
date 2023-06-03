#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <format>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "platform/platform.hpp"

#ifdef PLATFORM_WINDOWS
#define DEBUG_BREAK  __debugbreak()
#else
#define DEBUG_BREAK __builtin_trap()
#endif

// Logging Macros
#define LOG_INFO(...)    Logger::instance().log(ELogLevel::INFO,    __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) Logger::instance().log(ELogLevel::WARNING, __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_ERROR_NBP(...)   Logger::instance().log(ELogLevel::ERROR,   __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   Logger::instance().log(ELogLevel::ERROR,   __FILENAME__, __LINE__, __VA_ARGS__); Logger::instance().flush(); DEBUG_BREAK
#define LOG_CRITICAL(...) Logger::instance().log(ELogLevel::CRITICAL, __FILENAME__, __LINE__, __VA_ARGS__); std::exit(EXIT_FAILURE)

constexpr size_t MAX_LOG_SIZE = 5000;

/**
 * @enum ELogLevel
 * @brief Defines the log levels.
 */
enum class ELogLevel { INFO, WARNING, ERROR, CRITICAL };

/**
 * @class Logger
 * @brief Handles logging of messages with different levels.
 */
class Logger {
public:
	Logger();
	~Logger();

	/**
	 * @brief Returns the singleton instance of the logger.
	 * @return Logger& - The singleton instance.
	 */
	static Logger& instance();

	/**
	 * @brief Logs a message with the given level, filename, line, and format.
	 * @tparam TArgs - The types of the arguments.
	 * @param t_level - The log level.
	 * @param t_filename - The filename where the log call is made.
	 * @param t_line - The line number where the log call is made.
	 * @param t_format - The format of the log message.
	 * @param t_args - The arguments for the format string.
	 */
	template <typename ... TArgs>
	void log(const ELogLevel t_level, const std::string& t_filename, const int t_line, const std::string& t_format, TArgs&&... t_args) {
		std::unique_lock lock(m_queueMutex);
		m_logQueue.push({ t_level, t_filename, t_line, std::vformat(t_format, std::make_format_args(std::forward<TArgs>(t_args)...)) });

		lock.unlock();
		if (m_logQueue.size() >= MAX_LOG_SIZE) {
			flush();
		}

		m_condition.notify_one();

	}

	/**
	 * @brief Flushes the log queue.
	 */
	void flush();

private:
	/**
	 * @struct LogMessage
	 * @brief Represents a log message.
	 */
	struct LogMessage {
		ELogLevel level;
		std::string filename;
		int line;
		std::string message;
	};

	/**
	 * @brief Processes the log queue.
	 */
	void processQueue();

	/**
	 * @brief Handles a log message.
	 * @param t_msg - The log message to handle.
	 */
	void handleLogMessage(const LogMessage& t_msg);

	/**
	 * @brief Converts a log level to a string.
	 * @param t_level - The log level to convert.
	 * @return std::string - The string representation of the log level.
	 */
	static std::string toString(const ELogLevel t_level);

	/**
	 * @brief Writes a log message to a file.
	 * @param t_logStr - The log message to write.
	 */
	static void writeLogToFile(const std::string& t_logStr);

	/**
	 * @brief Writes a log message to the console.
	 * @param t_level - The log level of the message.
	 * @param t_logStr - The log message to write.
	 */
	static void writeLogToConsole(const ELogLevel t_level, const std::string& t_logStr);

	inline static std::string m_logFileName;
	std::queue<LogMessage> m_logQueue;
	std::mutex m_queueMutex;
	std::condition_variable m_condition;
	std::atomic_bool m_exitFlag;
	std::thread m_worker;
	std::mutex m_logMutex;
};