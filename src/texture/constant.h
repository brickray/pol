#pragma once

#include "../core/texture.h"

namespace pol {
	class Constant : public Texture {
	private:
		Vector3f value;

	public:
		Constant(const Vector3f& value)
			:value(value) {

		}

		Vector3f Evaluate(const Vector2f& uv) const {
			return value;
		}

		string ToString() const {
			string ret;
			ret += "Constant" + value.ToString();

			return ret;
		}
	};

	Constant* CreateConstantTexture(const Vector3f& value);
}