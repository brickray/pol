#pragma once

#include "../core/bsdf.h"

namespace pol {
	class Dielectric : public Bsdf {
	private:
		Texture* specular;
		Float etai, etat;

	public:
		Dielectric(Texture* specular, Float etai, Float etat);

		virtual bool IsDelta() const;
		virtual void SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const;
		virtual void Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const;

		virtual string ToString() const;
	};

	Dielectric* CreateDielectricBsdf(Texture* specular, Float etai, Float etat);
}