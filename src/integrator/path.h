#pragma once

#include "../core/integrator.h"

namespace pol {
	class Path : public Integrator {
	private:
		int maxDepth;
		int rrDepth;

	public:
		Path(const PropSets& props, Scene& scene);

		virtual Vector3f Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const;

		virtual string ToString() const;
	};
}