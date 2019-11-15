#pragma once

#include "../core/bsdf.h"

namespace pol {
	class FresnelBlend : public Bsdf {
	private:
		Texture* diffuse;
		Texture* specular;
		Texture* alphaX;
		Texture* alphaY;

		Float diffuseWeight;

	public:
		FresnelBlend(const PropSets& props, Scene& scene);

		virtual bool IsDelta() const;
		virtual void SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const;
		virtual void Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const;

		virtual string ToString() const;
	};
}