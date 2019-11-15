#pragma once

#include "../pol.h"

namespace pol {
	class Scene;
	class Parser {
	public:
		static bool Parse(const string& file, Scene& scene);
	};
}