#include "roughconductor.h"
#include "../core/scene.h"

namespace pol {
	POL_REGISTER_CLASS(RoughConductor, "roughconductor");

	RoughConductor::RoughConductor(const PropSets& props, Scene& scene)
		:Bsdf(props, scene) {
		string specName = props.GetString("specular");
		specular = scene.GetTexture(specName);
		bool isotropic = props.HasValue("alpha");
		if (isotropic) {
			string alphaName = props.GetString("alpha");
			alphaX = scene.GetTexture(alphaName);
			alphaY = alphaX;
		}
		else {
			string axName = props.GetString("alphaX");
			string ayName = props.GetString("alphaY");
			alphaX = scene.GetTexture(axName);
			alphaY = scene.GetTexture(ayName);
		}
		eta = props.GetVector3f("eta");
		k = props.GetVector3f("k");
	}

	bool RoughConductor::IsDelta() const {
		return false;
	}

	void RoughConductor::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		Float ax = alphaX->Evaluate(isect).X();
		Float ay = alphaY->Evaluate(isect).X();
		Vector3f wh = SampleWh(u, ax, ay);
		out = Reflect(in, wh);
		if (Frame::CosTheta(in) * Frame::CosTheta(out) < 0) {
			//it may occur at grazing angle
			pdf = 0;
			return;
		}

		Float cosi = Dot(out, wh);
		Vector3f F = ConductFresnel(fabs(cosi), eta, k);
		Float D = GGXD(wh, ax, ay);
		Float G = GGXG(in, out, ax, ay);
		fr = specular->Evaluate(isect) * F * D * G /
			(4 * Frame::AbsCosTheta(in));
		pdf = D * Frame::AbsCosTheta(wh) / (4 * fabs(Dot(in, wh)));
	}

	void RoughConductor::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		if (Frame::CosTheta(in) * Frame::CosTheta(out) < 0) {
			pdf = 0;
			return;
		}

		Float ax = alphaX->Evaluate(isect).X();
		Float ay = alphaY->Evaluate(isect).X();
		Vector3f wh = Normalize(in + out);
		Float cosi = Dot(out, wh);
		Vector3f F = ConductFresnel(fabs(cosi), eta, k);
		Float D = GGXD(wh, ax, ay);
		Float G = GGXG(in, out, ax, ay);
		fr = specular->Evaluate(isect) * F * D * G /
			(4 * Frame::AbsCosTheta(in));
		pdf = D * Frame::AbsCosTheta(wh) / (4 * fabs(Dot(in, wh)));
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
}