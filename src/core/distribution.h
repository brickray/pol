#pragma once

#include "../pol.h"

namespace pol {
	class Distribution1D {
	private:
		vector<Float> cdf;
		//integration of function
		Float funcInt;

	public:
		Distribution1D();
		Distribution1D(const Float f[], int n);

		int Count() const;

		Float SampleContinuous(Float u, Float& pdf) const;
		int SampleDiscrete(Float u) const;
		Float DiscretePdf(int idx) const;

	private:
		int findInterval(Float u) const;
	};

	class Distribution2D {
	private:
		vector<Distribution1D> conditional;

	public:
		Distribution2D(const Float f[], int nu, int nv);
	};
}