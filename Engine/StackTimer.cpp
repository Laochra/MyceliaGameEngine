#include "StackTimer.h"

#include "Debug.h"

StackTimer::StackTimer(const char* const nameInit, const char* const locationInfo) :
	name(nameInit),
	location(locationInfo),
	start(new TimePoint(Clock::now())) { }

StackTimer::~StackTimer()
{
	using namespace std::chrono;
	TimePoint end(Clock::now());

	Duration duration = end - *start;
	nanoseconds ns = duration_cast<nanoseconds>(duration);
	
	double ms = ns.count() / 1'000'000.0f;

	Debug::Log(name, " took ", ms, " milliseconds to complete", location);
}
