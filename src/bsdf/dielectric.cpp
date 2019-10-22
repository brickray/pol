#include "dielectric.h"

namespace pol {
	Dielectric::Dielectric(Texture* specular)
		:specular(specular) {

	}

	bool Dielectric::IsDelta() const {
		return true;
	}

	void Dielectric::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		out = Vector3f(-in.x, in.y, -in.z);
		fr = specular->Evaluate(isect.uv) / isect.shFrame.CosTheta(out);
		pdf = 1;
	}

	void Dielectric::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		pdf = 0;
		return;
	}

	string Dielectric::ToString() const {
		string ret;
		ret += "Dielectric[\n  specular = " + specular->ToString()
			+ "\n]";

		return ret;
	}

	Dielectric* CreateDielectricBsdf(Texture* specular) {
		return new Dielectric(specular);
	}
}