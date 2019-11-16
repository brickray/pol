#include "distant.h"
#include "../core/scene.h"

namespace pol {
	POL_REGISTER_CLASS(Distant, "distant");

	Distant::Distant(const PropSets& props, Scene& scene)
		:Light(props, scene) {
		radiance = props.GetVector3f("radiance", Vector3f::Zero());
		direction = Normalize(props.GetVector3f("direction", Vector3f::Up()));
	}

	void Distant::Prepare(const Scene& scene) {
		scene.GetBBox().BoundingSphere(center, radius);
	}

	bool Distant::IsDelta() const {
		return true;
	}

	bool Distant::IsInfinite() const {
		return false;
	}

	//power = ¡Ò¡ÒLe*cos(t)*dW*dA
	//since Le is constant, and delta direction
	//power = Le¡ÒdA
	//      = Le*A
	//      = Le*PI*radius*radius
	Float Distant::Luminance() const {
		Vector3f power = Float(PI) * radius * radius * radiance;

		return GetLuminance(power);
	}

	void Distant::SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {
		rad = radiance;
		pdf = 1;
		shadowRay = Ray(isect.p, -direction);
	}

	Float Distant::Pdf(const Intersection& isect, const Vector3f& pOnSurface) const {
		return 0;
	}

	string Distant::ToString() const {
		string ret;
		ret += "Distant[\n  radiance = " + radiance.ToString()
			+ ",\n  direction = " + direction.ToString()
			+ "\n]";

		return ret;
	}
}