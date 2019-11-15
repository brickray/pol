#pragma once

#include "../core/integrator.h"

namespace pol {
	class Ao : public Integrator {
	private:
		Float maxDist;

	public:
		Ao(const PropSets& props, Scene& scene);

		virtual Vector3f Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const;

		virtual string ToString() const;
	};
}