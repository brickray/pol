#pragma once

#include "../core/texture.h"

namespace pol {
	class Checkerboard : public Texture {
	private:
		Vector3f value[2];
		Float uscale;
		Float vscale;

	public:
		Checkerboard(const PropSets& props, Scene& scene)
			:Texture(props, scene) {
			value[0] = props.GetVector3f("first", Vector3f::Zero());
			value[1] = props.GetVector3f("second", Vector3f::One());
			uscale = props.GetFloat("uscale", 10);
			vscale = props.GetFloat("vscale", 10);
		}

		virtual Vector3f Evaluate(const Intersection& isect) const {
			int x = isect.uv.x * uscale;
			int y = isect.uv.y * vscale;
			return value[(x + y) % 2];
		}

		virtual string ToString() const {
			string ret;
			ret += "Checkerboard[\n  first = " + value[0].ToString()
				+ ",\n  second = " + value[1].ToString()
				+ ",\n  uscale = " + to_string(uscale)
				+ ",\n  vscale = " + to_string(vscale)
				+ "\n]";

			return ret;
		}
	};
}