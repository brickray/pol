#pragma once

#include "../core/bsdf.h"

namespace pol {
	class Lambertian : public Bsdf {
	private:
		Texture* diffuse;

	public:
		Lambertian(Texture* diffuse);

		virtual bool IsDelta() const;
		virtual void SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const;
		virtual void Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const;

		virtual string ToString() const;
	};

	Lambertian* CreateLambertianBsdf(Texture* diffuse);
}