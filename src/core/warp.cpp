#include "warp.h"

namespace pol {
	//pw integrates on the entire sphere
	//since pw is constant, so pw = 1/(4*pi)
	//p(theta) = sin(theta)/2, p(phi) = 1/(2*pi)
	Vector3f Warp::UniformSphere(const Vector2f& u) {
		//pw = p(theta)*p(phi)/sin(theta)
		Float theta = acos(1.0 - 2.0 * u.x);
		Float phi = TWOPI * u.y;

		return SphericalCoordinate(theta, phi);
	}

	//pw is constant
	Float Warp::UniformSpherePdf(const Vector3f& d) {
		return INV4PI;
	}

	//pw integrates on the hemisphere, n = (0, 1, 0)
	//since pw is constant, so pw = 1/(2*pi)
	//p(theta) = sin(theta), p(phi) = 1/(2*pi)
	Vector3f Warp::UniformHemiSphere(const Vector2f& u) {
		Float theta = acos(1.0 - u.x);
		Float phi = TWOPI * u.y;

		return SphericalCoordinate(theta, phi);
	}

	Float Warp::UniformHemiSpherePdf(const Vector3f& d) {
		return INV2PI;
	}

	//pw = c*cos(theta) where c is constant
	//integrates pw on the hemisphere, so pw = cos(theta)/pi
	//p(theta) = 2*cos(theta)*sin(theta), p(phi) = 1/(2*pi)
	Vector3f Warp::CosineHemiSphere(const Vector2f& u) {
		Float theta = asin(sqrtf(u.x));
		Float phi = TWOPI * u.y;

		return SphericalCoordinate(theta, phi);
	}

	//assume d is in local coordinante
	Float Warp::CosineHemiSpherePdf(const Vector3f& d) {
		return d.Y() * INVPI;
	}

	//integrate pw from -theta to theta
	//since pw is constant, pw = 1/(2*pi*(1-cos(thetamax))
	Vector3f Warp::UniformCone(const Vector2f& u, Float costhetaMax) {
		Float theta = acos(1.0 - u.x * (1.0 - costhetaMax));
		Float phi = TWOPI * u.y;

		return SphericalCoordinate(theta, phi);
	}

	//assume d is in local coordinante
	// pw = 1/(2*pi*(1-cos(thetamax))
	Float Warp::UniformConePdf(Float costhetaMax) {
		return 1.0 / (TWOPI * (1.0 - costhetaMax));
	}

	//pw integrates on the entire disk
	//since pw is constant, so pw = 1/pi
	//p(r) = 2*r, p(phi) = 1/(2*pi)
	Vector2f Warp::UniformDisk(const Vector2f& u) {
		Float r = sqrtf(u.x);
		Float phi = TWOPI * u.y;

		return Vector2f(r * cos(phi), r * sin(phi));
	}

	Float Warp::UniformDiskPdf(const Vector2f& p) {
		return INVPI;
	}

	//better than above
	//mapping from (x,y) to (r, theta) by
	//  r     =     x
	//            y   pi
	//  theta =  --- ---
	//            x   4
	//we can easily prove the sample points are uniformly on the disk
	Vector2f Warp::ConcentricDisk(const Vector2f& u) {
		Vector2f offset = Float(2) * u - Vector2f(1.0);
		if (offset.x == 0 && offset.y == 0)
			return Vector2f();

		Float r, phi;
		if (abs(offset.x) > abs(offset.y)) {
			r = offset.x;
			phi = PIOVER4 * (offset.y / offset.x);
		}
		else {
			r = offset.y;
			phi = PIOVER2 - PIOVER4 * (offset.x / offset.y);
		}

		return Vector2f(r * cos(phi), r * sin(phi));
	}

	Float Warp::ConcentricDiskPdf(const Vector2f& p) {
		return INVPI;
	}

	//return u and v
	//the pdf is one over triangle area
	Vector2f Warp::UniformTriangle(const Vector2f& u) {
		Float su0 = sqrtf(u.x);

		return Vector2f(1.0 - su0, u.y * su0);
	}
}