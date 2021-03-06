#pragma once

#include "../core/light.h"
#include "../core/mipmap.h"
#include "../core/distribution.h"

namespace pol {
	class Infinite : public Light {
	private:
		Mipmap image;
		Distribution2D distribution;

		//local to world transform
		Transform world;

		//bounding sphere of scene
		Vector3f center;
		Float radius;

	public:
		Infinite(const PropSets& props, Scene& scene);

		virtual void Prepare(const Scene& scene);
		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;
		virtual void SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Vector3f& nor, Ray& emitRay, Float& pdfA, Float& pdfW) const;
		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const;
		virtual Vector3f Le(const Vector3f& in, const Vector3f& nor) const;

		virtual string ToString() const;
	};
}