#pragma once

#include "object.h"
#include "intersection.h"

namespace pol {
	class Texture : public PolObject {
	public:
		Texture() {}
		virtual ~Texture() {}

		virtual Vector3f Evaluate(const Intersection& isect) const = 0;
	};
}