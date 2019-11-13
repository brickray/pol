#pragma once

#include "../pol.h"
#include "object.h"
#include "intersection.h"

namespace pol {
	class Scene;
	class Light : public PolObject {
	public:
		Light();
		virtual ~Light();

		//for distant and infinite light
		virtual void Prepare(const Scene& scene) {};
		//point, distant, and spot light are delta
		//point ---- position delta
		//distant -- direction delta
		//spot ----- position delta
		virtual bool IsDelta() const = 0;
		//return the light is infinite light
		virtual bool IsInfinite() const = 0;
		//d¦Õ = Li * dw * dA * cos(t)
		//¦Õ = ¡ÒLi*cos(t)*dw*dA
		virtual Float Luminance() const = 0;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const = 0;

		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const = 0;

		virtual Vector3f Le(const Vector3f& in, const Vector3f& nor) const {
			return Vector3f::Zero();
		}
	};
}