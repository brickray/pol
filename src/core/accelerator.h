#pragma once

#include "object.h"

namespace pol {
	class Intersection;
	class Shape;
	class Accelerator : public PolObject {
	public:
		Accelerator(const PropSets& props, Scene& scene);
		virtual ~Accelerator();

		virtual BBox GetRootBBox() const = 0;
		virtual int GetNodesCount() const = 0;
		virtual bool Build(const vector<Shape*>& primitives) = 0;
		virtual bool Intersect(Ray& ray, Intersection& isect) const = 0;
		virtual bool Occluded(const Ray& ray) const = 0;
	};
}