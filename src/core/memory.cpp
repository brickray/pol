#include "memory.h"
#include <Windows.h>

namespace pol {
	void* AllocAligned(int size) {
		return _aligned_malloc(size, POL_L1_CACHE_LINE_SIZE);
	}

	void FreeAligned(void* p) {
		_aligned_free(p);
	}
}