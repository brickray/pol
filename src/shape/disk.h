#pragma once

#include "../core/shape.h"

namespace pol {
	class Disk : public Shape {
	private:
		//object to world transform
		Transform world;
		Float radius;
		Vector3f normal;

	public:
		Disk(const PropSets& props, Scene& scene);

		virtual Float SurfaceArea() const;
		virtual BBox WorldBBox() const;
		virtual bool Intersect(Ray& ray, Intersection& isect) const;
		virtual bool Occluded(const Ray& ray) const;
		virtual void SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const;
		virtual void SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdfA) const;
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const;

		//return a human-readable string summary
		virtual string ToString() const;
	};
}