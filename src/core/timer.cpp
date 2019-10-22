#include "timer.h"

namespace pol {
	Timer::Timer()
		:start()
		, end() {

	}

	void Timer::Start() {
		QueryPerformanceCounter(&start);
	}

	void Timer::End() {
		QueryPerformanceCounter(&end);
	}

	float Timer::GetElapsed() const {
		LARGE_INTEGER freq;
		//frequency per second
		QueryPerformanceFrequency(&freq);

		return ((float)(end.QuadPart - start.QuadPart)) / freq.QuadPart;
	}
}