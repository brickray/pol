#pragma once

#include "../core/light.h"

namespace pol {
	class Infinite : public Light {
	public:
		Infinite(int w, int h, const vector<Vector3f>& data);

		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;

		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const;

		virtual string ToString() const;
	};
}