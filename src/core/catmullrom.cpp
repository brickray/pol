#include "catmullrom.h"

namespace pol {
	int FindInterval(int size, const Float* nodes, Float x) {
		for (int i = 0; i < size - 1; ++i) {
			if (x > nodes[i] && x < nodes[i + 1]) return i;
		}

		return -1;
	}

	Float CatmullRom(int size, const Float* nodes, const Float* values, Float x) {
		if (x < nodes[0] || x > nodes[size - 1]) return 0;

		int idx = FindInterval(size, nodes, x);
		Float x0 = nodes[idx], x1 = nodes[idx + 1];
		Float f0 = values[idx], f1 = values[idx + 1];
		Float width = x1 - x0;
		Float d0, d1;
		if (idx > 0) {
			d0 = (f1 - values[idx - 1]) / (x1 - nodes[idx - 1]);
		}
		else {
			d0 = (f1 - f0) / width;
		}

		if (idx < size - 2) {
			d1 = (values[idx + 1] - f0) / (nodes[idx + 1] - x0);
		}
		else {
			d1 = (f1 - f0) / width;
		}

		Float t = (x - x0) / (x1 - x0), t2 = t * t, t3 = t2 * t;
		return (2 * t3 - 3 * t2 + 1) * f0 + (-2 * t3 + 3 * t2) * f1 + 
			(t3 - 2 * t2 + t) * d0 + (t3 - t2) * d1;
	}

	bool CatmullRomWeights(int size, const Float* nodes, Float x, int* offset, Float* weights) {
		if (!(x >= nodes[0] && x <= nodes[size - 1])) return false;

		int idx = FindInterval(size, nodes, x);
		*offset = idx - 1;

		Float x0 = nodes[idx], x1 = nodes[idx + 1];
		Float t = (x - x0) / (x1 - x0), t2 = t * t, t3 = t2 * t;
		weights[0] = 2 * t3 - 3 * t2 + 1;
		weights[1] = -2 * t3 + 3 * t2;

		if (idx > 0) {
			weights[2] = t3 - 2 * t2 + t;
		}
		else {
			weights[2] = 0;
		}

		if (idx < size - 2) {
			weights[3] = t3 - t2;
		}
		else {
			weights[3] = 0;
		}

		return true;
	}

	Float SampleCatmullRom(int n, const Float* x, const Float* f, const Float* F,
		Float u, Float* fval, Float* pdf) {
		Float u1 = u * F[n - 1];
		int idx = FindInterval(n, F, u1);

		Float x0 = x[idx], x1 = x[idx + 1];
		Float f0 = f[idx], f1 = f[idx + 1];
		Float width = x1 - x0;

		Float d0, d1;
		if (idx > 0) {
			d0 = (f1 - f[idx - 1]) / (x1 - x[idx - 1]);
		}
		else {
			d0 = (f1 - f0) / width;
		}

		if (idx < n - 2) {
			d1 = (f[idx + 2] - f0) / (x[idx + 2] - x0);
		}
		else {
			d1 = (f1 - f0) / width;
		}

		Float u2 = u1 - F[idx];

		Float t;
		if (f0 != f1) {
			t = (f0 - sqrt(Max(Float(0), f0 * f0 + 2 * u2 * (f1 - f0)))) / (f0 - f1);
		}
		else {
			t = u2 / f0;
		}

		Float a = 0, b = 1, Fhat, fhat;
		while (true) {
			if (!(t >= a && t <= b))
				t = 0.5 * (a + b);
			
			Fhat = t * (f0 + t * (0.5 * d0 + t * ((1.f / 3.f) * (-2 * d0 - d1) + f1 - f0 + t * (0.25 * (d0 + d1) + 0.5 * (f0 - f1)))));
			fhat = f0 + t * (d0 + t * (-2 * d0 - d1 + 3 * (f1 - f0) + t * (d0 + d1 + 2 * (f0 - f1))));
			
			if (abs(Fhat - u2) < 1e-6f || b - a < 1e-6f)
				break;

			if (Fhat - u2 < 0) a = t;
			else b = t;

			t -= Fhat / fhat;
		}

		if (fval) *fval = fhat;
		if (pdf) *pdf = fhat / F[n - 1];
		return x0 + width * t;
	}

	Float SampleCatmullRom2D(int size1, int size2, const Float* nodes1,
		const Float* nodes2, const Float* values,
		const Float* cdf, Float alpha, Float u, Float* fval,
		Float* pdf) {
		return 0;
	}
}