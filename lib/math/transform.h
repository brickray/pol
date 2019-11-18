#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <iostream>  
#include <sstream>
#include <xmmintrin.h> //SSE

#pragma warning(disable : 4305)  // double constant assigned to float
#pragma warning(disable : 4244)  // int -> float conversion
#pragma warning(disable : 4843)  // double -> float conversion
#pragma warning(disable : 26454) // arithmetic operation causes negative unsigned result
#pragma warning(disable : 26451) // cast before arithmetic operation

//BRICK MATH LIB

namespace brick{
#ifndef BRICK_USING_DOUBLE_PRECISE
	typedef float Float;
	const Float Epsilon = 1e-4;
#else
	typedef double Float;
	const Float Epsilon = 1e-10;
#endif
	
#define INV4PI    0.07957747155
#define INV2PI    0.15915494309
#define INVPI     0.31830988618
#define PIOVER4   0.78539816340
#define PIOVER2   1.57079632679
#define PI        3.14159265358
#define TWOPI     6.28318530716
#define FOURPI    12.5663706143

	__forceinline Float Radians(Float degree){
		return degree*0.017453293/*PI / 180.0*/;
	}

	__forceinline Float Degree(Float radians){
		return radians*57.2957795/*180.0 / PI*/;
	}

	template <class T>
	__forceinline T Min(T a, T b){
		return a < b ? a : b;
	}

	template <class T>
	__forceinline T Max(T a, T b){
		return a > b ? a : b;
	}

	template <class T>
	__forceinline T Clamp(T val, T min, T max){
		if (min > val) return min;
		if (max < val) return max;

		return val;
	}

	__forceinline int RoundUpPow2(int v){
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		return v + 1;
	}

	template <class T>
	__forceinline T Lerp(const T& a, const T& b, Float t) {
		return (1 - t) * a + t * b;
	}

	__forceinline std::string indent(const std::string& str, int amount = 2) {
		std::istringstream iss(str);
		std::ostringstream oss;
		std::string spacer(amount, ' ');
		bool firstLine = true;
		for (std::string line; std::getline(iss, line); ) {
			if (!firstLine)
				oss << spacer;
			oss << line;
			if (!iss.eof())
				oss << std::endl;
			firstLine = false;
		}
		return oss.str();
	}
}

namespace brick{
#if (defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE))
	//-------------------------------------------------------------
	//                       Vector4
	//-------------------------------------------------------------
	template <class T>
	class Vector4{
	public:
		T x, y, z, w;

	public:
		Vector4(){
			x = y = z = w = 0;
		}

		Vector4(T cc){
			x = cc;
			y = cc;
			z = cc;
			w = cc;
		}

		Vector4(T xx, T yy, T zz, T ww){
			x = xx;
			y = yy;
			z = zz;
			w = ww;
		}

		template<class U>
		Vector4(const Vector4<U>& a){
			x = a.x;
			y = a.y;
			z = a.z;
			w = a.w;
		}

		static Vector4<T> Right() { return Vector4<T>(1, 0, 0, 0); }
		static Vector4<T> Up() { return Vector4<T>(0, 1, 0, 0); }
		static Vector4<T> Forward() { return Vector4<T>(0, 0, 1, 0); }
		static Vector4<T> Zero() { return Vector4<T>(0, 0, 0, 0); }
		static Vector4<T> One() { return Vector4<T>(1, 1, 1, 1); }

		__forceinline Float X() const { return x; }
		__forceinline Float Y() const { return y; }
		__forceinline Float Z() const { return z; }
		__forceinline Float W() const { return w; }

		__forceinline Float LengthSquare() const{
			return x*x + y*y + z*z + w*w;
		}

		__forceinline Float Length() const{
			return sqrt(LengthSquare());
		}

		__forceinline bool IsNormalized() const {
			return LengthSquare() - 1 < Epsilon;
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y, 2 = z, 3 = z
		__forceinline int MajorComponent() const {
			if (x > y&& x > z&& x > w) return 0;
			else if (y > z&& y > w) return 1;
			else if (z > w) return 2;
			else return 3;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y, 2 = z, 3 = z
		__forceinline int MinorComponent() const {
			Float n[4] = { x,y,z,w };
			std::sort(n, n + 4);
			if (n[1] == x) return 0;
			else if (n[1] == y) return 1;
			else if (n[2] == z) return 2;
			else return 3;
		}

		__forceinline T operator[](int idx) const{
			return (&x)[idx];
		}

		__forceinline T& operator[](int idx){
			return (&x)[idx];
		}

		template <class U>
		__forceinline bool operator==(const Vector4<U>& n) const{
			return fabs(x - n.x) < Epsilon && fabs(y - n.y) < Epsilon && fabs(z - n.z) < Epsilon && fabs(w - n.w) < Epsilon;
		}

		template <class U>
		__forceinline bool operator!=(const Vector4<U>& n) const{
			return !(*this == n);
		}

		template <class U>
		__forceinline Vector4<T>& operator=(const Vector4<U>& n){
			x = n.x;
			y = n.y;
			z = n.z;
			w = n.w;
			return *this;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			for (int i = 0; i < 4; ++i) {
				ret += std::to_string((*this)[i]);
				if (i < 3) ret += ", ";
			}

			return "[" + ret + "]";
		}
	};

	template <class T>
	__forceinline Vector4<T> operator-(const Vector4<T>& a){
		return Vector4<T>(-a.x, -a.y, -a.z, -a.w);
	}

	template <class T>
	__forceinline Vector4<T> operator+(const Vector4<T>& a, const Vector4<T>& b){
		return Vector4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	template <class T>
	__forceinline Vector4<T> operator+(const Vector4<T>& a, T scalar) {
		return Vector4<T>(a.x + scalar, a.y + scalar, a.z + scalar, a.w + scalar);
	}

	template <class T>
	__forceinline Vector4<T> operator-(const Vector4<T>& a, const Vector4<T>& b){
		return Vector4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	template <class T>
	__forceinline Vector4<T> operator-(const Vector4<T>& a, T scalar) {
		return Vector4<T>(a.x - scalar, a.y - scalar, a.z - scalar, a.w - scalar);
	}

	template <class T>
	__forceinline Vector4<T> operator*(const Vector4<T>& a, T scalar){
		return Vector4<T>(a.x*scalar, a.y*scalar, a.z*scalar, a.w*scalar);
	}

	template <class T>
	__forceinline Vector4<T> operator*(T scalar, const Vector4<T>& a){
		return Vector4<T>(a.x*scalar, a.y*scalar, a.z*scalar, a.w*scalar);
	}

	template <class T, class U>
	__forceinline Vector4<T> operator*(const Vector4<T>& a, const Vector4<U>& b) {
		return Vector4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	template <class T>
	__forceinline Vector4<T> operator/(const Vector4<T>& a, T scalar){
		return Vector4<T>(a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar);
	}

	template <class T>
	__forceinline Vector4<T> operator/(T scalar, const Vector4<T>& a){
		return Vector4<T>(scalar / a.x, scalar / a.y, scalar / a.z, scalar / a.w);
	}

	template <class T, class U>
	__forceinline Vector4<T> operator/(const Vector4<T>& a, const Vector4<U>& b){
		return Vector4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
	}

	template <class T>
	__forceinline Vector4<T>& operator+=(Vector4<T>& a, const Vector4<T>& b){
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		a.w += b.w;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator+=(Vector4<T>& a, Float scalar) {
		a.x += scalar;
		a.y += scalar;
		a.z += scalar;
		a.w += scalar;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator-=(Vector4<T>& a, const Vector4<T>& b){
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		a.w -= b.w;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator-=(Vector4<T>& a, Float scalar) {
		a.x -= scalar;
		a.y -= scalar;
		a.z -= scalar;
		a.w -= scalar;;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator*=(Vector4<T>& a, T scalar){
		a.x *= scalar;
		a.y *= scalar;
		a.z *= scalar;
		a.w *= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator*=(Vector4<T>& a, const Vector4<T>& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		a.w *= b.w;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator/=(Vector4<T>& a, T scalar){
		a.x /= scalar;
		a.y /= scalar;
		a.z /= scalar;
		a.w /= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector4<T>& operator/=(Vector4<T>& a, const Vector4<T>& b) {
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		a.w /= b.w;
		return a;
	}

	template <class T>
	__forceinline Vector4<T> Normalize(const Vector4<T>& a){
		return a / a.Length();
	}

	template <class T>
	__forceinline T Dot(const Vector4<T>& a, const Vector4<T>& b){
		return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
	}

	typedef Vector4<int> Vector4i;
	typedef Vector4<Float> Vector4f;

#else
	//-------------------------------------------------------------
	//                       Vector4 SSE
	//-------------------------------------------------------------
#define SHUFFLE4(V, X, Y, Z, W) Vector3f(_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(W, Z, Y, X)))
    class Vector4f {
	public:
		__m128 m;

	public:
		__forceinline Vector4f() { m = _mm_set1_ps(0); }
		__forceinline Vector4f(Float scalar) { m = _mm_set1_ps(scalar); }
		__forceinline Vector4f(Float x, Float y, Float z, Float w) { m = _mm_set_ps(w, z, y, x); }
		__forceinline Vector4f(__m128 v) { m = v; }

		static Vector4f Right() { return Vector4f(1, 0, 0, 0); }
		static Vector4f Up() { return Vector4f(0, 1, 0, 0); }
		static Vector4f Forward() { return Vector4f(0, 0, 1, 0); }
		static Vector4f Zero() { return Vector4f(0, 0, 0, 0); }
		static Vector4f One() { return Vector4f(1, 1, 1, 1); }

		// the __m128 type doesn't provide any decent direct access to the X/Y/Z components, so we'll need wrappers
		__forceinline Float X() const { return _mm_cvtss_f32(m); }
		__forceinline Float Y() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1))); }
		__forceinline Float Z() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 2, 2, 2))); }
		__forceinline Float W() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(3, 3, 3, 3))); }
		
		__forceinline Float operator[](int idx) const {
			return m.m128_f32[idx];
		}

		__forceinline Float& operator[](int idx) {
			return m.m128_f32[idx];
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y, 2 = z, 3 = z
		__forceinline int MajorComponent() const {
			Float x = X(); Float y = Y(); Float z = Z(); Float w = W();
			if (x > y&& x > z&& x > w) return 0;
			else if (y > z&& y > w) return 1;
			else if (z > w) return 2;
			else return 3;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y, 2 = z, 3 = z
		__forceinline int MinorComponent() const {
			Float x = X(); Float y = Y(); Float z = Z(); Float w = W();
			Float n[4] = { x,y,z,w };
			std::sort(n, n + 4);
			if (n[1] == x) return 0;
			else if (n[1] == y) return 1;
			else if (n[2] == z) return 2;
			else return 3;
		}

		__forceinline Float Length() const {
			return sqrtf(LengthSquare());
		}

		__forceinline Float LengthSquare() const {
			Vector4f a = _mm_mul_ps(m, m);;
			return a.X() + a.Y() + a.Z() + a.W();
		}

		__forceinline std::string ToString() const {
			std::string ret;
			for (int i = 0; i < 4; ++i) {
				ret += std::to_string((*this)[i]);
				if (i < 3) ret += ", ";
			}

			return "[" + ret + "]";
		}
	};

	__forceinline Vector4f operator+(Vector4f a, Vector4f b) {
		a.m = _mm_add_ps(a.m, b.m); 
		return a;
	}

	__forceinline Vector4f operator+(Vector4f a, Float scalar) {
		a.m = _mm_add_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector4f operator-(Vector4f a, Vector4f b) {
		a.m = _mm_sub_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector4f operator-(Vector4f a, Float scalar) {
		a.m = _mm_sub_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector4f operator*(Vector4f a, Vector4f b) {
		a.m = _mm_mul_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector4f operator*(Vector4f a, Float scalar) {
		a.m = _mm_mul_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector4f operator*(Float scalar, Vector4f a) {
		a.m = _mm_mul_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector4f operator/(Vector4f a, Vector4f b) {
		a.m = _mm_div_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector4f operator/(Vector4f a, Float scalar) {
		a.m = _mm_div_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector4f operator/(Float scalar, Vector4f a) {
		a.m = _mm_div_ps(_mm_set1_ps(scalar), a.m);
		return a;
	}

	__forceinline Vector4f& operator+=(Vector4f& a, Vector4f b) {
		a = a + b;
		return a;
	}

	__forceinline Vector4f& operator+=(Vector4f& a, Float scalar) {
		a = a + scalar;
		return a;
	}

	__forceinline Vector4f& operator-=(Vector4f& a, Vector4f b) {
		a = a - b;
		return a;
	}

	__forceinline Vector4f& operator-=(Vector4f& a, Float scalar) {
		a = a - scalar;
		return a;
	}

	__forceinline Vector4f& operator*=(Vector4f& a, Vector4f b) {
		a = a * b;
		return a;
	}

	__forceinline Vector4f& operator*=(Vector4f& a, Float scalar) {
		a = a * scalar;
		return a;
	}

	__forceinline Vector4f& operator/=(Vector4f& a, Vector4f b) {
		a = a / b;
		return a;
	}

	__forceinline Vector4f& operator/=(Vector4f& a, Float scalar) {
		a = a / scalar;
		return a;
	}

	__forceinline bool operator==(Vector4f a, Vector4f b) {
		return fabs(a.X() - b.X()) < Epsilon && fabs(a.Y() - b.Y()) < Epsilon &&
			fabs(a.Z() - b.Z()) < Epsilon && fabs(a.W() - b.W()) < Epsilon;
	}

	__forceinline bool operator!=(Vector4f a, Vector4f b) {
		return !(a == b);
	}

	__forceinline Vector4f operator-(Vector4f a) {
		return Vector4f(_mm_setzero_ps()) - a;
	}

	__forceinline Vector4f Min(Vector4f a, Vector4f b) {
		a.m = _mm_min_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector4f Max(Vector4f a, Vector4f b) {
		a.m = _mm_max_ps(a.m, b.m);
		return a;
	}

	__forceinline Float Sum(Vector4f a) {
		return a.X() + a.Y() + a.Z() + a.W();
	}

	__forceinline Vector4f Normalize(Vector4f a) {
		return a / a.Length();
	}

	__forceinline Float Dot(Vector4f a, Vector4f b) {
		return Sum(a * b);
	}

	__forceinline Vector4f Lerp(Vector4f a, Vector4f b, Vector4f t) {
		return (_mm_set1_ps(1) - t) * a + t * b;
	}

	class Vector4i {
	public:
		int x, y, z, w;

	public:
		Vector4i() { x = y = z = w = 0; }
		Vector4i(int xx, int yy, int zz, int ww) { x = xx;  y = yy; z = zz; w = ww; }
		Vector4i(const Vector4i& a) { x = a.x; y = a.y; z = a.z; w = a.w; }
		Vector4i(const Vector4f& a) { x = a.X(); y = a.Y(); z = a.Z(); w = a.W(); }

		__forceinline int operator[](int idx) const {
			return (&x)[idx];
		}

		__forceinline int& operator[](int idx) {
			return (&x)[idx];
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y, 2 = z, 3 = z
		__forceinline int MajorComponent() const {
			if (x > y&& x > z&& x > w) return 0;
			else if (y > z&& y > w) return 1;
			else if (z > w) return 2;
			else return 3;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y, 2 = z, 3 = z
		__forceinline int MinorComponent() const {
			int n[4] = { x,y,z,w };
			std::sort(n, n + 4);
			if (n[1] == x) return 0;
			else if (n[1] == y) return 1;
			else if (n[2] == z) return 2;
			else return 3;
		}

		__forceinline bool operator==(const Vector4i& a) const {
			return x == a.x && y == a.y && z == a.z && w == a.w;
		}

		__forceinline bool operator!=(const Vector4i& a) const {
			return x != a.x || y != a.y || z != a.z || w != a.w;
		}
	};

	__forceinline Vector4i operator+(const Vector4i& a, const Vector4i& b) {
		return Vector4i(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	__forceinline Vector4i operator-(const Vector4i& a, const Vector4i& b) {
		return Vector4i(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	__forceinline Vector4i operator*(const Vector4i& a, const Vector4i& b) {
		return Vector4i(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	__forceinline Vector4f operator*(const Vector4i& a, const Vector4f& b) {
		return Vector4f(a.x * b.X(), a.y * b.Y(), a.z * b.Z(), a.w * b.W());
	}

	__forceinline Vector4i operator/(const Vector4i& a, const Vector4i& b) {
		return Vector4i(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
	}

	__forceinline Vector4f operator/(const Vector4i& a, const Vector4f& b) {
		return Vector4f(a.x / b.X(), a.y / b.Y(), a.z / b.Z(), a.w / b.W());
	}

#endif

#if (defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE))
	//-------------------------------------------------------------
	//                       Vector3
	//-------------------------------------------------------------
	template <class T>
	class Vector3{
	public:
		T x, y, z;

	public:
		Vector3(){
			x = y = z = 0;
		}

		Vector3(T cc){
			x = cc;
			y = cc;
			z = cc;
		}

		Vector3(T xx, T yy, T zz){
			x = xx;
			y = yy;
			z = zz;
		}

		template<class U>
		Vector3(const Vector3<U>& a){
			x = a.x;
			y = a.y;
			z = a.z;
		}

		template <class U>
		Vector3(const Vector4<U>& a){
			x = a.x;
			y = a.y;
			z = a.z;
		}

		static Vector3<T> Right() { return Vector3<T>(1, 0, 0); }
		static Vector3<T> Up() { return Vector3<T>(0, 1, 0); }
		static Vector3<T> Forward() { return Vector3<T>(0, 0, 1); }
		static Vector3<T> Zero() { return Vector3<T>(0, 0, 0); }
		static Vector3<T> One() { return Vector3<T>(1, 1, 1); }

		__forceinline Float X() const { return x; }
		__forceinline Float Y() const { return y; }
		__forceinline Float Z() const { return z; }

		__forceinline Float LengthSquare() const{
			return x*x + y*y + z*z;
		}

		__forceinline Float Length() const{
			return sqrt(LengthSquare());
		}

		__forceinline bool IsNormalized() const {
			return LengthSquare() - 1 < Epsilon;
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y, 2 = z
		__forceinline int MajorComponent() const {
			if (x > y&& x > z) return 0;
			else if (y > z) return 1;
			else return 2;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y, 2 = z
		__forceinline int MinorComponent() const {
			if (x > y&& x > z) {
				if (y > z) return 1;
				else return 2;
			}
			else if (y > z) {
				if (x > z) return 0;
				else return 2;
			}
			else {
				if (x > y) return 0;
				else return 1;
			}
		}

		__forceinline T operator[](int idx) const{
			return (&x)[idx];
		}

		__forceinline T& operator[](int idx){
			return (&x)[idx];
		}

		template <class U>
		__forceinline bool operator==(const Vector3<U>& n) const{
			return fabs(x - n.x) < Epsilon && fabs(y - n.y) < Epsilon && fabs(z - n.z) < Epsilon;
		}

		template <class U>
		__forceinline bool operator!=(const Vector3<U>& n) const{
			return !(*this == n);
		}

		template <class U>
		__forceinline Vector3<T>& operator=(const Vector3<U>& n){
			x = n.x;
			y = n.y;
			z = n.z;
			return *this;
		}
		
		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			for (int i = 0; i < 3; ++i) {
				ret += std::to_string((*this)[i]);
				if (i < 2) ret += ", ";
			}

			return "[" + ret + "]";
		}
	};

	template <class T>
	__forceinline Vector3<T> operator-(const Vector3<T>& a){
		return Vector3<T>(-a.x, -a.y, -a.z);
	}

	template <class T>
	__forceinline Vector3<T> operator+(const Vector3<T>& a, const Vector3<T>& b){
		return Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template <class T>
	__forceinline Vector3<T> operator+(const Vector3<T>& a, T scalar) {
		return Vector3<T>(a.x + scalar, a.y + scalar, a.z + scalar);
	}

	template <class T>
	__forceinline Vector3<T> operator-(const Vector3<T>& a, const Vector3<T>& b){
		return Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	template <class T>
	__forceinline Vector3<T> operator-(const Vector3<T>& a, T scalar) {
		return Vector3<T>(a.x - scalar, a.y - scalar, a.z - scalar);
	}

	template <class T>
	__forceinline Vector3<T> operator*(const Vector3<T>& a, T scalar){
		return Vector3<T>(a.x*scalar, a.y*scalar, a.z*scalar);
	}

	template <class T>
	__forceinline Vector3<T> operator*(T scalar, const Vector3<T>& a){
		return Vector3<T>(a.x*scalar, a.y*scalar, a.z*scalar);
	}

	template <class T, class U>
	__forceinline Vector3<T> operator*(const Vector3<T>& a, const Vector3<U>& b) {
		return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	template <class T>
	__forceinline Vector3<T> operator/(const Vector3<T>& a, T scalar){
		return Vector3<T>(a.x / scalar, a.y / scalar, a.z / scalar);
	}

	template <class T>
	__forceinline Vector3<T> operator/(T scalar, const Vector3<T>& a){
		return Vector3<T>(scalar / a.x, scalar / a.y, scalar / a.z);
	}

	template <class T, class U>
	__forceinline Vector3<T> operator/(const Vector3<T>& a, const Vector3<U>& b){
		return Vector3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	template <class T>
	__forceinline Vector3<T>& operator+=(Vector3<T>& a, const Vector3<T>& b){
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator+=(Vector3<T> & a, Float scalar) {
		a.x += scalar;
		a.y += scalar;
		a.z += scalar;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator-=(Vector3<T>& a, const Vector3<T>& b){
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator-=(Vector3<T>& a, Float scalar) {
		a.x -= scalar;
		a.y -= scalar;
		a.z -= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator*=(Vector3<T>& a, T scalar){
		a.x *= scalar;
		a.y *= scalar;
		a.z *= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator*=(Vector3<T>& a, const Vector3<T>& b) {
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator/=(Vector3<T>& a, T scalar){
		a.x /= scalar;
		a.y /= scalar;
		a.z /= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector3<T>& operator/=(Vector3<T>& a, const Vector3<T>& b) {
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		return a;
	}

	template <class T>
	__forceinline Vector3<T> Min(const Vector3<T>& a, const Vector3<T>& b) {
		Vector3<T> ret;
		ret.x = brick::Min(a.x, b.x);
		ret.y = brick::Min(a.y, b.y);
		ret.z = brick::Min(a.z, b.z);
		return ret;
	}

	template <class T>
	__forceinline Vector3<T> Max(const Vector3<T>& a, const Vector3<T>& b) {
		Vector3<T> ret;
		ret.x = brick::Max(a.x, b.x);
		ret.y = brick::Max(a.y, b.y);
		ret.z = brick::Max(a.z, b.z);
		return ret;
	}

	template <class T>
	__forceinline T HMin(const Vector3<T>& a) {
		if (a.x < a.y && a.x < a.z) return a.x;
		else if (a.y < a.z) return a.y;
		else return a.z;
	}

	template <class T>
	__forceinline T HMax(const Vector3<T>& a) {
		if (a.x > a.y && a.x > a.z) return a.x;
		else if (a.y > a.z) return a.y;
		else return a.z;
	}

	template <class T>
	__forceinline Vector3<T> Normalize(const Vector3<T>& a){
		return a / a.Length();
	}

	template <class T>
	__forceinline T Dot(const Vector3<T>& a, const Vector3<T>& b){
		return a.x*b.x + a.y*b.y + a.z*b.z;
	}

	//only vector3 has cross method
	template <class T>
	__forceinline Vector3<T> Cross(const Vector3<T>& v1, const Vector3<T>& v2){
		Float v1x = v1.x, v1y = v1.y, v1z = v1.z;
		Float v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}

	typedef Vector3<int> Vector3i;
	typedef Vector3<Float> Vector3f;

#else
//-------------------------------------------------------------
//                       Vectorf3 SSE
//-------------------------------------------------------------
	//http://www.codersnotes.com/notes/maths-lib-2016/
#define SHUFFLE3(V, X, Y, Z) Vector3f(_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(Z, Z, Y, X)))
    class Vector3f {
	public:
		__m128 m;

	public:
		__forceinline Vector3f() { m = _mm_set1_ps(0); }
		__forceinline Vector3f(Float scalar) { m = _mm_set1_ps(scalar); }
		__forceinline Vector3f(Float x, Float y, Float z) { m = _mm_set_ps(z, z, y, x); }
		__forceinline Vector3f(Vector4f v) { m = _mm_set_ps(v.Z(), v.Z(), v.Y(), v.X()); }
		__forceinline Vector3f(__m128 v) { m = v; }

		static Vector3f Right() { return Vector3f(1, 0, 0); }
		static Vector3f Up() { return Vector3f(0, 1, 0); }
		static Vector3f Forward() { return Vector3f(0, 0, 1); }
		static Vector3f Zero() { return Vector3f(0, 0, 0); }
		static Vector3f One() { return Vector3f(1, 1, 1); }

		// the __m128 type doesn't provide any decent direct access to the X/Y/Z components, so we'll need wrappers
		__forceinline Float X() const { return _mm_cvtss_f32(m); }
		__forceinline Float Y() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1))); }
		__forceinline Float Z() const { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 2, 2, 2))); }
		__forceinline Vector3f YZX() const { return SHUFFLE3(*this, 1, 2, 0); }
		__forceinline Vector3f ZXY() const { return SHUFFLE3(*this, 2, 0, 1); }

		__forceinline Float operator[](int idx) const {
			return m.m128_f32[idx];
		}

		__forceinline Float& operator[](int idx) {
			return m.m128_f32[idx];
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y, 2 = z
		__forceinline int MajorComponent() const {
			Float x = X(); Float y = Y(); Float z = Z();
			if (x > y&& x > z) return 0;
			else if (y > z) return 1;
			else return 2;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y, 2 = z
		__forceinline int MinorComponent() const {
			Float x = X(); Float y = Y(); Float z = Z();
			if (x > y&& x > z) {
				if (y > z) return 1;
				else return 2;
			}
			else if (y > z) {
				if (x > z) return 0;
				else return 2;
			}
			else {
				if (x > y) return 0;
				else return 1;
			}
		}

		__forceinline Float Length() const {
			return sqrtf(LengthSquare());
		}

		__forceinline Float LengthSquare() const {
			Vector3f a = _mm_mul_ps(m, m);;
			return a.X() + a.Y() + a.Z();
		}

		__forceinline std::string ToString() const {
			std::string ret;
			for (int i = 0; i < 3; ++i) {
				ret += std::to_string((*this)[i]);
				if (i < 2) ret += ", ";
			}

			return "[" + ret + "]";
		}
	};

	__forceinline Vector3f operator+(Vector3f a, Vector3f b) {
		a.m = _mm_add_ps(a.m, b.m); 
		return a;
	}

	__forceinline Vector3f operator+(Vector3f a, Float scalar) {
		a.m = _mm_add_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector3f operator-(Vector3f a, Vector3f b) {
		a.m = _mm_sub_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector3f operator-(Vector3f a, Float scalar) {
		a.m = _mm_sub_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector3f operator*(Vector3f a, Vector3f b) {
		a.m = _mm_mul_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector3f operator*(Vector3f a, Float scalar) {
		a.m = _mm_mul_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector3f operator*(Float scalar, Vector3f a) {
		a.m = _mm_mul_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector3f operator/(Vector3f a, Vector3f b) {
		a.m = _mm_div_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector3f operator/(Vector3f a, Float scalar) {
		a.m = _mm_div_ps(a.m, _mm_set1_ps(scalar));
		return a;
	}

	__forceinline Vector3f operator/(Float scalar, Vector3f a) {
		a.m = _mm_div_ps(_mm_set1_ps(scalar), a.m);
		return a;
	}

	__forceinline Vector3f& operator+=(Vector3f& a, Vector3f b) {
		a = a + b;
		return a;
	}

	__forceinline Vector3f& operator+=(Vector3f& a, Float scalar) {
		a = a + scalar;
		return a;
	}

	__forceinline Vector3f& operator-=(Vector3f& a, Vector3f b) {
		a = a - b;
		return a;
	}

	__forceinline Vector3f& operator-=(Vector3f& a, Float scalar) {
		a = a - scalar;
		return a;
	}

	__forceinline Vector3f& operator*=(Vector3f& a, Vector3f b) {
		a = a * b;
		return a;
	}

	__forceinline Vector3f& operator*=(Vector3f& a, Float scalar) {
		a = a * scalar;
		return a;
	}

	__forceinline Vector3f& operator/=(Vector3f& a, Vector3f b) {
		a = a / b;
		return a;
	}

	__forceinline Vector3f& operator/=(Vector3f& a, Float scalar) {
		a = a / scalar;
		return a;
	}

	__forceinline bool operator==(Vector3f a, Vector3f b) {
		a.m = _mm_cmpeq_ps(a.m, b.m);
		return a.X() && a.Y() && a.Z();
	}

	__forceinline bool operator!=(Vector3f a, Vector3f b) {
		a.m = _mm_cmpneq_ps(a.m, b.m);
		return a.X() || a.Y() || a.Z();
	}

	__forceinline Vector3f operator-(Vector3f a) {
		return Vector3f(_mm_setzero_ps()) - a;
	}

	__forceinline Vector3f Min(Vector3f a, Vector3f b) {
		a.m = _mm_min_ps(a.m, b.m);
		return a;
	}

	__forceinline Vector3f Max(Vector3f a, Vector3f b) {
		a.m = _mm_max_ps(a.m, b.m);
		return a;
	}

	__forceinline Float HMin(Vector3f a) {
		a = Min(a, SHUFFLE3(a, 1, 0, 2));
		return Min(a, SHUFFLE3(a, 2, 0, 1)).X();
	}

	__forceinline Float HMax(Vector3f a) {
		a = Max(a, SHUFFLE3(a, 1, 0, 2));
		return Max(a, SHUFFLE3(a, 2, 0, 1)).X();
	}

	__forceinline Float Sum(Vector3f a) {
		return a.X() + a.Y() + a.Z();
	}

	__forceinline Vector3f Normalize(Vector3f a) {
		return a / a.Length();
	}

	__forceinline Float Dot(Vector3f a, Vector3f b) {
		return Sum(a * b);
	}

	__forceinline Vector3f Cross(Vector3f a, Vector3f b) {
		return (a.ZXY() * b - a * b.ZXY()).ZXY();
	}

	__forceinline Vector3f Lerp(Vector3f a, Vector3f b, Vector3f t) {
		return (_mm_set1_ps(1) - t) * a + t * b;
	}

	class Vector3i {
	public:
		int x, y, z;

	public:
		Vector3i() { x = y = z = 0; }
		Vector3i(int xx, int yy, int zz) { x = xx;  y = yy; z = zz; }
		Vector3i(const Vector3i& a) { x = a.x; y = a.y; z = a.z; }
		Vector3i(const Vector3f& a) { x = a.X(); y = a.Y(); z = a.Z(); }

		__forceinline int operator[](int idx) const {
			return (&x)[idx];
		}

		__forceinline int& operator[](int idx) {
			return (&x)[idx];
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y, 2 = z
		__forceinline int MajorComponent() const {
			if (x > y&& x > z) return 0;
			else if (y > z) return 1;
			else return 2;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y, 2 = z
		__forceinline int MinorComponent() const {
			if (x > y&& x > z) {
				if (y > z) return 1;
				else return 2;
			}
			else if (y > z) {
				if (x > z) return 0;
				else return 2;
			}
			else {
				if (x > y) return 0;
				else return 1;
			}
		}

		__forceinline bool operator==(const Vector3i& a) const {
			return x == a.x && y == a.y && z == a.z;
		}

		__forceinline bool operator!=(const Vector3i& a) const {
			return x != a.x || y != a.y || z != a.z;
		}
	};

	__forceinline Vector3i operator+(const Vector3i& a, const Vector3i& b) {
		return Vector3i(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	__forceinline Vector3i operator-(const Vector3i& a, const Vector3i& b) {
		return Vector3i(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	__forceinline Vector3i operator*(const Vector3i& a, const Vector3i& b) {
		return Vector3i(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	__forceinline Vector3f operator*(const Vector3i& a, const Vector3f& b) {
		return Vector3f(a.x * b.X(), a.y * b.Y(), a.z * b.Z());
	}

	__forceinline Vector3i operator/(const Vector3i& a, const Vector3i& b) {
		return Vector3i(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	__forceinline Vector3f operator/(const Vector3i& a, const Vector3f& b) {
		return Vector3f(a.x / b.X(), a.y / b.Y(), a.z / b.Z());
	}

#endif

	//-------------------------------------------------------------
	//                       Vector2
	//-------------------------------------------------------------
	template <class T>
	class Vector2{
	public:
		T x, y;

	public:
		Vector2(){
			x = y = 0;
		}

		Vector2(T cc){
			x = cc;
			y = cc;
		}

		Vector2(T xx, T yy){
			x = xx;
			y = yy;
		}

		template<class U>
		Vector2(const Vector2<U>& a){
			x = a.x;
			y = a.y;
		}

#if !((defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE)))
		Vector2(Vector3f a) {
			x = a.X();
			y = a.Y();
		}

		Vector2(Vector4f a) {
			x = a.X();
			y = a.Y();
		}
#else
		template <class U>
		Vector2(const Vector3<U>& a){
			x = a.x;
			y = a.y;
		}

		template <class U>
		Vector2(const Vector4<U>& a) {
			x = a.x;
			y = a.y;
		}
#endif

		__forceinline Float LengthSquare() const{
			return x*x + y*y;
		}

		__forceinline Float Length() const{
			return sqrt(LengthSquare());
		}

		__forceinline bool IsNormalized() const {
			return LengthSquare() - 1 < Epsilon;
		}

		//choose the maximum component of vector
		//return a idx that 0 = x, 1 = y
		__forceinline int MajorComponent() const {
			if (x > y) return 0;
			else return 1;
		}

		//choose the minor component of vector
		//return a idx that 0 = x, 1 = y
		__forceinline int MinorComponent() const {
			if (x > y) return 1;
			else return 0;
		}

		__forceinline T operator[](int idx) const{
			return (&x)[idx];
		}

		__forceinline T& operator[](int idx){
			return (&x)[idx];
		}

		template <class U>
		__forceinline bool operator==(const Vector2<U>& n) const{
			return fabs(x - n.x) < Epsilon && fabs(y - n.y) < Epsilon;
		}

		template <class U>
		__forceinline bool operator!=(const Vector2<U>& n) const{
			return !(*this == n);
		}

		template <class U>
		__forceinline Vector2<T>& operator=(const Vector2<U>& n){
			x = n.x;
			y = n.y;
			return *this;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			for (int i = 0; i < 2; ++i) {
				ret += std::to_string((*this)[i]);
				if(i < 1) ret += ", ";
			}

			return "[" + ret + "]";
		}
	};

	template <class T>
	__forceinline Vector2<T> operator-(const Vector2<T>& a){
		return Vector2<T>(-a.x, -a.y);
	}

	template <class T>
	__forceinline Vector2<T> operator+(const Vector2<T>& a, const Vector2<T>& b){
		return Vector2<T>(a.x + b.x, a.y + b.y);
	}

	template <class T>
	__forceinline Vector2<T> operator+(const Vector2<T>& a, T scalar) {
		return Vector2<T>(a.x + scalar, a.y + scalar);
	}

	template <class T>
	__forceinline Vector2<T> operator-(const Vector2<T>& a, const Vector2<T>& b){
		return Vector2<T>(a.x - b.x, a.y - b.y);
	}

	template <class T>
	__forceinline Vector2<T> operator-(const Vector2<T>& a, T scalar) {
		return Vector2<T>(a.x - scalar, a.y - scalar);
	}

	template <class T>
	__forceinline Vector2<T> operator*(const Vector2<T>& a, T scalar){
		return Vector2<T>(a.x*scalar, a.y*scalar);
	}

	template <class T>
	__forceinline Vector2<T> operator*(T scalar, const Vector2<T>& a){
		return Vector2<T>(a.x*scalar, a.y*scalar);
	}

	template <class T, class U>
	__forceinline Vector2<T> operator*(const Vector2<T>& a, const Vector2<U>& b) {
		return Vector2<T>(a.x * b.x, a.y * b.y);
	}

	template <class T>
	__forceinline Vector2<T> operator/(const Vector2<T>& a, T scalar){
		return Vector2<T>(a.x / scalar, a.y / scalar);
	}

	template <class T>
	__forceinline Vector2<T> operator/(T scalar, const Vector2<T>& a){
		return Vector2<T>(scalar / a.x, scalar / a.y);
	}

	template <class T, class U>
	__forceinline Vector2<T> operator/(const Vector2<T>& a, const Vector2<U>& b){
		return Vector2<T>(a.x / b.x, a.y / b.y);
	}

	template <class T>
	__forceinline Vector2<T>& operator+=(Vector2<T>& a, const Vector2<T>& b){
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator+=(Vector2<T>& a, Float scalar) {
		a.x += scalar;
		a.y += scalar;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator-=(Vector2<T>& a, const Vector2<T>& b){
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator-=(Vector2<T>& a, Float scalar) {
		a.x -= scalar;
		a.y -= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator*=(Vector2<T>& a, T scalar){
		a.x *= scalar;
		a.y *= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator*=(Vector2<T>& a, const Vector2<T>& b) {
		a.x *= b.x;
		a.y *= b.y;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator/=(Vector2<T>& a, T scalar){
		a.x /= scalar;
		a.y /= scalar;
		return a;
	}

	template <class T>
	__forceinline Vector2<T>& operator/=(Vector2<T>& a, const Vector2<T>& b) {
		a.x /= b.x;
		a.y /= b.y;
		return a;
	}

	template <class T>
	__forceinline Vector2<T> Normalize(const Vector2<T>& a){
		return a / a.Length();
	}

	template <class T>
	__forceinline T Dot(const Vector2<T>& a, const Vector2<T>& b){
		return a.x*b.x + a.y*b.y;
	}

	typedef Vector2<int> Vector2i;
	typedef Vector2<Float> Vector2f;

	//-------------------------------------------------------------
	//                       Ray
	//-------------------------------------------------------------
	class Ray{
	public:
		Vector3f o; //origin
		Float tmin; //for avoid self-intersect
		Vector3f d; //direction
		Float tmax;

	public:
		Ray()
		:tmin(Epsilon), tmax(INFINITY){}

		Ray(const Vector3f& origin, const Vector3f& direction, Float tmin = Epsilon, Float tmax = INFINITY)
			:o(origin), d(direction), tmin(tmin), tmax(tmax){}

		//p = o + t*d
		__forceinline Vector3f operator()(Float t) const{
			return o + t * d;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Ray[\n  o = " + o.ToString()
				+ ",\n  d = " + d.ToString()
				+ ",\n  tmin = " + std::to_string(tmin)
				+ ",\n  tmax = " + std::to_string(tmax)
				+ "\n]";

			return ret;
		}
	};

	//-------------------------------------------------------------
	//                       Bounds2
	//-------------------------------------------------------------
	template <class T>
	class Bounds2{
	public:
		Vector2<T> min, max;

	public:
		Bounds2(){
			Reset();
		}

		Bounds2(const Vector2<T>& min, const Vector2<T>& max)
			:min(min), max(max){}

		//reset bbox
		__forceinline void Reset(){
			T minNum = std::numeric_limits<T>::lowest();
			T maxNum = std::numeric_limits<T>::max();
			min = Vector2<T>(maxNum);
			max = Vector2<T>(minNum);
		}

		//expand bounds by point p
		__forceinline void Union(const Vector2<T>& p){
			min.x = Min(min.x, p.x);
			min.y = Min(min.y, p.y);

			max.x = Max(max.x, p.x);
			max.y = Max(max.y, p.y);
		}

		//expand bounds by bbox b
		__forceinline void Union(const Bounds2<T>& b){
			min.x = Min(min.x, b.min.x);
			min.y = Min(min.y, b.min.y);

			max.x = Max(max.x, b.max.x);
			max.y = Max(max.y, b.max.y);
		}

		//get center point of bounds
		__forceinline Vector2<T> Center() const{
			return (min + max) / T(2);
		}

		//get diagonal of bounds
		__forceinline Vector2<T> Diagonal() const{
			return max - min;
		}

		//
		__forceinline Vector2<T> Offset(const Vector2<T>& p) const{
			return (p - min) / Diagonal();
		}

		//get volume of bounds
		__forceinline Float SurfaceArea() const{
			Vector2<T> diag = Diagonal();
			return diag.x*diag.y;
		}

		//get max extent of bounds and return a integer indices the largest edge
		//x = 0, y = 1
		__forceinline int MaxExtent() const{
			Vector2<T> diag = Diagonal();
			if (diag.x > diag.y && diag.x > diag.z)
				return 0;
			else if (diag.y > diag.z)
				return 1;
		}

		__forceinline bool operator==(const Bounds2<T>& b) const{
			return min == b.min && max == b.max;
		}

		__forceinline bool operator!=(const Bounds2<T>& b) const{
			return min != b.min || max != b.max;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Bounds2[\n  min = " + min.ToString()
				+ ",\n  max = " + max.ToString()
				+ "\n]";

			return ret;
		}
	};

	//-------------------------------------------------------------
	//                       BBox
	//-------------------------------------------------------------
	class BBox{

	public:
		Vector3f fmin, fmax;

	public:
		BBox()
			:fmin(INFINITY), fmax(-INFINITY){}

		BBox(const Vector3f& fmin, const Vector3f& fmax)
			:fmin(fmin), fmax(fmax){}

		//reset bbox
		__forceinline void Reset(){
			fmin = Vector3f(INFINITY);
			fmax = Vector3f(-INFINITY);
		}

		//expand bbox by point p
		__forceinline void Union(const Vector3f& p){
#if !((defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE)))
			fmin = Min(fmin, p);
			fmax = Max(fmax, p);
#else
			fmin.x = Min(fmin.x, p.x);
			fmin.y = Min(fmin.y, p.y);
			fmin.z = Min(fmin.z, p.z);

			fmax.x = Max(fmax.x, p.x);
			fmax.y = Max(fmax.y, p.y);
			fmax.z = Max(fmax.z, p.z);
#endif
		}

		//expand bbox by bbox b
		__forceinline void Union(const BBox& b){
#if !((defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE)))
			fmin = Min(fmin, b.fmin);
			fmax = Max(fmax, b.fmax);
#else
			fmin.x = Min(fmin.x, b.fmin.x);
			fmin.y = Min(fmin.y, b.fmin.y);
			fmin.z = Min(fmin.z, b.fmin.z);

			fmax.x = Max(fmax.x, b.fmax.x);
			fmax.y = Max(fmax.y, b.fmax.y);
			fmax.z = Max(fmax.z, b.fmax.z);
#endif
		}

		//get center point of bbox
		__forceinline Vector3f Center() const{
			return (fmin + fmax) * Float(0.5);
		}

		//get diagonal of bbox
		__forceinline Vector3f Diagonal() const{
			return fmax - fmin;
		}

		//
		__forceinline Vector3f Offset(const Vector3f& p) const{
			return (p - fmin) / Diagonal();
		}

		__forceinline Vector3f Lerp(const Vector3f& p) const {
#if !((defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE)))
			return brick::Lerp(fmin, fmax, p);
#else
			return Vector3f(brick::Lerp(fmin.x, fmax.x, p.x),
				brick::Lerp(fmin.y, fmax.y, p.y),
				brick::Lerp(fmin.z, fmax.z, p.z));
#endif
		}

		//get surface area of bbox
		__forceinline Float SurfaceArea() const{
			Vector3f diag = Diagonal();
			Float x = diag.X();
			Float y = diag.Y();
			Float z = diag.Z();
			return 2.f * (x * y + x * z + y * z);
		}

		//get volume of bbox
		__forceinline Float Volume() const{
			Vector3f diag = Diagonal();
			return diag.X()*diag.Y()*diag.Z();
		}

		//get max extent of bbox and return a integer indices the largest edge
		//x = 0, y = 1, z = 2
		__forceinline int MaxExtent() const{
			Vector3f diag = Diagonal();

			return diag.MajorComponent();
		}

		//get bouding sphere of bbox
		__forceinline void BoundingSphere(Vector3f& o, Float& r) const{
			o = Center();
			r = (fmax - o).Length();
		}

		//test if a ray intersect with bbox
		__forceinline bool Intersect(const Ray& r, const Vector3f& invDir/*for faster*/) const{
			//The intersection condition:
			//the maximum t value of the ray entering the plane is smaller than 
			//the minimum t value leaving the plane
#if !((defined(BRICK_USING_DOUBLE_PRECISE)\
                     || !defined(BRICK_USING_SSE)))
			Vector3f t0 = (fmin - r.o) * invDir;
			Vector3f t1 = (fmax - r.o) * invDir;

			Vector3f v0 = Min(t0, t1);
			Vector3f v1 = Max(t0, t1);

			Float tmin = HMax(v0);
			Float tmax = HMin(v1);

			if (tmax <= 0.00001f) return false;//bbox behind ray
			if (tmin > tmax || tmin > r.tmax)
				return false;

			return true;
#else
			float t1 = (fmin.x - r.o.x)*invDir.x;
			float t2 = (fmax.x - r.o.x)*invDir.x;
			float t3 = (fmin.y - r.o.y)*invDir.y;
			float t4 = (fmax.y - r.o.y)*invDir.y;
			float t5 = (fmin.z - r.o.z)*invDir.z;
			float t6 = (fmax.z - r.o.z)*invDir.z;

			//tmin tmax is intersection point if intersect
			float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
			float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));
			if (tmax <= 0.00001f) return false;//bbox behind ray
			if (tmin > tmax || tmin > r.tmax)
				return false;

			return true;
#endif
		}

		__forceinline bool operator==(const BBox& b) const{
			return fmin == b.fmin && fmax == b.fmax;
		}

		__forceinline bool operator!=(const BBox& b) const{
			return fmin != b.fmin || fmax != b.fmax;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "BBox[\n  min = " + fmin.ToString()
				+ ",\n  max = " + fmax.ToString()
				+ "\n]";

			return ret;
		}
	};

	//-------------------------------------------------------------
	//                       Matrix4
	//-------------------------------------------------------------
	class Matrix4{
	public:
		Vector4f m[4];

	public:
		Matrix4(){
			m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
			m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
			m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
			m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
		}

		Matrix4(Float t00, Float t01, Float t02, Float t03,
			Float t10, Float t11, Float t12, Float t13,
			Float t20, Float t21, Float t22, Float t23,
			Float t30, Float t31, Float t32, Float t33){
			m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
			m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
			m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
			m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
		}

		Matrix4(Float mat[]){
			Float* p = (Float*)m;
			for (int i = 0; i < 16; ++i)
				p[i] = mat[i];
		}

		Matrix4(const Matrix4& mat){
			m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2]; m[0][3] = mat[0][3];
			m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2]; m[1][3] = mat[1][3];
			m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2]; m[2][3] = mat[2][3];
			m[3][0] = mat[3][0]; m[3][1] = mat[3][1]; m[3][2] = mat[3][2]; m[3][3] = mat[3][3];
		}

		//identity matrix
		//when i==j => m[i][j] = 1
		//when i!=j => m[i][j] = 0
		__forceinline bool IsIdentity() const{
			return fabs(m[0][0] - 1) < Epsilon && fabs(m[0][1]) < Epsilon && fabs(m[0][2]) < Epsilon && fabs(m[0][3]) < Epsilon &&
				fabs(m[1][0]) < Epsilon && fabs(m[1][1] - 1) < Epsilon && fabs(m[1][2]) < Epsilon && fabs(m[1][3]) < Epsilon &&
				fabs(m[2][0]) < Epsilon && fabs(m[2][1]) < Epsilon && fabs(m[2][2] - 1) < Epsilon && fabs(m[2][3]) < Epsilon &&
				fabs(m[3][0]) < Epsilon && fabs(m[3][1]) < Epsilon && fabs(m[3][2]) < Epsilon && fabs(m[3][3] - 1) < Epsilon;
		}

		//orthogonal matrix
		//ATA = AAT = I
		//arbitrarily two column or row of matrix are orthogonal
		//every column of matrix has length 1 
		//rotation matrix are always orthogonal
		__forceinline bool IsOrthogonal() const{
			if (fabs(Dot(m[0], m[1])) > Epsilon) return false;
			if (fabs(Dot(m[0], m[2])) > Epsilon) return false;
			if (fabs(Dot(m[0], m[3])) > Epsilon) return false;
			if (fabs(Dot(m[1], m[2])) > Epsilon) return false;
			if (fabs(Dot(m[1], m[3])) > Epsilon) return false;
			if (fabs(Dot(m[2], m[3])) > Epsilon) return false;

			if (fabs(fabs(m[0].LengthSquare() - 1)) > Epsilon) return false;
			if (fabs(fabs(m[1].LengthSquare() - 1)) > Epsilon) return false;
			if (fabs(fabs(m[2].LengthSquare() - 1)) > Epsilon) return false;
			if (fabs(fabs(m[3].LengthSquare() - 1)) > Epsilon) return false;

			return true;
		}

		//determinant = 0
		//if arbitrarily column(row) of matrix is a linear combination
		//of other column(row) of matrix, then matrix is singular
		__forceinline bool IsSingular() const{
			return fabs(Determinant()) < Epsilon;
		}

		//| a  b  c  d |
		//| e  f  g  h |
		//| i  j  k  l |
		//| m  n  o  p |
		//cofactor method
		__forceinline Float Determinant() const{
			Float m00 = m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) +
				-m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3]) +
				m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
			Float m01 = -m[0][1] * (m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) +
				-m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) +
				m[1][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
			Float m02 = m[0][2] * (m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3]) +
				-m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) +
				m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
			Float m03 = -m[0][3] * (m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]) +
				-m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]) +
				m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

			return m00 + m01 + m02 + m03;
		}

		//m[i][j] = m[j][i]
		//the inverse of orthonormal matrix is the same as its transpose (A(transpose) = A(inverse))
		__forceinline Matrix4 Transpose() const{
			Matrix4 ret;
			ret[0][0] = m[0][0];
			ret[0][1] = m[1][0];
			ret[0][2] = m[2][0];
			ret[0][3] = m[3][0];
			ret[1][0] = m[0][1];
			ret[1][1] = m[1][1];
			ret[1][2] = m[2][1];
			ret[1][3] = m[3][1];
			ret[2][0] = m[0][2];
			ret[2][1] = m[1][2];
			ret[2][2] = m[2][2];
			ret[2][3] = m[3][2];
			ret[3][0] = m[0][3];
			ret[3][1] = m[1][3];
			ret[3][2] = m[2][3];
			ret[3][3] = m[3][3];

			return ret;
		}

		//| a  b  c  d |      1  | 
		//| e  f  g  h | ->  --- | 
		//| i  j  k  l |	 |d| | 
		//| m  n  o  p |         | 
		//transposition of adjoint matrix divided by determinant
		__forceinline Matrix4 Inverse() const{
			Matrix4 ret;
			Float det = Determinant();
			if (fabs(det) < Epsilon){
				//matrix are singular, so return identity matrix
				return ret;
			}

			Float invDet = 1.0 / det;
			ret[0][0] = (m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) -
				m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3]) +
				m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][3]))*invDet;
			ret[0][1] = -(m[0][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) -
				m[0][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3]) +
				m[0][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]))*invDet;
			ret[0][2] = (m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3]) -
				m[0][2] * (m[1][1] * m[3][3] - m[3][1] * m[1][3]) +
				m[0][3] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]))*invDet;
			ret[0][3] = -(m[0][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3]) -
				m[0][2] * (m[1][1] * m[2][3] - m[2][1] * m[1][3]) +
				m[0][3] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]))*invDet;
			ret[1][0] = -(m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) -
				m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) +
				m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]))*invDet;
			ret[1][1] = (m[0][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) -
				m[0][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) +
				m[0][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]))*invDet;
			ret[1][2] = -(m[0][0] * (m[1][2] * m[3][3] - m[3][2] * m[1][3]) -
				m[0][2] * (m[1][0] * m[3][3] - m[3][0] * m[1][3]) +
				m[0][3] * (m[1][0] * m[3][2] - m[3][0] * m[1][2]))*invDet;
			ret[1][3] = (m[0][0] * (m[1][2] * m[2][3] - m[2][2] * m[1][3]) -
				m[0][2] * (m[1][0] * m[2][3] - m[2][0] * m[1][3]) +
				m[0][3] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]))*invDet;
			ret[2][0] = (m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3]) -
				m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) +
				m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]))*invDet;
			ret[2][1] = -(m[0][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3]) -
				m[0][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3]) +
				m[0][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]))*invDet;
			ret[2][2] = (m[0][0] * (m[1][1] * m[3][3] - m[3][1] * m[1][3]) -
				m[0][1] * (m[1][0] * m[3][3] - m[3][0] * m[1][3]) +
				m[0][3] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]))*invDet;
			ret[2][3] = -(m[0][0] * (m[1][1] * m[2][3] - m[2][1] * m[1][3]) -
				m[0][1] * (m[1][0] * m[2][3] - m[2][0] * m[1][3]) +
				m[0][3] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]))*invDet;
			ret[3][0] = -(m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]) -
				m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][3]) +
				m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][3]))*invDet;
			ret[3][1] = (m[0][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]) -
				m[0][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]) +
				m[0][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]))*invDet;
			ret[3][2] = -(m[0][0] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]) -
				m[0][1] * (m[1][0] * m[3][2] - m[3][0] * m[1][2]) +
				m[0][2] * (m[1][0] * m[3][1] - m[3][0] * m[2][1]))*invDet;
			ret[3][3] = (m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
				m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]) +
				m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]))*invDet;

			return ret;
		}

		__forceinline Matrix4 operator*(const Matrix4& mat) const{
			Matrix4 ret;
			//64 mul and 48 addition
			ret[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
			ret[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
			ret[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
			ret[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];
			ret[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
			ret[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
			ret[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
			ret[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];
			ret[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
			ret[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
			ret[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
			ret[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];
			ret[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
			ret[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
			ret[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
			ret[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];

			return ret;
		}

		__forceinline Vector4f operator*(const Vector4f& v) const{
			Vector4f ret;
			ret[0] = Dot(m[0], v);
			ret[1] = Dot(m[1], v);
			ret[2] = Dot(m[2], v);
			ret[3] = Dot(m[3], v);

			return ret;
		}

		__forceinline Vector4f operator[](int idx) const{
			return m[idx];
		}

		__forceinline Vector4f& operator[](int idx){
			return m[idx];
		}

		__forceinline bool operator==(const Matrix4& mat) const{
			return m[0] == mat[0] && m[1] == mat[1] && m[2] == mat[2] && m[3] == mat[3];
		}

		__forceinline bool operator!=(const Matrix4& mat) const{
			return m[0] != mat[0] || m[1] != mat[1] || m[2] != mat[2] || m[3] != mat[3];
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Matrix4[\n  " + m[0].ToString()
				+ ",\n  " + m[1].ToString()
				+ ",\n  " + m[2].ToString()
				+ ",\n  " + m[3].ToString()
				+ "\n]";
			
			return ret;
		}
	};

	//-------------------------------------------------------------
	//                       Matrix3
	//-------------------------------------------------------------
	class Matrix3{
	public:
		Vector3f m[3];

	public:
		Matrix3(){
			m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
			m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
			m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
		}

		Matrix3(Float t00, Float t01, Float t02,
			Float t10, Float t11, Float t12,
			Float t20, Float t21, Float t22){
			m[0][0] = t00; m[0][1] = t01; m[0][2] = t02;
			m[1][0] = t10; m[1][1] = t11; m[1][2] = t12;
			m[2][0] = t20; m[2][1] = t21; m[2][2] = t22;
		}

		Matrix3(Float mat[]){
			Float* p = (Float*)m;
			for (int i = 0; i < 9; ++i)
				p[i] = mat[i];
		}

		Matrix3(const Matrix3& mat){
			m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2];
			m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2];
			m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2];
		}

		Matrix3(const Matrix4& mat){
			m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2];
			m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2];
			m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2];
		}

		//identity matrix
		//when i==j => m[i][j] = 1
		//when i!=j => m[i][j] = 0
		__forceinline bool IsIdentity() const{
			return fabs(m[0][0] - 1) < Epsilon && fabs(m[0][1]) < Epsilon && fabs(m[0][2]) < Epsilon &&
				fabs(m[1][0]) < Epsilon && fabs(m[1][1] - 1) < Epsilon && fabs(m[1][2]) < Epsilon &&
				fabs(m[2][0]) < Epsilon && fabs(m[2][1]) < Epsilon && fabs(m[2][2] - 1) < Epsilon;
		}

		//orthogonal matrix
		//ATA = AAT = I
		//arbitrarily two column or row of matrix are orthogonal
		//every column of matrix has length 1 
		//rotation matrix are always orthogonal
		__forceinline bool IsOrthogonal() const{
			if (fabs(Dot(m[0], m[1])) > Epsilon) return false;
			if (fabs(Dot(m[0], m[2])) > Epsilon) return false;
			if (fabs(Dot(m[1], m[2])) > Epsilon) return false;

			if (fabs(m[0].LengthSquare() - 1) > Epsilon) return false;
			if (fabs(m[1].LengthSquare() - 1) > Epsilon) return false;
			if (fabs(m[2].LengthSquare() - 1) > Epsilon) return false;

			return true;
		}

		//determinant = 0
		//if arbitrarily column(row) of matrix is a linear combination
		//of other column(row) of matrix, then matrix is singular
		__forceinline bool IsSingular() const{
			return fabs(Determinant()) < Epsilon;
		}

		__forceinline Float Determinant() const{
			return m[0][0] * m[1][1] * m[2][2] +
				m[0][1] * m[1][2] * m[2][0] +
				m[0][2] * m[1][0] * m[2][1] -
				m[0][2] * m[1][1] * m[2][0] -
				m[0][1] * m[1][0] * m[2][2] -
				m[0][0] * m[1][2] * m[2][1];
		}

		//m[i][j] = m[j][i]
		//the inverse of orthonormal matrix is the same as its transpose (A(transpose) = A(inverse))
		__forceinline Matrix3 Transpose() const{
			Matrix3 ret;
			ret[0][0] = m[0][0];
			ret[0][1] = m[1][0];
			ret[0][2] = m[2][0];
			ret[1][0] = m[0][1];
			ret[1][1] = m[1][1];
			ret[1][2] = m[2][1];
			ret[2][0] = m[0][2];
			ret[2][1] = m[1][2];
			ret[2][2] = m[2][2];

			return ret;
		}

		//| a  b  c |      1  | (ei-hf)  -(bi-hc) (bf-ec)  |
		//| d  e  f | ->  --- | -(di-gf) (ai-gc)  -(af-dc) |
		//| g  h  i |     |d| | (dh-ge)  -(ah-gb) (ae-db)  |
		//transposition of adjoint matrix divided by determinant
		__forceinline Matrix3 Inverse() const{
			Matrix3 ret;
			Float det = Determinant();
			if (fabs(det) < Epsilon){
				//matrix are singular, so return identity matrix
				return ret;
			}

			Float invDet = 1.0 / det;
			ret[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
			ret[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * invDet;
			ret[0][2] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * invDet;
			ret[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * invDet;
			ret[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * invDet;
			ret[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * invDet;
			ret[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
			ret[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * invDet;
			ret[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;

			return ret;
		}

		__forceinline Matrix3 operator*(const Matrix3& mat) const{
			Matrix3 ret;
			ret[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0];
			ret[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1];
			ret[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2];
			ret[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0];
			ret[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1];
			ret[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2];
			ret[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0];
			ret[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1];
			ret[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2];

			return ret;
		}

		__forceinline Vector3f operator*(const Vector3f v) const{
			Vector3f ret;
			ret[0] = Dot(m[0], v);
			ret[1] = Dot(m[1], v);
			ret[2] = Dot(m[2], v);

			return ret;
		}

		__forceinline Vector3f operator[](int idx) const{
			return m[idx];
		}

		__forceinline Vector3f& operator[](int idx){
			return m[idx];
		}

		__forceinline bool operator==(const Matrix3& mat) const{
			return m[0] == mat[0] && m[1] == mat[1] && m[2] == mat[2];
		}

		__forceinline bool operator!=(const Matrix3& mat) const{
			return m[0] != mat[0] || m[1] != mat[1] || m[2] != mat[2];
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Matrix3[\n  " + m[0].ToString()
				+ ",\n  " + m[1].ToString()
				+ ",\n  " + m[2].ToString()
				+ "\n]";

			return ret;
		}
	};

	//-------------------------------------------------------------
	//                       Matrix2
	//-------------------------------------------------------------
	class Matrix2{
	public:
		Vector2f m[2];

	public:
		Matrix2(){
			m[0][0] = 1; m[0][1] = 0;
			m[1][0] = 0; m[1][1] = 1;
		}

		Matrix2(Float t00, Float t01,
			Float t10, Float t11){
			m[0][0] = t00; m[0][1] = t01;
			m[1][0] = t10; m[1][1] = t11;
		}

		Matrix2(Float mat[]){
			Float* p = (Float*)m;
			for (int i = 0; i < 4; ++i)
				p[i] = mat[i];
		}

		Matrix2(const Matrix2& mat){
			m[0][0] = mat[0][0]; m[0][1] = mat[0][1];
			m[1][0] = mat[1][0]; m[1][1] = mat[1][1];
		}

		Matrix2(const Matrix3& mat){
			m[0][0] = mat[0][0]; m[0][1] = mat[0][1];
			m[1][0] = mat[1][0]; m[1][1] = mat[1][1];
		}

		Matrix2(const Matrix4& mat){
			m[0][0] = mat[0][0]; m[0][1] = mat[0][1];
			m[1][0] = mat[1][0]; m[1][1] = mat[1][1];
		}

		//identity matrix
		//when i==j => m[i][j] = 1
		//when i!=j => m[i][j] = 0
		__forceinline bool IsIdentity() const{
			return fabs(m[0][0] - 1) < Epsilon && fabs(m[0][1]) < Epsilon && fabs(m[1][0]) < Epsilon && fabs(m[1][1] - 1) < Epsilon;
		}

		//orthogonal matrix
		//ATA = AAT = I
		//arbitrarily two column or row of matrix are orthogonal
		//every column of matrix has length 1 
		//rotation matrix are always orthogonal
		__forceinline bool IsOrthogonal() const{
			if (fabs(Dot(m[0], m[1])) > Epsilon) return false;

			if (fabs(m[0].LengthSquare() - 1) > Epsilon) return false;
			if (fabs(m[1].LengthSquare() - 1) > Epsilon) return false;

			return true;
		}

		//determinant = 0
		//if arbitrarily column(row) of matrix is a linear combination
		//of other column(row) of matrix, then matrix is singular
		__forceinline bool IsSingular() const{
			return fabs(Determinant()) < Epsilon;
		}

		__forceinline Float Determinant() const{
			return m[0][0] * m[1][1] - m[0][1] * m[1][0];
		}

		//m[i][j] = m[j][i]
		//the inverse of orthonormal matrix is the same as its transpose (A(transpose) = A(inverse))
		__forceinline Matrix2 Transpose() const{
			Matrix2 ret;
			ret[0][0] = m[0][0];
			ret[0][1] = m[1][0];
			ret[1][0] = m[0][1];
			ret[1][1] = m[1][1];

			return ret;
		}

		//| a    b |      1  | d    -b |
		//|        | ->  --- |         |
		//| c    d |     |d| | -c    a |
		__forceinline Matrix2 Inverse() const{
			Matrix2 ret;
			Float det = Determinant();
			if (fabs(det) < Epsilon){
				//matrix are singular, return identity matrix
				return ret;
			}

			Float invDet = 1.0 / det;
			ret[0][0] = m[1][1] * invDet;
			ret[0][1] = -m[0][1] * invDet;
			ret[1][0] = -m[1][0] * invDet;
			ret[1][1] = m[0][0] * invDet;

			return ret;
		}

		__forceinline Matrix2 operator*(const Matrix2& mat) const{
			Matrix2 ret;
			ret[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0];
			ret[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1];
			ret[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0];
			ret[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1];

			return ret;
		}

		template <class T>
		__forceinline Vector2<T> operator*(const Vector2<T>& v) const{
			Vector2<T> ret;
			ret.x = Dot(m[0], v);
			ret.y = Dot(m[1], v);

			return ret;
		}

		__forceinline Vector2f operator[](int idx) const{
			return m[idx];
		}

		__forceinline Vector2f& operator[](int idx){
			return m[idx];
		}

		__forceinline bool operator==(const Matrix2& mat) const{
			return m[0] == mat[0] && m[1] == mat[1];
		}

		__forceinline bool operator!=(const Matrix2& mat) const{
			return m[0] != mat[0] || m[1] != mat[1];
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Matrix2[\n  " + m[0].ToString()
				+ ",\n  " + m[1].ToString()
				+ "\n]";

			return ret;
		}
	};

	//-------------------------------------------------------------
	//                       Quaternion
	//-------------------------------------------------------------
	//
	//q^ = (qv, qw) = iqx + jqy + kqz + qw = qv + qw
	//qv = iqx + jqy + kqz = (qx, qy, qz)
	//i2 = j2 = k2 = -1, jk = -kj = i, ki = -ik = j, ij = -ji = k
	//qw is real part of quaternion.
	//qv is imaginary part of quaternion
	//i,j,and k are called imaginary units.
	class Quaternion{
	public:
		Float x, y, z, w;

	public:
		Quaternion()
			:x(0), y(0), z(0), w(1){}

		Quaternion(Float x, Float y, Float z, Float w)
			:x(x), y(y), z(z), w(w){}

		Quaternion(const Vector4f& v)
			:x(v.X()), y(v.Y()), z(v.Z()), w(v.W()){}

		//let p^ = (px, py, pz, pw)T
		//let q^ = (sin(t)uq, cos(t))
		//then q^p^q^-1 rotates p^ around the axis uq by an angle 2t
		//if q^ is unit quaternion, then q^p^q^* = q^p^q^-1
		//any nonzero real multiple of q^ also represents the same transform,
		//which means that q^ and -q^ represent the same rotation.
		Quaternion(const Vector3f& uq, Float theta){
			Float radians = Radians(theta * 0.5);
			Float sint = sin(radians);
			Float cost = cos(radians);
			x = uq.X()*sint;
			y = uq.Y()*sint;
			z = uq.Z()*sint;
			w = cost;
		}

		//q = (0, 0, 0, 1)
		//reset quaternion
		__forceinline void Identity(){
			x = 0;
			y = 0;
			z = 0;
			w = 1;
		}

		__forceinline bool IsIdentity() const{
			return fabs(x) < Epsilon &&
				   fabs(y) < Epsilon &&
				   fabs(z) < Epsilon &&
				   fabs(w - 1) < Epsilon;
		}

		//conjugate rules: (q*)* = q
		//                 (q + r)* = q* + r*
		//                 (qr)* = (r*)(q*)
		//
		//q = (-qv, qw)
		//return a conjugate quaterion
		__forceinline Quaternion Conjugate() const{
			Quaternion conj(-x, -y, -z, w);

			return conj;
		}

		//n(q) = sqrt(qq*) = sqrt((q*)q) where q* means conjugate of q
		//norm rules:   n(q*) = n(q)
		//              n(q^r^) = n(q^)n(r^)
		//
		//q = (0, 0, 0, sqrt(qx2 + qy2+ qz2+ qw2))
		__forceinline Float Module() const{
			return sqrt(x*x + y*y + z*z + w*w);
		}

		__forceinline Float Module2() const{
			return x*x + y*y + z*z + w*w;
		}

		//for complex numbers, a two-dimensional unit vector can be written as cos(p) + isin(p) = e^(ip) [euler formular]
		//the equivalent for quaternions is q^ = sin(p)uq + cos(p) = e^(p(uq)) where dot(uq, uq) = 1
		//the log function for unit quaternion is
		//               log(q^) = log(e^(p(uq)) = p(uq)
		__forceinline Vector3f Logarithm() const{

		}

		//the power function fo unit quaternion is
		//               (q^)^t = (sin(p)uq + cos(p))^t = e^(pt(uq))
		//                      = sin(pt)uq + cos(pt)
		__forceinline Quaternion Power(Float t) const{

		}

		// qq* = n2(q), so qq*/n2(q) = 1
		//             q-1 = q*/n2(q)
		//             q-1 = q* when q is unit quaternion
		__forceinline Quaternion Inverse() const{
			Float invLensq = 1.0 / Module2();
			Quaternion ret = Conjugate();
			ret.x *= invLensq;
			ret.y *= invLensq;
			ret.z *= invLensq;
			ret.w *= invLensq;

			return ret;
		}

		__forceinline Quaternion operator+(const Quaternion& q) const{
			Quaternion ret(x + q.x, y + q.y, z + q.z, w + q.w);

			return ret;
		}

		__forceinline Quaternion operator-(const Quaternion& q) const{
			Quaternion ret(x - q.x, y - q.y, z - q.z, w - q.w);

			return ret;
		}

		//laws of multiplication:
		//linearity:     pˆ(sqˆ + tr^) = spˆqˆ + tpˆr^,
		//               (sp^ + tq^)r^ = sp^r^ + tq^r^
		//associativity: p^(q^r^) = (p^q^)r^
		//
		//
		//q^*r^ = (iqx + jqy + kqz + qw)(irx + jry + krz + rw)
		//    = i(qyrz - qzry + rwqx + qwrx)
		//    + j(qzrx - qxrz + rwqy + qwry)
		//    + k(qxry - qyrx + rwqz + qwrz)
		//    + qwrw - qxrx - qyry - qzrz
		//    = (qv*rv + rwqv + qwrv, qwrw - qv.rv)
		__forceinline Quaternion operator*(const Quaternion& q) const{
			Float xx = y*q.z - z*q.y + q.w*x + w*q.x;
			Float yy = z*q.x - x*q.z + q.w*y + w*q.y;
			Float zz = x*q.y - y*q.x + q.w*z + w*q.z;
			Float ww = w*q.w - x*q.x - y*q.y - z*q.z;
			Quaternion ret(xx, yy, zz, ww);

			return ret;
		}

		__forceinline Quaternion& operator+=(const Quaternion& q){
			x += q.x;
			y += q.y;
			z += q.z;
			w += q.w;

			return *this;
		}

		__forceinline Quaternion& operator-=(const Quaternion& q){
			x -= q.x;
			y -= q.y;
			z -= q.z;
			w -= q.w;

			return *this;
		}

		//laws of multiplication:
		//linearity:     pˆ(sqˆ + tr^) = spˆqˆ + tpˆr^,
		//               (sp^ + tq^)r^ = sp^r^ + tq^r^
		//associativity: p^(q^r^) = (p^q^)r^
		//
		//
		//q^*r^ = (iqx + jqy + kqz + qw)(irx + jry + krz + rw)
		//    = i(qyrz - qzry + rwqx + qwrx)
		//    + j(qzrx - qxrz + rwqy + qwry)
		//    + k(qxry - qyrx + rwqz + qwrz)
		//    + qwrw - qxrx - qyry - qzrz
		//    = (qv*rv + rwqv + qwrv, qwrw - qv.rv)
		__forceinline Quaternion& operator*=(const Quaternion& q){
			Float xx = y*q.z - z*q.y + q.w*x + w*q.x;
			Float yy = z*q.x - x*q.z + q.w*y + w*q.y;
			Float zz = x*q.y - y*q.x + q.w*z + w*q.z;
			Float ww = w*q.w - x*q.x - y*q.y - z*q.z;
			x = xx;
			y = yy;
			z = zz;
			w = ww;

			return *this;
		}

		__forceinline bool operator==(const Quaternion& q) const{
			return fabs(x - q.x) < Epsilon &&
				fabs(y - q.y) < Epsilon &&
				fabs(z - q.z) < Epsilon &&
				fabs(w - q.w) < Epsilon;
		}

		__forceinline bool operator!=(const Quaternion& q) const{
			return fabs(x - q.x) > Epsilon ||
				fabs(y - q.y) > Epsilon ||
				fabs(z - q.z) > Epsilon ||
				fabs(w - q.w) > Epsilon;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Quaternion[qv = [" + std::to_string(x) 
				+ ", " + std::to_string(y) + ", " +  std::to_string(z)
                + "], qw = " + std::to_string(w) + "]";

			return ret;
		}
	};

	__forceinline Quaternion operator*(const Quaternion& q, Float scalar){
		return Quaternion(q.x*scalar, q.y*scalar, q.z*scalar, q.w*scalar);
	}

	__forceinline Quaternion operator*(Float scalar, const Quaternion& q){
		return  Quaternion(q.x*scalar, q.y*scalar, q.z*scalar, q.w*scalar);
	}

	__forceinline Quaternion& operator*=(Quaternion& q, Float scalar){
		q.x *= scalar;
		q.y *= scalar;
		q.z *= scalar;
		q.w *= scalar;

		return q;
	}

	__forceinline Quaternion operator/(const Quaternion& q, Float scalar){
		return Quaternion(q.x / scalar, q.y / scalar, q.z / scalar, q.w / scalar);
	}

	__forceinline Quaternion operator/(Float scalar, const Quaternion& q){
		return Quaternion(scalar / q.x, scalar / q.y, scalar / q.z, scalar / q.w);
	}

	__forceinline Quaternion& operator/=(Quaternion& q, Float scalar){
		q.x /= scalar;
		q.y /= scalar;
		q.z /= scalar;
		q.w /= scalar;

		return q;
	}

	__forceinline Float Dot(const Quaternion& q1, const Quaternion& q2){
		return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
	}

	//    | 1-s(qy2+qz2) s(qxqy-qwqz) s(qxqz+qwqy) 0 |
	//M = | s(qxqy+qwqz) 1-s(qx2+qz2) s(qyqz-qwqx) 0 | 
	//    | s(qxqz-qwqy) s(qyqz+qwqx) 1-s(qx2+qy2) 0 |
	//    | 0            0            0            1 |
	//where s = 2 / n2(q^)
	//if q^ is unit quaternion then s = 2
	__forceinline Matrix4 QuaterionToMatrix(const Quaternion& q){
		Float s = 2.0 / q.Module2();
		Float x2 = q.x*q.x, y2 = q.y*q.y, z2 = q.z*q.z;
		Float xy = q.x*q.y, xz = q.x*q.z, xw = q.x*q.w, yz = q.y*q.z, yw = q.y*q.w, zw = q.z*q.w;
		Matrix4 m;
		m[0][0] = 1.0 - s*(y2 + z2); m[0][1] = s*(xy - zw);       m[0][2] = s*(xz + yw);       m[0][3] = 0;
		m[1][0] = s*(xy + zw);       m[1][1] = 1.0 - s*(x2 + z2); m[1][2] = s*(yz - xw);       m[1][3] = 0;
		m[2][0] = s*(xz - yw);       m[2][1] = s*(yz + xw);       m[2][2] = 1.0 - s*(x2 + y2); m[2][3] = 0;
		m[3][0] = 0;                 m[3][1] = 0;                 m[3][2] = 0;                 m[3][3] = 1.0;

		return m;
	}

	//from above we get:   m21 - m12 = 4qwqx
	//                     m02 - m20 = 4qwqy
	//                     m10 - m01 = 4qwqz
	//if qw is known, the value of vector uq can be computed.
	//the trace of M is calculate by tr(M) = 4 - 2s(qx2 + qy2 + qz2)
	//because the unit quaternion is needed, s = 2 
	//=> tr(M) = 4(1 - (qx2+qy2+qz2)), since qx2+qy2+qz2+qw2 = 1
	//         = 4qw2 .
	//this result yields the following conversion for a unit quaternion
	//  qw = sqrt(tr(M))/2, qx = (m21 - m12)/4qw, qy = (m02 - m20)/4qw, qz = (m10 - m01)/4qw
	__forceinline Quaternion MatrixToQuaternion(const Matrix4& m){
		//to have a numerically stable routine, divisions by a small numbers should be avoided.
		//therefore, first set t = qw2 - qx2 - qy2 - qz2, from which it follows that
		//                 m00 = t + 2qx2
		//                 m11 = t + 2qy2
		//                 m22 = t + 2qz2
		//                   u = m00 + m11 + m22 = t + 2qw2 = 4qw2 - 1
		//which in turn implies that the largest of m00, m11, m22, and u determine which of qx,
		//qy, qz, and qw is largest. if qw is largest, then above equation is used to derive the
		//quaternion. otherwise, we note that the following holds:
		//                 4qx2 =  m00 - m11 - m22 + m33
		//                 4qy2 = -m00 + m11 - m22 + m33
		//                 4qz2 = -m00 - m11 + m22 + m33
		//                 4qw2 = tr(M)
		Float trace = m[0][0] + m[1][1] + m[2][2];
		if (trace > 0){
			Float s = sqrt(trace + 1.0);
			Float w = s * 0.5;
			s = 0.5 / s;
			Float x = (m[2][1] - m[1][2]) * s;
			Float y = (m[0][2] - m[2][0]) * s;
			Float z = (m[1][0] - m[0][1]) * s;
			
			return Quaternion(x, y, z, w);
		}
		else{
			//compute largest of x, y, and z, then remaining component
			//note that m[0][0] = t + 2qx2, m[1][1] = t + 2qy2, m[2][2] = t + 2qz2
			//code based from pbrt
			const int nxt[3] = { 1, 2, 0 };
			Float q[3];
			int i = 0;
			if (m[1][1] > m[0][0]) i = 1; //y is largest
			if (m[2][2] > m[i][i]) i = 2; //z is largest
			int j = nxt[i];
			int k = nxt[j];
			Float s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.f/*m[3][3]*/);
			q[i] = s * 0.5;
			if (s != 0) s = 0.5 / s;
			Float w = (m[k][j] - m[j][k]) * s;
			q[j] = (m[j][i] + m[i][j]) * s;
			q[k] = (m[k][i] + m[i][k]) * s;
			Float x = q[0];
			Float y = q[1];
			Float z = q[2];

			return Quaternion(x, y, z, w);
		}
	}

	//given two unit quaternion, q^ and r^ and a parameter t-[0,1], computers an  interpolated quaternion
	//this is useful for animating objects, for example. it is not as useful for interpolating camera
	//orientations, as the camera's "up" vector can become tilted during interpolate.
	//algebraic form of this operation is expressed by the composite quaternion,
	//                            sin(p(1 - t))       sin(pt)
	//         slerp(q^, r^, t) = ------------- q^ +  ------- r^
	//                                sin(p)           sin(p)
	__forceinline Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, Float t){
		Float costheta = Dot(q1, q2);
		if (costheta > 0.9995){
			//linear interpolate to avoid numberical instably
			return (1.0 - t)*q1 + t*q2;
		}
		else{
			Float theta = acos(costheta);
			Float thetap = theta * t;
			Quaternion qlerp = (q2 - q1*costheta);
			return q1 * cos(thetap) + qlerp*sin(thetap);
		}
	}

	//-------------------------------------------------------------
	//                       Transform
	//-------------------------------------------------------------
	class Transform{
	public:
		Matrix4 m;
		Matrix4 mInv;

	public:
		Transform() {}

		Transform(const Matrix4& m)
			:m(m), mInv(m.Inverse()){}

		Transform(const Matrix4& m, const Matrix4& mInv)
			:m(m), mInv(mInv){}

		Transform(const Quaternion& q){
			m = QuaterionToMatrix(q);
			mInv = m.Inverse();
		}

		//we compute the determinant of the upper left 3 × 3 elements of the matrix.
		//if the value is negative, the matrix is reflective.
		//reflection matrix usually require special treatment. 
		//it can change vertex order and cause incorrect lighting and backface culling
		__forceinline bool IsReflectionTransform() const{
			Matrix3 mat(m);
			return mat.Determinant() < 0;
		}

		__forceinline bool IsRotationTransform() const{
			Matrix3 mat(m);
			return (mat.Transpose()*mat).IsIdentity();
		}

		__forceinline Vector3f TransformPoint(const Vector3f& p) const{
			return Vector3f(m*Vector4f(p.X(), p.Y(), p.Z(), Float(1)));
		}

		__forceinline Vector3f TransformPointInverse(const Vector3f& p) const{
			return Vector3f(mInv*Vector4f(p.X(), p.Y(), p.Z(), Float(1)));
		}

		__forceinline Vector3f TransformVector(const Vector3f& v) const{
			Matrix3 mat(m);
			return mat*v;
		}

		__forceinline Vector3f TransformVectorInverse(const Vector3f& v) const{
			Matrix3 mat(mInv);
			return mat*v;
		}

		//transform normal must be special treatment
		//Let V be the tangent of the surface and N be the normal of the surface
		//then we get 0 = N(T)V,  where N(T) means transpose of N
		//if we transform V by M then we get
		//            0 = (SN)(T)(MV)
		//            0 = N(T)S(T)MV
		//so S(T)M should be identity, ie., S(T)M = I =>S = M(IT), where M(IT) means the transpose of inverse of M
		__forceinline Vector3f TransformNormal(const Vector3f& n) const{
			Matrix3 mat(mInv.Transpose());
			return mat*n;
		}

		__forceinline Vector3f TransformNormalInverse(const Vector3f& n) const{
			Matrix3 mat(m.Transpose());
			return mat*n;
		}

		__forceinline Ray TransformRay(const Ray& r) const{
			Vector3f o = Vector3f(m * Vector4f(r.o.X(), r.o.Y(), r.o.Z(), Float(1)));
			Vector3f d = Matrix3(m) * r.d;
			Ray ray(o, d, r.tmin, r.tmax);
			return ray;
		}

		__forceinline Ray TransformRayInverse(const Ray& r) const{
			Vector3f o = Vector3f(mInv * Vector4f(r.o.X(), r.o.Y(), r.o.Z(), Float(1)));
			Vector3f d = Matrix3(mInv) * r.d;
			Ray ray(o, d, r.tmin, r.tmax);
			return ray;
		}

		__forceinline BBox TransformBBox(const BBox& b) const{
			BBox ret;
			Float minX = b.fmin.X(); Float minY = b.fmin.Y(); Float minZ = b.fmin.Z();
			Float maxX = b.fmax.X(); Float maxY = b.fmax.Y(); Float maxZ = b.fmax.Z();
			ret.Union(TransformPoint(Vector3f(minX, minY, minZ)));
			ret.Union(TransformPoint(Vector3f(minX, minY, maxZ)));
			ret.Union(TransformPoint(Vector3f(minX, maxY, minZ)));
			ret.Union(TransformPoint(Vector3f(minX, maxY, maxZ)));
			ret.Union(TransformPoint(Vector3f(maxX, minY, minZ)));
			ret.Union(TransformPoint(Vector3f(maxX, minY, maxZ)));
			ret.Union(TransformPoint(Vector3f(maxX, maxY, minZ)));
			ret.Union(TransformPoint(Vector3f(maxX, maxY, maxZ)));
			
			return ret;
		}

		__forceinline BBox TransformBBoxInverse(const BBox& b) const{
			BBox ret;
			Float minX = b.fmin.X(); Float minY = b.fmin.Y(); Float minZ = b.fmin.Z();
			Float maxX = b.fmax.X(); Float maxY = b.fmax.Y(); Float maxZ = b.fmax.Z();
			ret.Union(TransformPointInverse(Vector3f(minX, minY, minZ)));
			ret.Union(TransformPointInverse(Vector3f(minX, minY, maxZ)));
			ret.Union(TransformPointInverse(Vector3f(minX, maxY, minZ)));
			ret.Union(TransformPointInverse(Vector3f(minX, maxY, maxZ)));
			ret.Union(TransformPointInverse(Vector3f(maxX, minY, minZ)));
			ret.Union(TransformPointInverse(Vector3f(maxX, minY, maxZ)));
			ret.Union(TransformPointInverse(Vector3f(maxX, maxY, minZ)));
			ret.Union(TransformPointInverse(Vector3f(maxX, maxY, maxZ)));

			return ret;
		}

		//must be rotation transform
		//E = Rz(r)Rx(p)Ry(h)
		//| cos(r)cos(h)-sin(r)sin(p)sin(h)     -sin(r)cos(p)   cos(r)sin(h)+sin(r)sin(p)cos(h) |
		//| sin(r)cos(h)+cos(r)sin(p)sin(h)      cos(r)cos(p)   sin(r)sin(h)-cos(r)sin(p)cos(h) |
		//| -cos(p)sin(h)                        sin(p)         cos(p)cos(h)                    |
		__forceinline Vector3f ExtractEulerAngleZXY() const{
			const Matrix4& mat = m;
			
			Float p, h, r;
			Float sx = sqrt(m[2][0] * m[2][0] + m[2][2] * m[2][2]);
			bool singular = sx < Epsilon;
			if (!singular){
				p = Degree(atan2(mat[2][1], sx));
				h = Degree(atan2(-mat[2][0], mat[2][2]));
				r = Degree(atan2(-mat[0][1], mat[1][1]));
			}
			else{
				//p = 90 or -90
				p = Degree(asin(mat[2][1]));
				h = 0;
				r = Degree(atan2(mat[1][0], mat[0][0]));
			}

			return Vector3f(p, h, r);
		}

		//M = T*R*S 
		//be sure the matrix is composited by T*R*S order before call this function
		__forceinline bool DecompositionTRS(Vector3f& translate, Vector3f& rotation, Vector3f& scale) const{
			translate = Vector3f(m[0][3], m[1][3], m[2][3]);

			Float sign = IsReflectionTransform() ? -1 : 1;
			Float x = sign * sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
			Float y = sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
			Float z = sqrt(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);
			scale = Vector3f(x, y, z);
			if (x == 0 || y == 0 || z == 0){
				rotation = Vector3f(0, 0, 0);

				return false;
			}

			Matrix4 rotate(m[0][0] / x, m[0][1] / y, m[0][2] / z, 0,
						   m[1][0] / x, m[1][1] / y, m[1][2] / z, 0,
						   m[2][0] / x, m[2][1] / y, m[2][2] / z, 0,
						   0, 0, 0, 1);

			Transform t(rotate);
			rotation = t.ExtractEulerAngleZXY();

			return true;
		}

		__forceinline bool ExtractTranslation(Vector3f& translate) const {
			translate = Vector3f(m[0][3], m[1][3], m[2][3]);
			return true;
		}

		__forceinline bool ExtractRotation(Vector3f& rotation) const {
			Float sign = IsReflectionTransform() ? -1 : 1;
			Float sx = sign * sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
			Float sy = sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
			Float sz = sqrt(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);
			if (sx == 0 || sy == 0 || sz == 0) {
				rotation = Vector3f(0, 0, 0);

				return false;
			}

			Matrix4 rotate(m[0][0] / sx, m[0][1] / sy, m[0][2] / sz, 0,
				m[1][0] / sx, m[1][1] / sy, m[1][2] / sz, 0,
				m[2][0] / sx, m[2][1] / sy, m[2][2] / sz, 0,
				0, 0, 0, 1);

			Transform t(rotate);
			rotation = t.ExtractEulerAngleZXY();

			return true;
		}

		__forceinline bool ExtractScale(Vector3f& scale) const {
			Float sign = IsReflectionTransform() ? -1 : 1;
			Float x = sign * sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
			Float y = sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
			Float z = sqrt(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);
			scale = Vector3f(x, y, z);

			return true;
		}

		__forceinline bool operator==(const Transform& t){
			return m == t.m && mInv == t.mInv;
		}

		__forceinline bool operator!=(const Transform& t){
			return m != t.m || mInv != t.mInv;
		}

		// return a human-readable string summary
		__forceinline std::string ToString() const {
			std::string ret;
			ret += "Transform[\n  m = " + indent(m.ToString())
				+ ",\n  mInv = " + indent(mInv.ToString())
				+ "\n]";
			
			return ret;
		}
	};

	//| 1  0  0  tx |               | 1  0  0  -tx |
	//| 0  1  0  ty |   inverse     | 0  1  0  -ty |
	//| 0  0  1  tz | ----------->> | 0  0  1  -tz |
	//| 0  0  0   1 |               | 0  0  0    1 |
	__forceinline Transform Translate(const Vector3f& t){
		Float x = t.X(); Float y = t.Y(); Float z = t.Z();
		Matrix4 m(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
		Matrix4 mInv(1, 0, 0, -x, 0, 1, 0, -y, 0, 0, 1, -z, 0, 0, 0, 1);

		return Transform(m, mInv);
	}

	//  If the bottom row and rightmost column are deleted from a 4 × 4 matrix, a 3 × 3
	//  matrix is obtained.For every 3 × 3 rotation matrix, R, that rotates φ radians around
	//	any axis, the trace(which is the sum of the diagonal elements in a matrix) is constant
	//	independent of the axis, and is computed as: 
	//                               tr(R) = 1 + 2 cos φ.
	
	//  looking from the positive X-axis to the negative, the couterclockwise direction is the rotation direction
	//| 1         0         0         0 |               | 1         0         0         0 |
	//| 0       cos(t)    -sin(t)     0 |    inverse    | 0      cos(-t)   -sin(-t)     0 |
	//| 0       sin(t)     cos(t)     0 |  ---------->> | 0      sin(-t)    cos(-t)     0 |
	//| 0         0         0         1 |               | 0         0         0         1 |
	__forceinline Transform RotateX(Float theta){
		Float radians = Radians(theta);
		Float sint = sin(radians);
		Float cost = cos(radians);
		Float isint = sin(-radians);
		Float icost = cos(-radians);

		Matrix4 m(1, 0, 0, 0, 0, cost, -sint, 0, 0, sint, cost, 0, 0, 0, 0, 1);
		Matrix4 mInv(1, 0, 0, 0, 0, icost, -isint, 0, 0, isint, icost, 0, 0, 0, 0, 1);

		return Transform(m, mInv);
	}

	//  looking from the positive Y-axis to the negative, the couterclockwise direction is the rotation direction
	//| cos(t)     0       sin(t)     0 |               | cos(-t)     0       sin(-t)     0 |
	//| 0          1        0         0 |    inverse    | 0           1        0          0 | 
	//| -sin(t)    0       cos(t)     0 |  ---------->> | -sin(-t)    0       cos(-t)     0 |
	//| 0          0        0         1 |               | 0           0         0         1 |
	__forceinline Transform RotateY(Float theta){
		Float radians = Radians(theta);
		Float sint = sin(radians);
		Float cost = cos(radians);
		Float isint = sin(-radians);
		Float icost = cos(-radians);

		Matrix4 m(cost, 0, sint, 0, 0, 1, 0, 0, -sint, 0, cost, 0, 0, 0, 0, 1);
		Matrix4 mInv(icost, 0, isint, 0, 0, 1, 0, 0, -isint, 0, icost, 0, 0, 0, 0, 1);

		return Transform(m, mInv);
	}

	//  looking from the positive Z-axis to the negative, the couterclockwise direction is the rotation direction
	//| cos(t)   -sin(t)     0         0 |               | cos(-t)   -sin(-t)     0      0 |
	//| sin(t)   cos(t)      0         0 |    inverse    | sin(-t)    cos(-t)     0      0 | 
	//| 0          0         1         0 |  ---------->> | 0           0          1      0 |
	//| 0          0         0         1 |               | 0           0          0      1 |
	__forceinline Transform RotateZ(Float theta){
		Float radians = Radians(theta);
		Float sint = sin(radians);
		Float cost = cos(radians);
		Float isint = sin(-radians);
		Float icost = cos(-radians);
		
		Matrix4 m(cost, -sint, 0, 0, sint, cost, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		Matrix4 mInv(icost, -isint, 0, 0, isint, icost, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

		return Transform(m, mInv);
	}

	//parameter AXIS must normalized
	//| cos(p)+(1-cos(p))(rx)2           (1-cos(p))(rx)(ry)-(rz)sin(p)      (1-cos(p))(rx)(rz)+(ry)sin(p) |
	//| (1-cos(p))(rx)(ry)+(rz)sin(p)    cos(p)+(1-cos(p)(ry)2              (1-cos(p))(ry)(rz)-(rx)sin(p) |
	//| (1-cos(p))(rx)(rz)-(ry)sin(p)    (1-cos(p))(ry)(rz)+(rx)sin(p)      cos(p)+(1-cos(p))(rz)2        |  
	__forceinline Transform Rotate(Float theta, const Vector3f& axis){
		Float cosp = cos(Radians(theta));
		Float sinp = sin(Radians(theta));
		Float oneMinusCosp = 1.0 - cosp;
		Float x = axis.X(); Float y = axis.Y(); Float z = axis.Z();

		Matrix4 m(cosp + oneMinusCosp*x*x, oneMinusCosp*x*y-z*sinp,oneMinusCosp*x*z+y*sinp,0,
				  oneMinusCosp*x*y+z*sinp,cosp+oneMinusCosp*y*y,oneMinusCosp*y*z-x*sinp,0,
				  oneMinusCosp*x*z-y*sinp,oneMinusCosp*y*z+x*sinp,cosp+oneMinusCosp*z*z,0,
				  0, 0, 0, 1);
		Matrix4 mInv(m.Transpose());

		return Transform(m, mInv);
	}

	//| x  0  0  0 |              | 1/x   0    0    0 |
	//| 0  y  0  0 |   inverse    |  0   1/y   0    0 |
	//| 0  0  z  0 |  --------->> |  0    0   1/z   0 |
	//| 0  0  0  1 |              |  0    0    0    1 |
	__forceinline Transform Scale(const Vector3f& s){
		Float x = s.X(); Float y = s.Y(); Float z = s.Z();
		Matrix4 m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
		Matrix4 mInv(1.0 / x, 0, 0, 0, 0, 1.0 / y, 0, 0, 0, 0, 1.0 / z, 0, 0, 0, 0, 1);

		return Transform(m, mInv);
	}

	//E = Rz(r)Rx(p)Ry(h)
	//| cos(r)cos(h)-sin(r)sin(p)sin(h)     -sin(r)cos(p)   cos(r)sin(h)+sin(r)sin(p)cos(h) |
	//| sin(r)cos(h)+cos(r)sin(p)sin(h)      cos(r)cos(p)   sin(r)sin(h)-cos(r)sin(p)cos(h) |
	//| -cos(p)sin(h)                        sin(p)         cos(p)cos(h)                    |
	__forceinline Transform EulerZXY(const Vector3f& e){
		Float p = Radians(e.X());
		Float h = Radians(e.Y());
		Float r = Radians(e.Z());
		Float cosp = cos(p), sinp = sin(p);
		Float cosh = cos(h), sinh = sin(h);
		Float cosr = cos(r), sinr = sin(r);

		Matrix4 m(cosr*cosh - sinr*sinp*sinh, -sinr*cosp, cosr*sinh + sinr*sinp*cosh, 0,
				  sinr*cosh + cosr*sinp*sinh, cosr*cosp, sinr*sinh - cosr*sinp*cosh, 0,
				  -cosp*sinh, sinp, cosp*cosh, 0,
				  0, 0, 0, 1);
		Matrix4 mInv(m.Transpose());

		return Transform(m, mInv);
	}

	//construct transform from t,r,s
	__forceinline Transform TRS(const Vector3f& translate, const Vector3f& rotation, const Vector3f& scale){
		Transform t = Translate(translate);
		Transform r = EulerZXY(rotation);
		Transform s = Scale(scale);

		Transform ret(t.m*r.m*s.m, s.mInv*r.mInv*t.mInv);
		
		return ret;
	}

	//construct transform from t,r,s
	__forceinline Transform TRS(const Vector3f& translate, const Vector3f& axis, Float angle, const Vector3f& scale) {
		Transform t = Translate(translate);
		Transform r = Rotate(angle, axis);
		Transform s = Scale(scale);

		Transform ret(t.m * r.m * s.m, s.mInv * r.mInv * t.mInv);

		return ret;
	}

	//construct a coordinate system
	__forceinline Transform Lookat(const Vector3f& eye, const Vector3f& center, const Vector3f& up){
		//|a*b| = |a||b|*sin(t)
		Vector3f f = Normalize(eye - center);
		Vector3f r = Normalize(Cross(up, f));
		//vector u doesn't need normalize, because f and r are
		//all normalized and perpendicular with each other
		Vector3f u = Cross(f, r);
		Vector3f t = -eye;

		Matrix4 m(r.X(), r.Y(), r.Z(), Dot(r, t),
				  u.X(), u.Y(), u.Z(), Dot(u, t),
				  f.X(), f.Y(), f.Z(), Dot(f, t),
				  0,   0,   0,   1);

		return Transform(m, m.Inverse());
	}

	//note that n > f, beacuse we are looking down the negative z-axis at this volume of space.
	//our common sense says that the near value should be a lower number than the far, so internally
	//negate them and n > f
	//
	//| 2/(r - l)   0           0           -(r + l)/(r - l) |
	//| 0           2/(t - b)   0           -(t + b)/(t - b) |
	//| 0           0           2/(n - f)   -(f + n)/(f - n) |
	//| 0           0           0           1                |
	__forceinline Transform Ortho(Float l, Float r, Float b, Float t, Float n, Float f){
		//distance values are proprotional to the z-value
		Matrix4 m(2 / (r - l), 0, 0, -(r + l) / (r - l),
				  0, 2 / (t - b), 0, -(t + b) / (t - b),
				  0, 0, 2 / (n - f), -(f + n) / (f - n),
				  0, 0, 0, 1);
		
		Transform ortho(m);
		
		return ortho;
	}

	//| 2n/(r - l)  0           (r + l)/(r - l)   0            |
	//| 0           2n/(t - b)  (t + b)/(t - b)   0            |
	//| 0           0           -(f + n)/(f - n)  -2fn/(f - n) |
	//| 0           0           -1                0
	__forceinline Transform Perspective(Float l, Float r, Float b, Float t, Float n, Float f){
		//distance values are proprotional to the 1/z
		//to increase the depth precision, a common metod, which we call reversed z,
		//is to store 1.0 - zNDC either with float point depth or integers.
		
		//for standard mappint(i.e., non-reverse z) sparating  the projecting matrix in
		//the transform decreases the error rate, for example, it can be better to  use
		//P(Mp) than Tp, where T = PM.

		//the reason for having zNDC proportional to 1/z is that it makes hardware simpler
		//and compression of depth more successful
		Matrix4 m(2 * n / (r - l), 0, (r + l) / (r - l), 0,
				  0, 2 * n / (t - b), (t + b) / (t - b), 0,
				  0, 0, -(f + n) / (f - n), -2 * f*n / (f - n),
				  0, 0, -1, 0);

		Transform perspective(m);
		
		return perspective;
	}

	//| n/r           0                0                 0            |
	//| 0             n/t              0                 0            |
	//| 0             0                -(f + n)/(f - n)  -2fn/(f - n) |
	//| 0             0                -1                0            |
	__forceinline Transform Perspective(Float fovy, Float aspect, Float near, Float far){
		//distance values are proprotional to the 1/z
		//to increase the depth precision, a common metod, which we call reversed z,
		//is to store 1.0 - zNDC either with float point depth or integers.
		
		//for standard mappint(i.e., non-reverse z) sparating  the projecting matrix in
		//the transform decreases the error rate, for example, it can be better to  use
		//P(Mp) than Tp, where T = PM.

		//the reason for having zNDC proportional to 1/z is that it makes hardware simpler
		//and compression of depth more successful
		Float height = tan(Radians(fovy*0.5)) * near;
		Float width = height * aspect;

		Matrix4 m(near / width, 0, 0, 0,
				  0, near / height, 0, 0,
				  0, 0, -(far + near) / (far - near), -2 * far*near / (far - near),
				  0, 0, -1, 0);

		Transform perspective(m);

		return perspective;
	}

	//solve quadratic equation, return true if there is solution
	//given equation A*t^2 + B*t + C = 0
	//       -B - sqrt(B^2 - 4*A*C)            -B + sqrt(B^2 - 4*A*C)
	// t1 = ------------------------    t2 = -------------------------
	//                 2*A                            2*A
	__forceinline bool SolveQuadratic(Float A, Float B, Float C, Float& t1, Float& t2) {
		Float det = B * B - 4 * A * C;
		//if det less than 0, no solution.
		if (det < 0) return false;

		det = sqrt(det);
		t1 = (-B - det) / (2 * A);
		t2 = (-B + det) / (2 * A);

		//make sure t1 < t2
		if (t1 > t2) std::swap(t1, t2);

		return true;
	}

	//solve linear system 2 by 2
	//| a00  a01 || x1 |   | b0 |
	//|          ||    | = |    |
	//| a10  a11 || x2 |   | b1 |
	__forceinline bool SolveLinearSystem2x2(const Matrix2& A, const Vector2f& B, Float& x1, Float& x2) {
		Float det = A.Determinant();
		if (abs(det) < 1e-10) return false;

		x1 = (A[1][1] * B[0] - A[0][1] * B[1]) / det;
		x2 = (A[0][0] * B[1] - A[1][0] * B[0]) / det;
		if (isnan(x1) || isnan(x2)) return false;

		return true;
	}

	//create a coordinate system from a single vector n, n is up vector
	__forceinline void CoordinateSystem(const Vector3f& n, Vector3f& u, Vector3f& w) {
		Float x = n.X(); Float y = n.Y(); Float z = n.Z();
		if (std::abs(x) > std::abs(y)) {
			float invLen = 1.0f / std::sqrt(x * x + z * z);
			w = Vector3f(z * invLen, 0.0f, -x * invLen);
		}
		else {
			float invLen = 1.0f / std::sqrt(y * y + z * z);
			w = Vector3f(0.0f, z * invLen, -y * invLen);
		}
		u = Cross(w, n);
	}

	//return spherical coordinate from theta and phi parameter
	__forceinline Vector3f SphericalCoordinate(Float theta, Float phi) {
		Float costheta = cos(theta);
		Float sintheta = sin(theta);
		Float cosphi = cos(phi);
		Float sinphi = sin(phi);

		return Vector3f(sintheta * cosphi, costheta, sintheta * sinphi);
	}

	__forceinline Float SphericalTheta(const Vector3f& w) {
		//assume w is normalized
		//w.y may exceed than one due to float point precision
		return acos(Clamp(w.Y(), Float(-1), Float(1)));
	}

	__forceinline Float SphericalPhi(const Vector3f& w) {
		Float phi = atan2(w.Z(), w.X());
		if (phi < 0) phi += TWOPI;
		return phi;
	}
}