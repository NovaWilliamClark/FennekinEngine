#include "core/time.hpp"

Time::Time() {
    m_startTime = std::chrono::high_resolution_clock::now();
    m_previousTime = m_startTime;
    m_deltaTime = 0.0f;
    m_totalTime = 0.0f;

    startTimer();
}

void Time::update() {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> timeSinceStart = currentTime - m_startTime;
    const std::chrono::duration<float> frameTime = currentTime - m_previousTime;

    m_deltaTime = frameTime.count();
    m_totalTime = timeSinceStart.count();

    m_previousTime = currentTime;
}

float Time::getDeltaTime() const {
    return m_deltaTime;
}

float Time::getTotalTime() const {
    return m_totalTime;
}

void Time::startTimer() {
    m_timerStart = std::chrono::high_resolution_clock::now();
}

template <typename TDurationType>
long long Time::endTimer() const {
    const auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<TDurationType>(endTime - m_timerStart);

    return duration.count();
}

template long long Time::endTimer<std::chrono::minutes>() const;
template long long Time::endTimer<std::chrono::seconds>() const;
template long long Time::endTimer<std::chrono::milliseconds>() const;
template long long Time::endTimer<std::chrono::nanoseconds>() const;