#pragma once

#include "object.h"
#include "intersection.h"
#include "bsdf.h"

namespace pol {
	class Light;
	class Scene;
	class Shape : public PolObject {
	protected:
		//material of shape
		Bsdf* bsdf;
		//light pointer
		Light* light;

	public:
		Shape() {}
		Shape(const PropSets& props, Scene& scene);
		virtual ~Shape();

		virtual Float SurfaceArea() const = 0;
		virtual BBox WorldBBox() const = 0;
		//return true if ray intersect with shape
		virtual bool Intersect(Ray& ray, Intersection& isect) const = 0;
		//shadow ray test
		virtual bool Occluded(const Ray& ray) const = 0;
		//sample shape
		//soldAngle : pdf in which type (area or solidAngle)
		virtual void SampleShape(const Vector2f& u, /*in out*/Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const = 0;
		//soldAngle : pdf in which type (area or solidAngle)
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const = 0;

		virtual void SetLight(Light* l) { light = l; }
		const Bsdf* GetBsdf() const { return bsdf;  }
	};
}