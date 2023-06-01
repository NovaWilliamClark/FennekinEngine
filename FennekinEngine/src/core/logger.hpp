#pragma once
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <queue>
#include <string>
#include <format>
#include <thread>
#include <atomic>

#include "platform/platform.hpp"

#ifdef PLATFORM_WINDOWS
#define DEBUG_BREAK  __debugbreak()
#else
#define DEBUG_BREAK __builtin_trap()
#endif


#define LOG_INFO(...)    Logger::instance().log(ELogLevel::INFO,    __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) Logger::instance().log(ELogLevel::WARNING, __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   Logger::instance().log(ELogLevel::ERROR,   __FILENAME__, __LINE__, __VA_ARGS__); Logger::instance().flush(); DEBUG_BREAK
#define LOG_CRITICAL(...) Logger::instance().log(ELogLevel::CRITICAL, __FILENAME__, __LINE__, __VA_ARGS__); std::exit(EXIT_FAILURE)

enum class ELogLevel{INFO, WARNING, ERROR, CRITICAL};

class Logger {
public:
	Logger();

	~Logger();

	static Logger& instance();

	template <typename ... Args>
	void log(const ELogLevel t_level, const std::string& t_filename, const int t_line, const std::string& t_format,
		Args&&... t_args)
	{
		std::unique_lock lock(m_queueMutex);
		m_logQueue.push({ t_level, t_filename, t_line, std::vformat(t_format, std::make_format_args(std::forward<Args>(t_args)...)) });
		lock.unlock();

		m_condition.notify_one();
	}

	void flush();

private:
	struct LogMessage {
		ELogLevel level;
		std::string filename;
		int line;
		std::string message;
	};

	void processQueue();

	void handleLogMessage(const LogMessage& t_msg);

	static std::string toString(const ELogLevel t_level);

	static void writeLogToFile(const std::string& t_logStr);

	static void writeLogToConsole(const ELogLevel t_level, const std::string& t_logStr);

	inline static std::string m_logFileName;

	std::queue<LogMessage> m_logQueue;	
	std::mutex m_queueMutex;
	std::condition_variable m_condition;
	std::atomic_bool m_exitFlag;
	std::thread m_worker;
	std::mutex m_logMutex;
};