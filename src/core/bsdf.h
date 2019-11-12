#pragma once

#include "../pol.h"
#include "object.h"
#include "texture.h"
#include "intersection.h"
#include "warp.h"
#include "frame.h"

namespace pol {
	class Bsdf : public PolObject {
	public:
		Bsdf();
		virtual ~Bsdf();

		virtual bool IsDelta() const = 0;
		//evaluate Bsdf*Cos(theta)
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
		return rs + c * c * c * c * c * (Vector3f::One() - rs);
	}

	//microfacet model
	//
	//
	//D(wm) is normal distribution function
	//              dA(wh) = D(wh)*dwh*dA
	//microfacet distribution functions must be normalized to ensure that they are physically
	//plausible. Intuitively, if we consider incident rays on the microsurface along the normal
	//direction n, then each ray must intersect the microfacet surface exactly one. More formally
	//givel a differential area of the microsurface, dA, then the projected area of the microfacet 
	//faces above that area must be equal to dA. Mathematically, this corresponds to the following 
	//requirement:
	//              ¡ÒD(wm)cos(h)dw = 1
	//
	//the GGX normal distribution function are given as follow:
	//            D(wh) = alpha^2 / (PI*(cos(wh)^2*(alpha^2 - 1) + 1)^2)
	//and its anisotropic variant is given by
	//            D(wh) = 1 / (PI*alphax*alphay*cos(wh)^4*(1 + tan(wh)^2*(cos(wh)^2/alphax^2 + sin(wh)^2/alphay^2))^2
	//here we proof the isotropic ggx ndf is normalized 
	//      ¡ÒD(wm)cos(h)dw = ¡Òalpha^2*cos(wh)/(PI*cos(wh)^4*(alpha^2 + tan(wh)^2)^2) * d(wh)
	//note that d(wh) = sin(wh)*d(theta)d(phi),
	//      ¡ÒD(wm)cos(h)dw = 2*alpha^2*¡Òcos(wh)*sin(wh)/(cos(wh)^4*(alpha^2+tan(wh)^2)^2)*d(wh)
	//                      = alpha^2*¡Ò1/(cos(wh)^4*(alpha^2+tan(wh)^2)^2*d(sin(wh)^2)
	//                      = alpha^2*¡Ò1/((cos(wh)^2*alpha^2+sin(wh)^2)^2)*d(sin(wh)^2)
	//let u = sin(wh)^2, and the identity sin(wh)^2 + cos(wh)^2 = 1, we get
	//                      = alpha^2*¡Ò1/((1-u)*alpha^2 + u)^2 * du
    //the primitive function of f(u) = alpha^2/((1-u)*alpha^2 + u)^2 can easily found
	//                 F(u) = alpha^2/((alpha^2 - 1) * ((1 - u)*alpha^2 + u))
	//where u is from 0 to 1
	//so the definite integral of F(u) is equal to F(1) - F(0) = 1
	//thus, the ggx normal distribution function is normalized
	//here is the code
	__forceinline Float GGXD(const Vector3f& wh, Float alphaX, Float alphaY) {
		/*if (alphaX == alphaY) {
			//isotropic
			Float alpha2 = alphaX * alphaX;
			Float squared = cosTheta * cosTheta * (alpha2 - 1) + 1;
			Float denominator = PI * squared * squared;
			return alpha2 / denominator;
		}
		else*/ {
			//anisotropic
			Float cosTheta2 = Frame::CosTheta2(wh);
			Float sinTheta2 = Clamp(1 - cosTheta2, Float(0), Float(1));
			Float tanTheta2 = sinTheta2 / cosTheta2;
			if (isinf(tanTheta2)) return 0;
			Float squared = 1 + tanTheta2 * (cosTheta2 / (alphaX * alphaX) + sinTheta2 / (alphaY * alphaY));
			Float denominator = PI * alphaX * alphaY * cosTheta2 * cosTheta2 * squared * squared;
			return 1 / denominator;
		}
	}

	//the distribution of microfacet normals alone isnot enough to fully characterize the microsurface
	//for rendering.It's also important to account for the fact that some microfacets will be invisible
	//from a given viewing or illumination direction because they are back-facing and also for the fact
	//that some of the forward-facing microfacet area will be hidden since it's shadowed by back-facing 
	//microfacets.These effects are accounted for by Smith's masking-shadowing function G1(w, wh), which
	//gives the fraction of microfacets with normals wh that are visible from direction w. Note that 
	//0 <= G1(w, wh) <= 1. In the usual case where the probability a microfacet is visible is independent 
	//of ths orientation wh, we can write this function as G1(w).
	//the G1 must satisify the following equation
	//        cos(wh) = ¡ÒG1(w)*max(0, w.wh)*D(wh)*d(wh)
	//In other words, the projected area of visible microfacets for a given direction w must be equal to
	//(w.n) = cos(wh) times the differential area of the macrosurface dA.
	//the GGX masking-shadowing function are given as follow
	//        G1(w) = 2 / (1 + sqrt(1 + alpha^2*tan(wh)^2))
	//and its anisotropic variant is given by
	//        G1(w) = 2 / (1 + sqrt(1 + tan(wh)^2*(cos(wh)^2*(alphax*alphax) + sin(wh)^2*(alphay*alphay)))
	__forceinline Float GGXG1(const Vector3f& w, const Vector3f& wh, Float alphaX, Float alphaY) {
		Float cosTheta2 = Frame::CosTheta2(w);
		Float sinTheta2 = Clamp(1 - cosTheta2, Float(0), Float(1));
		Float tanTheta2 = sinTheta2 / cosTheta2;
		if (isinf(tanTheta2)) return 0;
		/*if (alphaX == alphaY) {
			//isotropic
			Float sqrD = 1 + alphaX * alphaX * tanTheta2;
			return 2 / (1 + sqrt(sqrD));
		}
		else*/ {
			//anisotropic
			Float sqrD = 1 + tanTheta2 * (cosTheta2 * alphaX * alphaX + sinTheta2 * alphaY * alphaY);
			return 2 / (1 + sqrt(sqrD));
		}
	}

	//and the G(wo, wi) = G1(wo)*G1(wi)
	__forceinline Float GGXG(const Vector3f& wo, const Vector3f& wi, const Vector3f& wh, Float alphaX, Float alphaY) {
		return GGXG1(wo, wh, alphaX, alphaY) * GGXG1(wi, wh, alphaX, alphaY);
	}

	//the sample ggx distribution is easy
	//theta = acos(sqrt(1-u1)/(u1*(alpha^2-1)+1))
	//phi = 2pi*u2
	__forceinline Vector3f SampleWh(const Vector2f& u, Float alphaX, Float alphaY) {
		if (alphaX == alphaY) {
			//isotropic
			Float theta = acos(sqrt((1 - u.x) / (u.x * (alphaX * alphaX - 1) + 1)));
			Float phi = TWOPI * u.y;
			
			return SphericalCoordinate(theta, phi);
		}
		else {
			//anisotropic
		}
	}

	//pdf 
	__forceinline Float PdfWh(const Vector3f& wh, Float alphaX, Float alphaY) {
		return GGXD(wh, alphaX, alphaY) * Frame::AbsCosTheta(wh);
	}
}
