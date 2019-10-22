#include "sampler.h"

namespace pol {
	Sampler::Sampler(int sampleCount)
	:sampleCount(sampleCount){

	}

	Sampler::~Sampler() {

	}

	int Sampler::GetSampleCount() const {
		return sampleCount;
	}
}