#include "core/logger.hpp"

int main ()
{
	try {
		LOG_INFO("This is an info log with no args");
		LOG_WARNING("This is a warning log with no args");
		LOG_ERROR("This is an error log with no args");
		LOG_CRITICAL("This is a critical log with no args");

		int testInt = 5;
		std::string testStr = "test";
		LOG_INFO("This is an info log with args: {} {}", testInt, testStr);
		LOG_WARNING("This is a warning log with args: {} {}", testInt, testStr);
		LOG_ERROR("This is an error log with args: {} {}", testInt, testStr);
		LOG_CRITICAL("This is a critical log with args: {} {}", testInt, testStr);

		std::cout << "All logs have been written without throwing exceptions.\n";
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception caught: " << ex.what() << '\n';
		return 1;
	}
	catch (...) {
		std::cerr << "Unknown exception caught\n";
		return 1;
	}

	return 0;
}
