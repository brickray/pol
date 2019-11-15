#pragma once

#include "../pol.h"

namespace pol {
	class Directory {
	public:
		static string base;

	public:
		static string GetFullPath(const string& p);
	};
}