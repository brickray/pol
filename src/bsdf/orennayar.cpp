#include "orennayar.h"
#include "../core/scene.h"

namespace pol {
	POL_REGISTER_CLASS(OrenNayar, "orennayar");

	//fr = R/pi * (A + B*max(0,cos(phii - phio))*sin(a)*tan(b))
	// A = 1 - sigma2/(2*(sigma2+0.33))
	// B = 0.45*sigma2/(sigma2+0.09)
	// a = max(thetai, thetao)
	// b = min(thetai, thetao)
	OrenNayar::OrenNayar(const PropSets& props, Scene& scene)
		:Bsdf(props, scene) {
		string diffName = props.GetString("diffuse");
		diffuse = scene.GetTexture(diffName);

		sigma = props.GetFloat("sigma", 1);

		sigma = Max(Float(0), sigma);
		Float sigma2 = sigma * sigma;
		A = 1 - (sigma2 / (2 * (sigma2 + 0.33)));
		B = 0.45 * sigma2 / (sigma2 + 0.09);
	}

	bool OrenNayar::IsDelta() const {
		return false;
	}

	void OrenNayar::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		out = Warp::CosineHemiSphere(u);
		pdf = Warp::CosineHemiSpherePdf(out);
		Float c1 = Frame::CosTheta(out);

		Float sinthetai = Frame::AbsSinTheta(in);
		Float sinthetao = Frame::AbsSinTheta(out);
		Float sinphii = Frame::SinPhi(in);
		Float cosphii = Frame::CosPhi(in);
		Float sinphio = Frame::SinPhi(out);
		Float cosphio = Frame::CosPhi(out);
		Float dcos = cosphii * cosphio + sinphii * sinphio;
		if (dcos < 0) dcos = 0;
		Float sina, tanb;
		if (sinthetai > sinthetao) {
			sina = sinthetai;
			tanb = sinthetao / Frame::AbsCosTheta(out);
		}
		else {
			sina = sinthetao;
			tanb = sinthetai / Frame::AbsCosTheta(in);
		}

		fr = diffuse->Evaluate(isect) * Float(INVPI) *
			(A + B * dcos * sina * tanb) * fabs(c1);

		if (!(isect.shFrame.CosTheta(in) * c1 > 0))
			out = -out;
	}

	void OrenNayar::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		Float c1 = Frame::CosTheta(out);
		if (!(Frame::CosTheta(in) * c1 > 0)) {
			pdf = 0;
			return;
		}

		Float sinthetai = Frame::AbsSinTheta(in);
		Float sinthetao = Frame::AbsSinTheta(out);
		Float sinphii = Frame::SinPhi(in);
		Float cosphii = Frame::CosPhi(in);
		Float sinphio = Frame::SinPhi(out);
		Float cosphio = Frame::CosPhi(out);
		Float dcos = cosphii * cosphio + sinphii * sinphio;
		if (dcos < 0) dcos = 0;
		Float sina, tanb;
		if (sinthetai > sinthetao) {
			sina = sinthetai;
			tanb = sinthetao / Frame::AbsCosTheta(out);
		}
		else {
			sina = sinthetao;
			tanb = sinthetai / Frame::AbsCosTheta(in);
		}

		fr = diffuse->Evaluate(isect) * Float(INVPI) *
			(A + B * dcos * sina * tanb) * fabs(c1);
		pdf = Frame::AbsCosTheta(out) * INVPI;
	}

	string OrenNayar::ToString() const {
		string ret;
		ret += "OrenNayar[\n  diffuse = " + indent(diffuse->ToString())
			+ "\n]";

		return ret;
	}
}