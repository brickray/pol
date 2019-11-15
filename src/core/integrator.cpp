#include "integrator.h"
#include "scene.h"

namespace pol {
	Integrator::Integrator(const PropSets& props, Scene& scene) {
		scene.SetIntegrator(this);
	}

	Integrator::~Integrator() {

	}
}