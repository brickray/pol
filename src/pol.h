#pragma once
#define BRICK_USING_SSE
#include <transform.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace brick;
using namespace std;

#define POL_SAFE_DELETE(ptr) if(ptr) delete ptr; 
#if _DEBUG
#define POL_ASSERT(cond) if(!(cond)) abort();
#else
#define POL_ASSERT(cons)  
#endif

__forceinline float Log2(float x) {
	static float invLog2 = 1.f / logf(2.f);
	return logf(x) * invLog2;
}

__forceinline int Log2Int(float v) {
	return (int)floorf(Log2(v));
}

__forceinline Float Pow5(Float scalar) {
	return scalar * scalar * scalar * scalar * scalar;
}

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
	return c.X() == 0 && c.Y() == 0 && c.Z() == 0;
}

__forceinline Vector3f Reflect(const Vector3f& in, const Vector3f& nor) {
	//assume in and nor are normalized
	return 2 * Dot(in, nor) * nor - in;
}

__forceinline Float BalanceHeuristic(Float f, Float g) {
	return f / (f + g);
}

__forceinline Float PowerHeuristic(Float f, Float g) {
	f *= f;
	g *= g;
	return f / (f + g);
}


