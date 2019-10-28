#include "image.h"

namespace pol {
	Image* CreateImageTexture(int w, int h, const vector<Vector3f>& data, FilterMode fmode, WrapMode wmode) {
		return new Image(w, h, data, fmode, wmode);
	}
}