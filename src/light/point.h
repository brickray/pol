#pragma once

#include "../core/light.h"

namespace pol {
	class Point : public Light {
	private:
		Vector3f radiance;
		Vector3f position;

	public:
		Point(const Vector3f& radiance, const Vector3f& position);

		virtual bool IsDelta() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;

		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const;

		virtual string ToString() const;
	};

	Point* CreatePointLight(const Vector3f& radiance, const Vector3f& position);
}