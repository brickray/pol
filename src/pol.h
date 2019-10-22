#pragma once

#include <transform.h>
#include <string>
#include <vector>
#include <memory>

using namespace brick;
using namespace std;

#define POL_SAFE_DELETE(ptr) if(ptr) delete ptr; 
#if _DEBUG
#define POL_ASSERT(cond) if(!(cond)) abort();
#else
#define POL_ASSERT(cons)  
#endif

__forceinline Float GetLuminance(const Vector3f & c) {
	static Vector3f luma(0.212671f, 0.715160f, 0.072169f);

	return Dot(luma, c);
}

__forceinline bool SameHemiSphere(const Vector3f& in, const Vector3f& out, const Vector3f& nor) {
	return Dot(in, nor) * Dot(out, nor) > 0;
}

__forceinline bool SameHemiSphere(const Vector3f& in, const Vector3f& out) {
	return Dot(in, out) > 0;
}

__forceinline bool IsBlack(const Vector3f& c) {
	return c.x == 0 && c.y == 0 && c.z == 0;
}

__forceinline Vector3f Reflect(const Vector3f& in, const Vector3f& nor) {
	//assume in and nor are normalized
	return 2 * Dot(in, nor) * nor - in;
}

__forceinline Float BalanceHeuristic(Float nf, Float fPdf, Float ng, Float gPdf) {
	Float f = nf * fPdf;
	Float g = ng * gPdf;
	return f / (f + g);
}

__forceinline Float PowerHeuristic(Float nf, Float fPdf, Float ng, Float gPdf) {
	Float f = nf * fPdf;
	Float g = ng * gPdf;
	return f * f / (f * f + g * g);
}


