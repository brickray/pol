#pragma once

#include "../pol.h"

namespace pol {
	class Warp {
	public:
		//in order to draw a sample from given distribution, we first
		//find the CDF P(x). In the continuous case, P is the indefinite
		//integral of p. In the discrete case, we can directly construct 
		//the CDF by stacking the bars on top of eaching other.
		//we then take a uniform random number u and use it to select one of
		//the possible outcomes using the CDF, doing so in a way that chooses
		//a particular outcome with probability equal to the outcome's own probability.
		//summary is:
		//1. compute the CDF p(x)
		//2. compute the inverse p(x)^-1
		//3. Obtain a uniformly distributed random number u
		//4. compute xi = p(u)^-1
		//
		//dw = sint(theta)dtdp
		//

		static Vector3f UniformSphere(const Vector2f& u);
		//return pdf of UniformSphere() in solid angle
		static Float UniformSpherePdf(const Vector3f& d);
		static Vector3f UniformHemiSphere(const Vector2f& u);
		//return pdf of UniformHemiSphere() in solid angle
		static Float UniformHemiSpherePdf(const Vector3f& d);
		static Vector3f CosineHemiSphere(const Vector2f& u);
		//return pdf of CosineHemiSphere() in solid angle
		//assume d is in local coordinante
		static Float CosineHemiSpherePdf(const Vector3f& d);
		static Vector3f UniformCone(const Vector2f& u, Float costhetaMax);
		//return pdf of UniformCone() in solid angle
		static Float UniformConePdf(Float costhetaMax);
		static Vector2f UniformDisk(const Vector2f& u);
		//return pdf of UniformDisk() in solid angle
		static Float UniformDiskPdf(const Vector2f& p);
		static Vector2f ConcentricDisk(const Vector2f& u);
		//return pdf of ConcentricDisk() in solid angle
		static Float ConcentricDiskPdf(const Vector2f& p);
		static Vector2f UniformTriangle(const Vector2f& u);

		static Vector2f GaussianDisk(const Vector2f& u, Float falloff);
		static Float GaussianDiskPdf(const Vector3f& center, const Vector3f& sample, const Vector3f& n, Float falloff);
		static Float GaussianDiskPdf(Float x, Float y, Float falloff);
		static Vector2f GaussianDisk(const Vector2f& u, Float falloff, Float rMax);
		static Float GaussianDiskPdf(const Vector3f& center, const Vector3f& sample, const Vector3f& n, Float falloff, Float rMax);
		static Float GaussianDiskPdf(Float x, Float y, Float falloff, Float rMax);
		static Float ExponentialDistance(Float u, Float falloff);
		static Float ExponentialDistancePdf(Float x, Float falloff);
	};
}
