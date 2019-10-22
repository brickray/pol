#pragma once

#include "../core/texture.h"

namespace pol {
	class Checkerboard : public Texture {
	private:
		Vector3f value[2];
		Float uscale;
		Float vscale;

	public:
		Checkerboard(const Vector3f& first, const Vector3f& second, Float uscale, Float vscale)
			: uscale(uscale), vscale(vscale) {
			value[0] = first;
			value[1] = second;
		}

		virtual Vector3f Evaluate(const Vector2f& uv) const {
			int x = uv.x * uscale * 0.5f;
			int y = uv.y * vscale * 0.5f;
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

	Checkerboard* CreateCheckerboardTexture(const Vector3f& first, const Vector3f& second, Float uscale, Float vscale);
}