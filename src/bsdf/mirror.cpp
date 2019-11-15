#include "mirror.h"
#include "../core/scene.h"

namespace pol {
	POL_REGISTER_CLASS(Mirror, "mirror");

	Mirror::Mirror(const PropSets& props, Scene& scene)
		:Bsdf(props, scene) {
		string specName = props.GetString("specular");
		specular = scene.GetTexture(specName);
	}

	//specular reflection bsdf is a delta bsdf
	bool Mirror::IsDelta() const {
		return true;
	}

	void Mirror::SampleBsdf(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& out, Vector3f& fr, Float& pdf) const {
		out = Vector3f(-in.X(), in.Y(), -in.Z());
		fr = specular->Evaluate(isect);
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
}