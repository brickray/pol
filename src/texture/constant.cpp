#include "constant.h"

namespace pol {
	Constant* CreateConstantTexture(const Vector3f& value) {
		return new Constant(value);
	}
}