#include "light.h"
#include "scene.h"

namespace pol {
	Light::Light(const PropSets& props, Scene& scene) {
		scene.AddLight(this);
	}

	Light::~Light() {

	}
}