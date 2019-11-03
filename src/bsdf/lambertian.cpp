#include "lambertian.h"

namespace pol {
	Lambertian::Lambertian(Texture* diffuse)
		:diffuse(diffuse) {

	}

	bool Lambertian::IsDelta() const {
		return false;
	}

	void Lambertian::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		out = Warp::CosineHemiSphere(u);
		pdf = Warp::CosineHemiSpherePdf(out);
		fr = diffuse->Evaluate(isect) * Float(INVPI);

		if (!(isect.shFrame.CosTheta(in) * Frame::CosTheta(out) > 0))
			out = -out;
	}

	void Lambertian::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		if (!(Frame::CosTheta(in) * Frame::CosTheta(out) > 0)) {
			pdf = 0;
			return;
		}

		fr = diffuse->Evaluate(isect) * Float(INVPI);
		pdf = Frame::AbsCosTheta(out) * INVPI;
	}

	string Lambertian::ToString() const {
		string ret;
		ret += "Lambertian[\n  diffuse = " + indent(diffuse->ToString())
			+ "\n]";

		return ret;
	}

	Lambertian* CreateLambertianBsdf(Texture* diffuse) {
		return new Lambertian(diffuse);
	}
}