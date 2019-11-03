#include "roughconductor.h"

namespace pol {
	RoughConductor::RoughConductor(Texture* specular, Texture* alphaX, Texture* alphaY, const Vector3f& eta, const Vector3f& k)
		:specular(specular)
		, alphaX(alphaX), alphaY(alphaY)
		, eta(eta), k(k) {

	}

	bool RoughConductor::IsDelta() const {
		return false;
	}

	void RoughConductor::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		Float ax = alphaX->Evaluate(isect).x;
		Float ay = alphaY->Evaluate(isect).x;
		Vector3f wh = SampleWh(u, ax, ay);
		if (Frame::CosTheta(in) < 0)
			wh = -wh;
		out = Reflect(in, wh);
		if (Frame::CosTheta(in) * Frame::CosTheta(out) < 0) {
			pdf = 0;
			return;
		}

		Float cosi = Dot(out, wh);
		Vector3f F = ConductFresnel(fabs(cosi), eta, k);
		Float D = GGXD(wh, ax, ay);
		Float G = GGXG(in, out, wh, ax, ay);
		fr = specular->Evaluate(isect) * F * D * G /
			(4 * Frame::AbsCosTheta(in) * Frame::AbsCosTheta(out));
		pdf = PdfWh(wh, ax, ay) / (4 * fabs(Dot(in, wh)));
	}

	void RoughConductor::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		if (Frame::CosTheta(in) * Frame::CosTheta(out) < 0) {
			pdf = 0;
			return;
		}

		Float ax = alphaX->Evaluate(isect).x;
		Float ay = alphaY->Evaluate(isect).x;
		Vector3f wh = Normalize(in + out);
		Float cosi = Dot(out, wh);
		Vector3f F = ConductFresnel(fabs(cosi), eta, k);
		Float D = GGXD(wh, ax, ay);
		Float G = GGXG(in, out, wh, ax, ay);
		fr = specular->Evaluate(isect) * F * D * G /
			(4 * Frame::AbsCosTheta(in) * Frame::AbsCosTheta(out));
		pdf = PdfWh(wh, ax, ay) / (4 * fabs(Dot(in, wh)));
	}

	string RoughConductor::ToString() const {
		string ret;
		ret += "RoughConductor[\n  specular = " + indent(specular->ToString())
			+ ",\n  alphaX = " + indent(alphaX->ToString())
			+ ",\n  alphaY = " + indent(alphaY->ToString())
			+ ",\n  eta = " + eta.ToString()
			+ ",\n  k = " + k.ToString()
			+ "\n]";

		return ret;
	}

	RoughConductor* CreateRoughConductorBsdf(Texture* specular, Texture* alphaX, Texture* alphaY, const Vector3f& eta, const Vector3f& k) {
		return new RoughConductor(specular, alphaX, alphaY, eta, k);
	}


}