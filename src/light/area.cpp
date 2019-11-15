#include "area.h"

namespace pol {
	POL_REGISTER_CLASS(Area, "area");

	Area::Area(const PropSets& props, Scene& scene)
		:Light(props, scene) {
		radiance = props.GetVector3f("radiance", Vector3f::Zero());
		twoside = props.GetBool("twoside", false);
	}

	void Area::SetShape(Shape* s) {
		shape = s;
	}

	bool Area::IsDelta() const {
		return false;
	}

	bool Area::IsInfinite() const {
		return false;
	}

	//power = ¡Ò¡ÒLe*cos(t)*dW*dA
	//since Le is constant,
	//power = Le¡Ò¡Òcos(t)*dW*dA
	//      = Le¡Ò¡Ò¡Òcos(t)sin(t)dt*dp*dA
	//      = Le*2*PI*A*¡Òcos(t)sin(t)dt
	//      = Le*PI*A
	//A is surface area of shape
	Float Area::Luminance() const {
		Vector3f power = radiance * shape->SurfaceArea() * Float(PI);
		if (twoside) power *= Float(2);

		return GetLuminance(power);
	}

	void Area::SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {
		Vector3f pos = isect.p, nor;
		bool solidAngle;
		shape->SampleShape(u, pos, nor, pdf, solidAngle);
		Vector3f dir = pos - isect.p;
		//twoside?
		if (!twoside && Dot(dir, nor) > 0) {
			pdf = 0;
			return;
		}

		Float lensq = dir.LengthSquare();
		Float len = sqrtf(lensq);
		dir = Normalize(dir);
		rad = radiance;
		if(!solidAngle) pdf *= (lensq / fabs(Dot(dir, nor)));
		shadowRay = Ray(isect.p, dir, Epsilon, len - Epsilon);
	}

	Float Area::Pdf(const Intersection& isect, const Vector3f& pOnSurface) const {
		bool solidAngle;
		Float pdf = shape->Pdf(isect.p, pOnSurface, solidAngle);
		if (!solidAngle) {
			Vector3f dir = pOnSurface - isect.p;
			Float lensq = dir.LengthSquare();
			Float costheta = fabs(Dot(Normalize(dir), isect.n));
			pdf *= lensq / costheta;
		}

		return pdf;
	}

	Vector3f Area::Le(const Vector3f& in, const Vector3f& nor) const {
		//twoside?
		if (!twoside && Dot(in, nor) < 0) return Vector3f::Zero();

		return radiance;
	}

	string Area::ToString() const {
		string ret;
		ret += "Area[\n  radiance = " + radiance.ToString()
			+ ",\n  shape = " + indent(shape->ToString())
			+ ",\n  twoside = " + to_string(twoside)
			+ "\n]";

		return ret;
	}
}