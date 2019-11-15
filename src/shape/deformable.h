#pragma once

#include "../pol.h"

namespace pol {
	class TriangleMesh;
	bool CreateSubDivisionShape(int level, TriangleMesh* mesh);
}