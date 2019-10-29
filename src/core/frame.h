#pragma once

#include "../pol.h"

namespace pol {
	//coordinate system
	class Frame {
	private:
		Vector3f u, v, w;

	public:
		Frame() {

		}

		Frame(const Vector3f& n)
			:v(n) {
			CoordinateSystem(v, u, w);
		}

		Frame(const Vector3f& u, const Vector3f& v, const Vector3f& w)
			:u(u), v(v), w(w) {

		}

		//convert from world coordinate to local coordinate
		__forceinline Vector3f ToLocal(const Vector3f& d) const {
			return Vector3f(Dot(u, d), Dot(v, d), Dot(w, d));
		}

		//convert from local coordinate to world coordinate
		__forceinline Vector3f ToWorld(const Vector3f& d) const {
			return u * d.x + v * d.y + w * d.z;
		}

		//assume vector is in local coordinate
		//return cosine of the angle between vector and normal
		static __forceinline Float CosTheta(const Vector3f& d) {
			return d.y;
		}

		//assume vector is in local coordinate
		//return sine of the angle between vector and normal
		static __forceinline Float SinTheta(const Vector3f& d) {
			//the range of theta between 0 and pi
			//so sine of the theta are always great than 0
			return sqrtf(SinTheta2(d));
		}

		//assume vector is in local coordinate
		//return tangent of the angle between vector and normal
		static __forceinline Float TanTheta(const Vector3f& d) {
			return SinTheta(d) / CosTheta(d);
		}

		//assume vector is in local coordinate
		//return square of cosine of the angle between vector and normal
		static __forceinline Float CosTheta2(const Vector3f& d) {
			return d.y * d.y;
		}

		//assume vector is in local coordinate
		//return square of sine of the angle between vector and normal
		static __forceinline Float SinTheta2(const Vector3f& d) {
			return 1.0 - CosTheta2(d);
		}

		//assume vector is in local coordinate
		//return square of tangent of the angle between vector and normal
		static __forceinline Float TanTheta2(const Vector3f& d) {
			return SinTheta2(d) / CosTheta2(d);
		}

		//assume vector is in local coordinate
		//return cosine of the phi parameter in spherical coordinates
		static __forceinline Float CosPhi(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.x, 0, d.z));
			return t.x;
		}

		//assume vector is in local coordinate
		//return sine of the phi parameter in spherical coordinates
		static __forceinline Float SinPhi(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.x, 0, d.z));
			return t.z;
		}

		//assume vector is in local coordinate
		//return tangent of the phi parameter in spherical coordinates
		static __forceinline Float TanPhi(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.x, 0, d.z));
			return t.z / t.x;
		}

		//assume vector is in local coordinate
		//return square of cosine of the phi parameter in spherical coordinates
		static __forceinline Float CosPhi2(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.x, 0, d.z));
			return t.x * t.x;
		}

		//assume vector is in local coordinate
		//return square of sine of the phi parameter in spherical coordinates
		static __forceinline Float SinPhi2(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.x, 0, d.z));
			return t.z * t.z;
		}

		//assume vector is in local coordinate
		//return square of tangent of the phi parameter in spherical coordinates
		static __forceinline Float TanPhi2(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.x, 0, d.z));
			return t.z * t.z / (t.x * t.x);
		}

		string ToString() const {
			string ret;
			ret = "Frame[\n"
				   "  u = " + u.ToString() + ",\n"
				   "  v = " + v.ToString() + ",\n"
				   "  w = " + w.ToString() + "\n"
				   "]";

			return ret;
		}
	};
}
