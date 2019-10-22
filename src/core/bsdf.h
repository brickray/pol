#pragma once

#include "../pol.h"
#include "object.h"
#include "texture.h"
#include "intersection.h"
#include "warp.h"

namespace pol {
	class Bsdf : public PolObject {
	public:
		Bsdf();
		virtual ~Bsdf();

		virtual bool IsDelta() const = 0;
		virtual void SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const = 0;
		virtual void Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const = 0;
	};

	__forceinline Float DielectricFresnel(Float cosi, Float cost, Float etai, Float etat) {
		float Rparl = (etat * cosi - etai * cost) / (etat * cosi + etai * cost);
		float Rperp = (etai * cosi - etat * cost) / (etai * cosi + etat * cost);

		return (Rparl * Rparl + Rperp * Rperp) * 0.5f;
	}

	__forceinline Vector3f ConductFresnel(Float cosi, const Vector3f& eta, const Vector3f& k) {
		Vector3f tmp = (eta * eta + k * k) * cosi * cosi;
		Vector3f Rparl2 = (tmp - eta * cosi * Float(2) + Float(1)) /
			(tmp + eta * cosi * Float(2) + Float(1));
		Vector3f tmp_f = (eta * eta + k * k);
		Vector3f Rperp2 = (tmp_f - eta * cosi * Float(2) + cosi * cosi) /
			(tmp_f + eta * cosi * Float(2) + cosi * cosi);
		return (Rparl2 + Rperp2) * Float(0.5);
	}

	__forceinline Vector3f SchlickFresnel(const Vector3f& specular, Float costheta) {
		Vector3f rs = specular;
		Float c = 1 - costheta;
		return rs + c * c * c * c * c * (Vector3f::one - rs);
	}
}
