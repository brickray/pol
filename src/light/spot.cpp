#include "spot.h"
#include "../core/warp.h"

namespace pol {
	POL_REGISTER_CLASS(Spot, "spot");

	Spot::Spot(const PropSets& props, Scene& scene)
		:Light(props, scene) {
		radiance = props.GetVector3f("radiance", Vector3f::Zero());
		position = props.GetVector3f("position", Vector3f::Zero());
		direction = Normalize(props.GetVector3f("direction", Vector3f::Up()));
		total = cos(Radians(props.GetFloat("total", 45)));
		falloff = cos(Radians(props.GetFloat("falloff", 30)));
	}

	bool Spot::IsDelta() const {
		return true;
	}

	bool Spot::IsInfinite() const {
		return false;
	}

	//power = ¡Ò¡ÒLe*cos(t)*dW*dA
	//since Le is constant, and delta position
	//power = Le¡ÒdW from 0 to thetamax
	//      = 2*PI*Le*(1-cos(tmax))
	Float Spot::Luminance() const {
		Vector3f power = radiance * Float(TWOPI * (1 - (total + falloff) * 0.5));

		return GetLuminance(power);
	}

	void Spot::SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {
		Vector3f dir = position - isect.p;
		Float lensq = dir.LengthSquare();
		Float len = sqrtf(lensq);
		dir = Normalize(dir);
		Float costheta = Dot(-dir, direction);
		Float attenuation = getFalloff(costheta);
		if (attenuation == 0) {
			pdf = 0;
			return;
		}
		rad = radiance * attenuation;
		pdf = lensq;
		shadowRay = Ray(isect.p, dir, Epsilon, len - Epsilon);
	}

	void Spot::SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Ray& emitRay, Float& pdfW, Float& pdfA) const {
		Vector3f dir = Warp::UniformCone(dirSample, total);
		Frame frame(direction);
		dir = frame.ToWorld(dir);

		rad = radiance;
		emitRay = Ray(position, dir);
		pdfA = 1;
		pdfW = Warp::UniformConePdf(total);
	}

	Float Spot::Pdf(const Intersection& isect, const Vector3f& pOnSurface) const {
		return 0;
	}

	Float Spot::getFalloff(Float val) const {
		if (val < total) return 0;
		if (val > falloff) return 1;

		return (val - total) / (falloff - total);
	}

	string Spot::ToString() const {
		string ret;
		ret += "Spot[\n  radiance = " + radiance.ToString()
			+ ",\n  position = " + position.ToString()
			+ ",\n  direction = " + direction.ToString()
			+ "\n]";

		return ret;
	}
}