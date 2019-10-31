#include "point.h"

namespace pol {
	Point::Point(const Vector3f& radiance, const Vector3f& position)
		:radiance(radiance), position(position) {

	}

	bool Point::IsDelta() const {
		return true;
	}

	bool Point::IsInfinite() const {
		return false;
	}

	Float Point::Luminance() const {
		Vector3f power = Float(FOURPI) * radiance;

		return GetLuminance(power);
	}

	//Lo = ¡Òfr*Li*cos(t)*dw
	void Point::SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {
		Vector3f dir = position - isect.p;
		Float lensq = dir.LengthSquare();
		Float len = sqrtf(lensq);
		pdf = lensq;
		rad = radiance;
		shadowRay = Ray(isect.p, Normalize(dir), Epsilon, len - Epsilon);
	}

	Float Point::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		return 0;
	}

	string Point::ToString() const {
		string ret;
		ret += "Point[\n  radiance = " + radiance.ToString()
			+ ",\n  position = " + position.ToString()
			+ "\n]";

		return ret;
	}

	Point* CreatePointLight(const Vector3f& radiance, const Vector3f& position) {
		return new Point(radiance, position);
	}
}