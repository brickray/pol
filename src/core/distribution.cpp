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

	Float Distribution1D::FuncInt() const {
		return funcInt;
	}

	Float Distribution1D::SampleContinuous(Float u, Float& pdf, int& ret) const {
		int idx = findInterval(u);
		ret = idx;

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

	Distribution2D::Distribution2D() {

	}

	Distribution2D::Distribution2D(const Float f[], int nu, int nv) {
		conditional.resize(nv);
		for (int i = 0; i < nv; ++i) {
			conditional[i] = Distribution1D(&f[i * nu], nu);
		}

		vector<Float> marginalFunc;
		marginalFunc.resize(nv);
		for (int i = 0; i < nv; ++i) {
			marginalFunc[i] = conditional[i].FuncInt();
		}
		marginal = Distribution1D(&marginalFunc[0], nv);
	}

	Vector2f Distribution2D::SampleContinuous(const Vector2f& u, Float& pdf) const {
		Float pdf1, pdf2;
		int v;
		Float p2 = marginal.SampleContinuous(u.y, pdf2, v);
		Float p1 = conditional[v].SampleContinuous(u.x, pdf1, v);

		pdf = pdf1 * pdf2;
		return Vector2f(p1, p2);
	}

	Float Distribution2D::Pdf(const Vector2f& uv) const {
		Float count = conditional[0].Count();
		int iu = Clamp(uv.x * count, Float(0), Float(count - 1));
		int iv = Clamp(uv.y * marginal.Count(), Float(0), Float(marginal.Count() - 1));
		
		Float pdf2 = marginal.DiscretePdf(iv) * marginal.Count();
		Float pdf1 = conditional[iv].DiscretePdf(iu) * count;
		return pdf1 * pdf2 ;
	}
}