#pragma once

#include "../pol.h"

namespace pol {
	//input:   height map
	//output:  triangle mesh
	class TriangleMesh;
	bool CreateHeightFieldShape(const char* file, TriangleMesh* mesh);
}