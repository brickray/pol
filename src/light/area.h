#pragma once

#include "../core/light.h"
#include "../core/shape.h"

namespace pol {
	class Area : public Light {
	private:
		Vector3f radiance;
		Shape* shape;
		bool twoside;

	public:
		Area(const PropSets& props, Scene& scene);

		virtual void SetShape(Shape* s);
		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;
		virtual void SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Ray& emitRay, Float& pdfW, Float& pdfA) const;

		virtual Vector3f Le(const Vector3f& in, const Vector3f& nor) const;

		virtual Float Pdf(const Intersection& isect, const Vector3f& pOnSurface) const;

		virtual string ToString() const;
	};
}