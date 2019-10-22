#include "distant.h"

namespace pol {
	Distant::Distant(const Vector3f& radiance, const Vector3f& direction)
		:radiance(radiance), direction(direction) {

	}

	bool Distant::IsDelta() const {
		return true;
	}

	Float Distant::Luminance() const {
		return 0;
	}

	void Distant::SampleLight(const Intersection& isect, const Vector3f& in, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {
		rad = radiance;
		pdf = 1;
		shadowRay = Ray(isect.p, -direction);
	}

	Float Distant::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		return 0;
	}

	string Distant::ToString() const {
		string ret;
		ret += "Distant[\n  radiance = " + radiance.ToString()
			+ ",\n  direction = " + direction.ToString()
			+ "\n]";

		return ret;
	}

	Distant* CreateDistantLight(const Vector3f& radiance, const Vector3f& direction) {
		return new Distant(radiance, Normalize(direction));
	}
}