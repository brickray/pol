#pragma once

#include "../core/texture.h"

namespace pol {
	class Constant : public Texture {
	private:
		Vector3f value;

	public:
		Constant(const PropSets& props, Scene& scene)
			:Texture(props, scene) {
			value = props.GetVector3f("value", Vector3f::One());
		}

		Vector3f Evaluate(const Intersection& isect) const {
			return value;
		}

		string ToString() const {
			string ret;
			ret += "Constant" + value.ToString();

			return ret;
		}
	};
}