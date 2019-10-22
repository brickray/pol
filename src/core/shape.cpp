#include "shape.h"

namespace pol {
	Shape::Shape(const Transform& world, Bsdf* bsdf)
		:world(world), bsdf(bsdf) {

	}

	Shape::~Shape() {

	}
}