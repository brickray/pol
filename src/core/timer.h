#pragma once

#include "../pol.h"
#include <Windows.h>

namespace pol {
	class Timer {
	private:
		LARGE_INTEGER start;
		LARGE_INTEGER end;

	public:
		Timer();

		void Start();
		void End();

		//return seconds;
		float GetElapsed() const;
	};
}