#include "accelerator.h"
#include "scene.h"

namespace pol {
	Accelerator::Accelerator(const PropSets& props, Scene& scene) {
		scene.SetAccelerator(this);
	}

	Accelerator::~Accelerator() {

	}
}