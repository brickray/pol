#include "distant.h"
#include "../core/scene.h"
#include "../core/warp.h"

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

	void Distant::SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Ray& emitRay, Float& pdfW, Float& pdfA) const {
		Vector2f uv = Warp::ConcentricDisk(posSample) * radius;
		Vector3f pos = Vector3f(uv.x, 0, uv.y);
		Frame frame(direction);
		pos = frame.ToWorld(pos);
		pos += center;
		pos -= direction * radius;

		rad = radiance;
		emitRay = Ray(pos, direction);
		pdfA = 1 / (PI * radius * radius);
		pdfW = 1;
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