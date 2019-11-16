#pragma once

#include "../core/texture.h"
#include "../core/mipmap.h"
#include "../core/imageio.h"
#include "../core/directory.h"

namespace pol {
	class Image : public Texture {
	private:
		Mipmap image;

	public:
		Image(const PropSets& props, Scene& scene)
			:Texture(props, scene) {
			string file = props.GetString("file");
			if (file == "") {
				fprintf(stderr, "Please specific file name\n");
				return;
			}
			bool srgb = props.GetBool("srgb", true);
			string filtermode = props.GetString("filtermode", "trilinear");
			string wrapmode = props.GetString("wrapmode", "repeat");
			FilterMode fmode;
			if (filtermode == "nearst") fmode = FilterMode::E_NEARST;
			else if (filtermode == "linear") fmode = FilterMode::E_LINEAR;
			else if (filtermode == "trilinear") fmode = FilterMode::E_TRILINEAR;
			else fmode = FilterMode::E_TRILINEAR;

			WrapMode wmode;
			if (wrapmode == "clamp") wmode = WrapMode::E_CLAMP;
			else if (wrapmode == "mirror") wmode = WrapMode::E_MIRROR;
			else if (wrapmode == "repeat") wmode = WrapMode::E_REPEAT;
			else wmode = WrapMode::E_REPEAT;

			bool flip = props.GetBool("flip", true);
			
			int w, h;
			vector<Vector3f> data;
			ImageIO::LoadTexture(Directory::GetFullPath(file).c_str(), w, h, srgb, flip, data);

			//build mipmap
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
}