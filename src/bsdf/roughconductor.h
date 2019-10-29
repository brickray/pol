#pragma once

#include "../core/bsdf.h"

namespace pol {
	class RoughConductor : public Bsdf {
	private:
		Texture* specular;
		Texture* alphaX;
		Texture* alphaY;
		Vector3f eta, k;

	public:
		RoughConductor(Texture* specular, Texture* alphaX, Texture* alphaY, const Vector3f& eta, const Vector3f& k);

		virtual bool IsDelta() const;
		virtual void SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const;
		virtual void Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const;

		virtual string ToString() const;
	};

	RoughConductor* CreateRoughConductorBsdf(Texture* specular, Texture* alphaX, Texture* alphaY, const Vector3f& eta, const Vector3f& k);
}