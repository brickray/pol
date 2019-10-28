#pragma once

#include "../core/texture.h"
#include "../core/mipmap.h"

namespace pol {
	class Image : public Texture {
	private:
		Mipmap image;

	public:
		Image(int w, int h, const vector<Vector3f>& data, FilterMode fmode, WrapMode wmode) {
			image.Build(w, h, data, fmode, wmode);
		}

		Vector3f Evaluate(const Intersection& isect) const {
			return image.Lookup(isect);
		}

		string ToString() const {
			string ret;
			ret += "Image[\n  " + indent(image.ToString())
				+ "\n]";

			return ret;
		}
	};

	Image* CreateImageTexture(int w, int h, const vector<Vector3f>& data, FilterMode fmode = FilterMode::E_TRILINEAR, WrapMode wmode = WrapMode::E_REPEAT);
}