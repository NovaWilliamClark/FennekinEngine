#pragma once
#include <chrono>

class Time
{
public:
    Time();

    void update();

    [[nodiscard]] float getDeltaTime() const;

    [[nodiscard]] float getTotalTime() const;

    void startTimer();


    template <typename TDurationType>
    [[nodiscard]] long long endTimer() const;

    using Minutes = std::chrono::minutes;
    using Seconds = std::chrono::seconds;
    using Milliseconds = std::chrono::milliseconds;
    using Nanoseconds = std::chrono::nanoseconds;

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::chrono::high_resolution_clock::time_point m_previousTime;
    std::chrono::high_resolution_clock::time_point m_timerStart;
    float m_deltaTime;
    float m_totalTime;
};