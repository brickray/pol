#include "random.h"

namespace pol {
	Random::Random(int sampleCount)
		:Sampler(sampleCount) {

	}

	void Random::Prepare(uint64_t idx) {
		rng.Seed(idx);
	}

	Float Random::Next1D() const {
		return rng.UniformFloat();
	}

	Vector2f Random::Next2D() const {
		return Vector2f(rng.UniformFloat(), rng.UniformFloat());
	}

	//return a human-readable string summary
	string Random::ToString() const {
		string ret;
		ret = "Random[\n  SampleCount = " + to_string(sampleCount) + "\n]";
		return ret;
	}

	Random* CreateRandomSampler(int sampleCount) {
		return new Random(sampleCount);
	}
}