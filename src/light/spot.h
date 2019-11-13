#pragma once

#include "../core/light.h"

namespace pol {
	class Spot : public Light {
	private:
		Vector3f radiance;
		Vector3f position;
		Vector3f direction;
		Float total;
		Float falloff;

	public:
		Spot(const Vector3f& radiance, const Vector3f& position, const Vector3f& direction,
			Float total, Float falloff);

		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;

		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const;

		virtual string ToString() const;

	private:
		Float getFalloff(Float val) const;
	};

	Spot* CreateSpotLight(const Vector3f& radiance, const Vector3f& position, const Vector3f& direction, Float total = 45, Float falloff = 30);
}