#include "shape.h"
#include "scene.h"

namespace pol {
	Shape::Shape(const PropSets& props, Scene& scene) {
		string bsdfName = props.GetString("bsdf");
		bsdf = scene.GetBsdf(bsdfName);

		light = nullptr;

		//add shape to scene
		scene.AddPrimitive(this);
	}

	Shape::~Shape() {

	}
}