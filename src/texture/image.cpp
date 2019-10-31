#include "image.h"
#include "../core/imageio.h"

namespace pol {
	Image* CreateImageTexture(int w, int h, const vector<Vector3f>& data, FilterMode fmode, WrapMode wmode) {
		return new Image(w, h, data, fmode, wmode);
	}

	Image* CreateImageTexture(const char* file, FilterMode fmode, WrapMode wmode) {
		int w, h;
		vector<Vector3f> data;
		ImageIO::LoadTexture(file, w, h, true, data);
		return CreateImageTexture(w, h, data, fmode, wmode);
	}
}