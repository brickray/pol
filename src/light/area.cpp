#include "area.h"

namespace pol {
	Area::Area(const Vector3f& radiance, Shape* shape, bool twoside)
		:radiance(radiance), shape(shape), twoside(twoside) {

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

	Float Area::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		return shape->Pdf(pOnLight, pOnSurface);
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

	Area* CreateAreaLight(const Vector3f& radiance, Shape* shape, bool twoside) {
		return new Area(radiance, shape, twoside);
	}
}