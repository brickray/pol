#pragma once

#include "../core/shape.h"

namespace pol {
	class Quad : public Shape {
	private:
		//object to world transform
		Transform world;
		Vector3f normal;

	public:
		Quad(const Transform& world, Bsdf* bsdf);

		virtual Float SurfaceArea() const;
		virtual BBox WorldBBox() const;
		virtual bool Intersect(Ray& ray, Intersection& isect) const;
		virtual bool Occluded(const Ray& ray) const;
		virtual void SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const;
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const;

		//return a human-readable string summary
		virtual string ToString() const;
	};

	Quad* CreateQuadShape(Bsdf* bsdf, const Vector3f& t, const Vector3f& r, const Vector3f& s);
}