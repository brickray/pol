#include "fresnelblend.h"

namespace pol {
	FresnelBlend::FresnelBlend(Texture* diffuse, Texture* specular, Texture* alphaX, Texture* alphaY)
		:diffuse(diffuse), specular(specular)
		, alphaX(alphaX), alphaY(alphaY) {
		Intersection isect;
		isect.uv = Vector2f(0.5, 0.5);
		Float diff = GetLuminance(diffuse->Evaluate(isect));
		Float spec = GetLuminance(specular->Evaluate(isect));
		if (diff == 0 || spec == 0) {
			diffuseWeight = 0.5;
		}
		else {
			diffuseWeight = diff / (diff + spec);
		}
	}

	bool FresnelBlend::IsDelta() const {
		return false;
	}

	void FresnelBlend::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		bool sameSide = Frame::CosTheta(in) > 0;

		Float ax = alphaX->Evaluate(isect).X();
		Float ay = alphaY->Evaluate(isect).X();

		if (u.x < diffuseWeight) {
			//diffuse part
			Float ux = u.x / diffuseWeight;
			out = Warp::CosineHemiSphere(Vector2f(ux, u.y));
			if (!sameSide) out = -out;
		}
		else {
			//specular part
			Float ux = (u.x - diffuseWeight) / (1 - diffuseWeight);
			Vector3f wh = SampleWh(Vector2f(ux, u.y), ax, ay);
			out = Reflect(in, wh);
			if (Frame::CosTheta(in) * Frame::CosTheta(out) < 0) {
				//to prevent light leak
				//it may occur at grazing angle
				pdf = 0;
				return;
			}
		}

		Float c0 = Frame::AbsCosTheta(in);
		Float c1 = Frame::AbsCosTheta(out);
		Vector3f rd = diffuse->Evaluate(isect);
		Vector3f rs = specular->Evaluate(isect);
		Float cons0 = 1 - 0.5 * c0;
		Float cons1 = 1 - 0.5 * c1;
		Vector3f wh = Normalize(in + out);
		Vector3f diff = Float(28 / (23 * PI)) * rd * (Vector3f::One() - rs) *
			(1 - Pow5(cons0)) * (1 - Pow5(cons1));
		Float D = GGXD(wh, ax, ay);
		Vector3f spec = D * SchlickFresnel(rs, fabs(Dot(out, wh))) / (4 * fabs(Dot(out, wh)) * Max(c0, c1));
		fr = (diff + spec) * c1;
		Float diffPdf = c1 * INVPI;
		Float specPdf = D * Frame::AbsCosTheta(wh) / (4 * fabs(Dot(in, wh)));
		pdf = Lerp(diffPdf, specPdf, diffuseWeight);
	
		return;
	}

	void FresnelBlend::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		if (Frame::CosTheta(in) * Frame::AbsCosTheta(out) < 0) {
			//to prevent light leak
			pdf = 0;
			return;
		}

		Float ax = alphaX->Evaluate(isect).X();
		Float ay = alphaY->Evaluate(isect).X();
		Float c0 = Frame::AbsCosTheta(in);
		Float c1 = Frame::AbsCosTheta(out);
		Vector3f rd = diffuse->Evaluate(isect);
		Vector3f rs = specular->Evaluate(isect);
		Float cons0 = 1 - 0.5 * c0;
		Float cons1 = 1 - 0.5 * c1;
		Vector3f wh = Normalize(in + out);
		Vector3f diff = Float(28 / (23 * PI)) * rd * (Vector3f::One() - rs) *
			(1 - Pow5(cons0)) * (1 - Pow5(cons1));
		Float D = GGXD(wh, ax, ay);
		Vector3f spec = D * SchlickFresnel(rs, fabs(Dot(out, wh))) / (4 * fabs(Dot(out, wh)) * Max(c0, c1));
		fr = (diff + spec) * c1;
		Float diffPdf = c1 * INVPI;
		Float specPdf = D * Frame::AbsCosTheta(wh) / (4 * fabs(Dot(in, wh)));
		pdf = Lerp(diffPdf, specPdf, diffuseWeight);
		
		return;
	}

	string FresnelBlend::ToString() const {
		string ret;
		ret += "FresnelBlend[\n  diffuse = " + indent(diffuse->ToString())
			+ ",\n  specular = " + indent(specular->ToString())
			+ ",\n  alphaX = " + indent(alphaX->ToString())
			+ ",\n  alphaY = " + indent(alphaY->ToString())
			+ ",\n  diffuse weight = " + to_string(diffuseWeight)
			+ "\n]";

		return ret;
	}

	FresnelBlend* CreateFresnelBlendBsdf(Texture* diffuse, Texture* specular, Texture* alphaX, Texture* alphaY) {
		return new FresnelBlend(diffuse, specular, alphaX, alphaY);
	}
}