#pragma once

#include "../core/light.h"

namespace pol {
	class Distant : public Light {
	private:
		Vector3f radiance;
		Vector3f direction;

		//bounding sphere of scene
		Vector3f center;
		Float radius;

	public:
		Distant(const PropSets& props, Scene& scene);

		virtual void Prepare(const Scene& scene);
		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;
		virtual void SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Ray& emitRay, Float& pdfW, Float& pdfA) const;

		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const;

		virtual string ToString() const;
	};
}