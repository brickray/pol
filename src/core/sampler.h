#pragma once

#include "object.h"
#include "rng.h"

namespace pol {
	class Sampler : public PolObject {
	protected:
		int sampleCount;

	public:
		Sampler(int sampleCount);
		virtual ~Sampler();

		virtual void Prepare(uint64_t idx) = 0;
		virtual Float Next1D() const = 0;
		virtual Vector2f Next2D() const = 0;

		int GetSampleCount() const;
	};
}