#pragma once

#include "object.h"
#include "intersection.h"
#include "bsdf.h"
#include "bssrdf.h"

namespace pol {
	class Light;
	class Scene;
	class Shape : public PolObject {
	protected:
		//material of shape
		Bsdf* bsdf;
		Bssrdf* bssrdf;
		//light pointer
		Light* light;

	public:
		Shape() {}
		Shape(const PropSets& props, Scene& scene);
		virtual ~Shape();

		virtual Float SurfaceArea() const = 0;
		virtual BBox WorldBBox() const = 0;
		//return true if ray intersect with shape
		virtual bool Intersect(Ray& ray, Intersection& isect) const = 0;
		//shadow ray test
		virtual bool Occluded(const Ray& ray) const = 0;
		//sample shape
		//soldAngle : pdf in which type (area or solidAngle)
		virtual void SampleShape(const Vector2f& u, /*in out*/Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const = 0;
		virtual void SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdfA) const = 0;
		//soldAngle : pdf in which type (area or solidAngle)
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const = 0;

		virtual void SetLight(Light* l) { light = l; }
		const Bsdf* GetBsdf() const { return bsdf;  }
	};

	//helper function
	__forceinline Transform GetWorldTransform(const PropSets& props) {
		Transform world;
		if (props.HasValue("world")) {
			world = props.GetTransform("world");
		}
		else {
			Vector3f t = props.GetVector3f("translate", Vector3f::Zero());
			Vector3f s = props.GetVector3f("scale", Vector3f::One());
			if (props.HasValue("axis")) {
				Vector3f axis = props.GetVector3f("axis");
				Float angle = props.GetFloat("angle");

				world = TRS(t, axis, angle, s);
			}
			else {
				Vector3f r = props.GetVector3f("rotate", Vector3f::Zero());
				world = TRS(t, r, s);
			}
		}

		return world;
	}
}