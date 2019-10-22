#pragma once

#include "../pol.h"
#include <thread>

namespace pol {
	class Parallel {
	private:
		vector<thread> threads;

	public:
		static void Init();

		static int GetNumSystemCores();
	};
}