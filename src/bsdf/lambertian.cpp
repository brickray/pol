#include "lambertian.h"
#include "../core/scene.h"

namespace pol {
	POL_REGISTER_CLASS(Lambertian, "lambertian");

	Lambertian::Lambertian(const PropSets& props, Scene& scene)
		:Bsdf(props, scene) {
		string diffName = props.GetString("diffuse");
		diffuse = scene.GetTexture(diffName);
	}

	bool Lambertian::IsDelta() const {
		return false;
	}

	void Lambertian::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		out = Warp::CosineHemiSphere(u);
		pdf = Warp::CosineHemiSpherePdf(out);
		Float c1 = Frame::CosTheta(out);
		fr = diffuse->Evaluate(isect) * Float(INVPI) * fabs(c1);

		if (!(isect.shFrame.CosTheta(in) * c1 > 0))
			out = -out;
	}

	void Lambertian::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		Float c1 = Frame::CosTheta(out);
		if (!(Frame::CosTheta(in) * c1 > 0)) {
			pdf = 0;
			return;
		}

		fr = diffuse->Evaluate(isect) * Float(INVPI) * fabs(c1);
		pdf = Frame::AbsCosTheta(out) * INVPI;
	}

	string Lambertian::ToString() const {
		string ret;
		ret += "Lambertian[\n  diffuse = " + indent(diffuse->ToString())
			+ "\n]";

		return ret;
	}
}