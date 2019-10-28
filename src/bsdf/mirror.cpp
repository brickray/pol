#include "mirror.h"

namespace pol {
	Mirror::Mirror(Texture* specular)
		:specular(specular) {

	}

	//specular reflection bsdf is a delta bsdf
	bool Mirror::IsDelta() const {
		return true;
	}

	void Mirror::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		out = Vector3f(-in.x, in.y, -in.z);
		fr = specular->Evaluate(isect) / isect.shFrame.CosTheta(out);
		pdf = 1;
	}

	void Mirror::Fr(const Intersection& isect, const Vector3f& in, const Vector3f& out, Vector3f& fr, Float& pdf) const {
		pdf = 0;
		return;
	}

	string Mirror::ToString() const {
		string ret;
		ret += "Mirror[\n  specular = " + specular->ToString()
			+ "\n]";

		return ret;
	}

	Mirror* CreateMirrorBsdf(Texture* specular) {
		return new Mirror(specular);
	}
}