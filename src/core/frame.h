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
			return u * d.X() + v * d.Y() + w * d.Z();
		}

		//assume vector is in local coordinate
		//return cosine of the angle between vector and normal
		static __forceinline Float CosTheta(const Vector3f& d) {
			return d.Y();
		}

		//assume vector is in local coordinate
		//return absolute cosine of the angle between vector and normal
		static __forceinline Float AbsCosTheta(const Vector3f& d) {
			return fabs(CosTheta(d));
		}

		//assume vector is in local coordinate
		//return sine of the angle between vector and normal
		static __forceinline Float SinTheta(const Vector3f& d) {
			//the range of theta between 0 and pi
			//so sine of the theta are always great than 0
			return sqrtf(SinTheta2(d));
		}

		//assume vector is in local coordinate
		//return absolute sine of the angle between vector and normal
		static __forceinline Float AbsSinTheta(const Vector3f& d) {
			return fabs(SinTheta(d));
		}

		//assume vector is in local coordinate
		//return tangent of the angle between vector and normal
		static __forceinline Float TanTheta(const Vector3f& d) {
			return SinTheta(d) / CosTheta(d);
		}

		//assume vector is in local coordinate
		//return absolute tangent of the angle between vector and normal
		static __forceinline Float AbsTanTheta(const Vector3f& d) {
			return fabs(TanTheta(d));
		}

		//assume vector is in local coordinate
		//return square of cosine of the angle between vector and normal
		static __forceinline Float CosTheta2(const Vector3f& d) {
			return d.Y() * d.Y();
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
			Vector3f t = Normalize(Vector3f(d.X(), 0, d.Z()));
			return t.X();
		}

		//assume vector is in local coordinate
		//return absolute cosine of the phi parameter in spherical coordinates
		static __forceinline Float AbsCosPhi(const Vector3f& d) {
			return fabs(CosPhi(d));
		}

		//assume vector is in local coordinate
		//return sine of the phi parameter in spherical coordinates
		static __forceinline Float SinPhi(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.X(), 0, d.Z()));
			return t.Z();
		}

		//assume vector is in local coordinate
		//return absolute sine of the phi parameter in spherical coordinates
		static __forceinline Float AbsSinPhi(const Vector3f& d) {
			return fabs(SinPhi(d));
		}

		//assume vector is in local coordinate
		//return tangent of the phi parameter in spherical coordinates
		static __forceinline Float TanPhi(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.X(), 0, d.Z()));
			return t.Z() / t.X();
		}

		//assume vector is in local coordinate
		//return absolute tangent of the phi parameter in spherical coordinates
		static __forceinline Float AbsTanPhi(const Vector3f& d) {
			return fabs(TanPhi(d));
		}

		//assume vector is in local coordinate
		//return square of cosine of the phi parameter in spherical coordinates
		static __forceinline Float CosPhi2(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.X(), 0, d.Z()));
			return t.X() * t.X();
		}

		//assume vector is in local coordinate
		//return square of sine of the phi parameter in spherical coordinates
		static __forceinline Float SinPhi2(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.X(), 0, d.Z()));
			return t.Z() * t.Z();
		}

		//assume vector is in local coordinate
		//return square of tangent of the phi parameter in spherical coordinates
		static __forceinline Float TanPhi2(const Vector3f& d) {
			Vector3f t = Normalize(Vector3f(d.X(), 0, d.Z()));
			return t.Z() * t.Z() / (t.X() * t.X());
		}

		string ToString() const {
			string ret;
			/*ret = "Frame[\n"
				   "  u = " + u.ToString() + ",\n"
				   "  v = " + v.ToString() + ",\n"
				   "  w = " + w.ToString() + "\n"
				   "]";*/

			return ret;
		}
	};
}
