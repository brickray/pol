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
		Spot(const PropSets& props, Scene& scene);

		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;
		virtual void SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Vector3f& nor, Ray& emitRay, Float& pdfW, Float& pdfA) const;

		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const;

		virtual string ToString() const;

	private:
		Float getFalloff(Float val) const;
	};
}