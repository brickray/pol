#include "spot.h"

namespace pol {
	Spot::Spot(const Vector3f& radiance, const Vector3f& position, const Vector3f& direction,
		Float total, Float falloff)
		:radiance(radiance), position(position)
		, direction(direction), total(cos(Radians(total)))
		, falloff(cos(Radians(falloff))) {

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

	Float Spot::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
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

	Spot* CreateSpotLight(const Vector3f& radiance, const Vector3f& position, const Vector3f& direction, Float total, Float falloff) {
		return new Spot(radiance, position, Normalize(direction), total, falloff);
	}
}