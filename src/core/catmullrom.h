#pragma once

#include "../pol.h"

namespace pol {
	Float CatmullRom(int size, const Float* nodes, const Float* value, Float x);
	bool CatmullRomWeights(int size, const Float* nodes, Float x, int* offset, Float* weights);
	Float SampleCatmullRom(int n, const Float* x, const Float* f, const Float* F,
		Float u, Float* fval, Float* pdf);
	Float SampleCatmullRom2D(int size1, int size2, const Float* nodes1,
		const Float* nodes2, const Float* values,
		const Float* cdf, Float alpha, Float u, Float* fval,
		Float* pdf);
}