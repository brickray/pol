#include "random.h"

namespace pol {
	POL_REGISTER_CLASS(Random, "random");

	Random::Random(const PropSets& props, Scene& scene)
		:Sampler(props, scene) {

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
}