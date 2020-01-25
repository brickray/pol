#pragma once

#include "../pol.h"
#include "renderblock.h"
#include <thread>
#include <functional>
#include <mutex>

namespace pol {
	class Parallel {
	private:
		static vector<thread*> threads;

	public:
		static void Startup();
		static void Shutdown();
		static void ParallelLoop(function<void(const RenderBlock& rb)> func, const vector<RenderBlock>& rbs);
		static bool IsFinish();
		static void WaitUntilTaskFinish();

		static int GetNumSystemCores();
	};
}