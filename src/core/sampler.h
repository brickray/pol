#pragma once

#include "object.h"
#include "rng.h"

namespace pol {
	class Sampler : public PolObject {
	protected:
		int sampleCount;

	public:
		Sampler(const PropSets& props, Scene& scene);
		virtual ~Sampler();

		virtual void Prepare(uint64_t idx) = 0;
		virtual Float Next1D() const = 0;
		virtual Vector2f Next2D() const = 0;
		virtual Sampler* Clone() const = 0;

		int GetSampleCount() const;
	};

	Float RadicalInverse(unsigned int idx, unsigned int n);
}