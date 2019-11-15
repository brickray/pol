#include "bsdf.h"
#include "scene.h"

namespace pol {
	Bsdf::Bsdf(const PropSets& props, Scene& scene) {
		string bsdfName = props.GetString("name");
		scene.AddBsdf(bsdfName, this);
	}

	Bsdf::~Bsdf() {

	}
}