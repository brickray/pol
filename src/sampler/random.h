#pragma once

#include "../core/sampler.h"

namespace pol {
	class Random : public Sampler {
	private:
		Rng rng;

	public:
		Random(const PropSets& props, Scene& scene);

		virtual void Prepare(uint64_t idx);
		virtual Float Next1D() const;
		virtual Vector2f Next2D() const;

		virtual string ToString() const;
	};
}
