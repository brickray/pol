#pragma once

#include "object.h"

namespace pol {
	class Texture : public PolObject {
	public:
		Texture() {}
		virtual ~Texture() {}

		virtual Vector3f Evaluate(const Vector2f& uv) const = 0;
	};
}