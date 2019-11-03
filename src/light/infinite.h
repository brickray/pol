#pragma once

#include "../core/light.h"
#include "../core/mipmap.h"
#include "../core/distribution.h"

namespace pol {
	class Infinite : public Light {
	private:
		Mipmap image;
		Distribution2D distribution;

		//local to world transform
		Transform world;

		//bounding sphere of scene
		Vector3f center;
		Float radius;

	public:
		Infinite(const Transform& world, int w, int h, const vector<Vector3f>& data);

		virtual void Prepare(const Scene& scene);
		virtual bool IsDelta() const;
		virtual bool IsInfinite() const;
		virtual Float Luminance() const;
		virtual void SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const;

		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const;
		virtual Vector3f Le(const Vector3f& in, const Vector3f& nor) const;

		virtual string ToString() const;
	};

	Infinite* CreateInfiniteLight(const Transform& world, int w, int h, const vector<Vector3f>& data);
	Infinite* CreateInfiniteLight(const Transform& world, const char* file);
}