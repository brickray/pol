#pragma once

#include "../core/integrator.h"

namespace pol {
	class Ao : public Integrator {
	private:
		Float maxDist;

	public:
		Ao(Float maxDist);

		virtual Vector3f Li(const Ray& ray, const Scene& scene, const Sampler* sampler) const;

		virtual string ToString() const;
	};

	Ao* CreateAoIntegrator(Float maxDist = 2.0);
}