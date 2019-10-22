#pragma once

#include "object.h"
#include "intersection.h"
#include "bsdf.h"

namespace pol {
	class Light;
	class Shape : public PolObject {
	protected:
		//object to world transform
		Transform world;
		//material of shape
		Bsdf* bsdf;
		//light pointer
		Light* light;

	public:
		Shape(const Transform& world, Bsdf* bsdf);
		virtual ~Shape();

		virtual Float SurfaceArea() const = 0;
		virtual BBox WorldBBox() const = 0;
		//return true if ray intersect with shape
		virtual bool Intersect(Ray& ray, Intersection& isect) const = 0;
		//shadow ray test
		virtual bool Occluded(const Ray& ray) const = 0;
		//sample shape
		//soldAngle : pdf in which type (area or soldAngle)
		virtual void SampleShape(const Vector2f& u, /*in out*/Vector3f& pos, Vector3f& nor, Float& pdf, bool& soldAngle) const = 0;
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const = 0;

		void SetLight(Light* l) {
			light = l;
		}
	};
}