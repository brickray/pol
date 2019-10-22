#include "distribution.h"

namespace pol {
	Distribution1D::Distribution1D() {

	}

	Distribution1D::Distribution1D(const Float f[], int n) {
		cdf.resize(n + 1);

		Float invN = 1.0 / n;
		cdf[0] = 0;
		for (int i = 1; i < n + 1; ++i) {
			cdf[i] = cdf[i - 1] + f[i - 1] * invN;
		}

		funcInt = cdf[n];
		if (funcInt == 0) {
			for (int i = 1; i < n + 1; ++i)
				cdf[i] = Float(i) / Float(n);
		}
		else {
			//normalize
			for (int i = 1; i < n + 1; ++i)
				cdf[i] /= funcInt;
		}
	}

	int Distribution1D::Count() const {
		return cdf.size() - 1;
	}

	Float Distribution1D::SampleContinuous(Float u, Float& pdf) const {
		int idx = findInterval(u);

		Float du = u - cdf[idx];
		if (cdf[idx + 1] - cdf[idx] > 0) {
			du /= (cdf[idx + 1] - cdf[idx]);
		}

		//continuous case should not be multiplied by dx
		pdf = (cdf[idx + 1] - cdf[idx]) * Count();

		return (idx + du) / Count();
	}

	int Distribution1D::SampleDiscrete(Float u) const {
		int idx = findInterval(u);

		return idx;
	}

	Float Distribution1D::DiscretePdf(int idx) const {
		return cdf[idx + 1] - cdf[idx];
	}

	int Distribution1D::findInterval(Float u) const {
		//binary search
		int s = 0;
		int e = Count();
		while (true) {
			int mid = (s + e) / 2;
			
			if (cdf[mid] <= u && cdf[mid + 1] >= u)
				return mid;

			if (cdf[mid] < u) s = mid;
			else if (cdf[mid] > u) e = mid;
		}
	}


	Distribution2D::Distribution2D(const Float f[], int nu, int nv) {

	}
}