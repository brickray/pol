#pragma once

#include "../core/integrator.h"

namespace pol {
	class Path : public Integrator {
	private:
		int maxDepth;
		int rrDepth;

	public:
		Path(int maxDepth, int rrDepth);

		virtual Vector3f Li(const Ray& ray, const Scene& scene, const Sampler* sampler) const;

		virtual string ToString() const;
	};

	Path* CreatePathIntegrator(int maxDepth = -1, int rrDepth = 5);
}