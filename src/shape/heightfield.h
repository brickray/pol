#pragma once

#include "triangle.h"

namespace pol {
	//input:   height map
	//output:  triangle mesh
	vector<Triangle*> CreateHeightFieldShape(const Transform& world, int w, int h, const vector<Vector3f>& height, Bsdf* bsdf, Texture* alphaMask = nullptr);
	vector<Triangle*> CreateHeightFieldShape(const Transform& world, const char* file, Bsdf* bsdf, Texture* alphaMask = nullptr);
}