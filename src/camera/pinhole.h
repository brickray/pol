#pragma once

#include "../core/camera.h"

namespace pol {
	class Pinhole : public ProjectionCamera {
	public:
		Pinhole(const PropSets& props, Scene& scene);

		//pinhole camera doesn't consider dof
		virtual Ray GenerateRay(const Vector2f& cameraSample, const Vector2f& dofSample) const;
		//generate ray differential, mainly for texture anti-aliasing
		virtual RayDifferential GenerateRayDifferential(const Vector2f& cameraSample, const Vector2f& dofSample) const;

		//return a human-readable string summary
		virtual string ToString() const;
	};
}
