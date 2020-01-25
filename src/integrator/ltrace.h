#pragma once

#include "../core/integrator.h"

namespace pol {
	class LTrace : public Integrator {
	private:
		int maxDepth;
		int rrDepth;

	public:
		LTrace(const PropSets& props, Scene& scene);

		virtual Vector3f Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const;
		virtual void Render(const Scene& scene) const;
		virtual bool IsBidirectional() const { return true; }

		virtual string ToString() const;
	};
}