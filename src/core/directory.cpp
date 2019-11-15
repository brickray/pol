#include "directory.h"

namespace pol {
	string Directory::base;

	string Directory::GetFullPath(const string& p) {
		return base + p;
	}
}