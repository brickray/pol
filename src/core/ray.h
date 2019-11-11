#pragma once

#include "../pol.h"

namespace pol {
	//-------------------------------------------------------------
	//                       RayDifferential
	//-------------------------------------------------------------
	class RayDifferential : public Ray {
	public:
		Vector3f rxOrigin, ryOrigin;
		Vector3f rxDirection, ryDirection;

	public:
		RayDifferential() {

		}

		RayDifferential(const Vector3f& o, const Vector3f& d, Float tmin = Epsilon, Float tmax = INFINITY)
			:Ray(o, d, tmin, tmax) {

		}

		RayDifferential(const Ray& ray)
			:Ray(ray) {

		}

		string ToString() const {
			string ret;
			/*ret += "RayDifferential[\n  " + indent(Ray::ToString())
				+ ",\n  xo = " + rxOrigin.ToString()
				+ ",\n  xd = " + rxDirection.ToString()
				+ ",\n  yo = " + ryOrigin.ToString()
				+ ",\n  yd = " + ryDirection.ToString()
				+ "\n]";*/

			return ret;
		}
	};
}