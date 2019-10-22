#pragma once

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
//http://www.pcg-random.org/

#include "../pol.h"

namespace pol {
	class Rng {
	public:
		mutable uint64_t state, inc;

	public:
		Rng()
			:state(0x853c49e6748fea9bULL)
			,inc(0xda3e39cb94b95bdbULL) {
		}

		Rng(uint64_t initseq) {
			Seed(initseq);
		}

		__forceinline void Seed(uint64_t initseq) {
			state = 0u;
			inc = (initseq << 1u) | 1u;
			UniformUInt();
			state += 0x853c49e6748fea9bULL;
			UniformUInt();
		}

		__forceinline uint32_t UniformUInt() const {
			uint64_t oldstate = state;
			// Advance internal state
			state = oldstate * 6364136223846793005ULL + (inc | 1);
			// Calculate output function (XSH RR), uses old state for max ILP
			uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
			uint32_t rot = oldstate >> 59u;
			return (xorshifted >> rot) | (xorshifted << (uint32_t(-int32_t(rot)) & 31));
		}

		__forceinline Float UniformFloat() const {
			return Min(Float(0.99999994),
				Float(UniformUInt() * 2.3283064365386963e-10f));
		}
	};
}