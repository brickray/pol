#include "checkerboard.h"

namespace pol {
	Checkerboard* CreateCheckerboardTexture(const Vector3f& first, const Vector3f& second, Float uscale, Float vscale) {
		return new Checkerboard(first, second, uscale, vscale);
	}
}