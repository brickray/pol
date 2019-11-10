#pragma once

#include "triangle.h"

namespace pol {
	vector<Triangle*> CreateSubDivisionShape(int level, const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
		const vector<Vector2f>& uv, const vector<int>& indices, Bsdf* bsdf, Texture* alphaMask = nullptr);
}