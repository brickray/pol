#pragma once

#include "../core/light.h"

namespace pol {
	class Point : public Light {
	private:
		Vector3f radiance;
		Vector3f position;

	public:
		Point(const PropSets& props, Scene& scene);

		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;
		virtual void SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Ray& emitRay, Float& pdfW, Float& pdfA) const;
		
		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const;

		virtual string ToString() const;
	};
}