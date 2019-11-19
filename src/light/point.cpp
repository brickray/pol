#include "point.h"
#include "../core/warp.h"

namespace pol {
	POL_REGISTER_CLASS(Point, "point");

	Point::Point(const PropSets& props, Scene& scene)
		:Light(props, scene) {
		radiance = props.GetVector3f("radiance", Vector3f::Zero());
		position = props.GetVector3f("position", Vector3f::Zero());
	}

	bool Point::IsDelta() const {
		return true;
	}

	bool Point::IsInfinite() const {
		return false;
	}

	//power = ¡Ò¡ÒLe*cos(t)*dW*dA
	//since Le is constant, and delta position
	//power = Le¡ÒdW
	//      = 4*PI*Le
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

    void Point::SampleLight(const Vector2f& posSample, const Vector2f& dirSample, Vector3f& rad, Ray& emitRay, Float& pdfW, Float& pdfA) const {
		Vector3f dir = Warp::UniformSphere(dirSample);

		rad = radiance;
		emitRay = Ray(position, dir);
		pdfA = 1;
		pdfW = Warp::UniformSpherePdf(dir);
	}

	Float Point::Pdf(const Intersection& isect, const Vector3f& pOnSurface) const {
		return 0;
	}

	string Point::ToString() const {
		string ret;
		ret += "Point[\n  radiance = " + radiance.ToString()
			+ ",\n  position = " + position.ToString()
			+ "\n]";

		return ret;
	}
}