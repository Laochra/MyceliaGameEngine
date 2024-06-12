#pragma once

#include <chrono>

typedef std::chrono::system_clock Clock;
typedef std::chrono::duration<double> Duration;
typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

class StackTimer
{
public:
	StackTimer(const char* const nameInit = "Unnamed Stack Timer", const char* const locationInfo = "");
	~StackTimer();
	StackTimer(StackTimer&) = delete;
	StackTimer& operator=(StackTimer&) = delete;

private:
	const char* const name;
	const char* const location;
	TimePoint* start;
};