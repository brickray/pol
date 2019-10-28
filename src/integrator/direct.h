#pragma once

#include "../core/integrator.h"

namespace pol {
	class Direct : public Integrator {
	public:
		virtual Vector3f Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const;

		virtual string ToString() const;
	};

	Direct* CreateDirectIntegrator();
}