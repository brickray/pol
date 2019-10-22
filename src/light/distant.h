#pragma once

#include "../core/light.h"

namespace pol {
	class Distant : public Light {
	private:
		Vector3f radiance;
		Vector3f direction;

	public:
		Distant(const Vector3f& radiance, const Vector3f& direction);

		virtual bool IsDelta() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;

		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const;

		virtual string ToString() const;
	};

	Distant* CreateDistantLight(const Vector3f& radiance, const Vector3f& direction);
}