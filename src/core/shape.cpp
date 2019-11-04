#include "shape.h"

namespace pol {
	Shape::Shape(Bsdf* bsdf)
		: bsdf(bsdf), light(nullptr) {

	}

	Shape::~Shape() {

	}
}