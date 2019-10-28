#pragma once

#include "object.h"
#include "warp.h"
#include "ray.h"

namespace pol {
	//monte carlo method
	//      1   N    f(xi)
	// FN = --- ��  -------  = ��f(x)dx
	//      N   i=1  p(xi)
	//can easily be proved by the law of large number
	//here we use another method
	//           1  N
	//  E[FN] = --- �� ��f(x)/p(x) * p(x)dx
	//			 N  i=1
	//           1  N
	//        = --- �� ��f(x)dx
	//           N  I=1
	//        =��f(x)dx
	//
	//Convergence rates of monte carlo method
	//let Yi = f(Xi)/p(Xi), so that
	//      FN = 1/N ��Yi from i = 0 to N
	//variance of FN is
	//    V(FN) = E[1/N^2(��Yi)^2] - u^2, where u is expect value of FN
	//          = 1/N^2 E[(��Yi)^2] - u^2
	//          = 1/N^2(E[��Yi^2] + (N-1)/N*u^2) - u^2
	//          = 1/N E[Yi^2] - 1/N*u^2
	//          = 1/N(��f^2(x)/p(x)dx - u^2)
	//thus the standard deviation is
	//   ��[FN] �� 1/sqrt(N)
	//i.e., the standard deviation is proportional to one over square root of N
	//in other words to halve the error we must quadruple the number of samples
	//
	//Multiple Importance Sampling
	//the multi-sample estimator is given by
	//      F = ��1/(Ni)��(Wi*f(Xi,j)/p(Xi,j))
	//for this estimate to be unbiased, the weighting function Wi must satisfy the
	//following two conditions:
	//1. ��Wi(x) = 1 whenever f(x) != 0, and
	//2. Wi(x) = 0 whenever p(x) = 0
	//now we proof E[F] = ��f(x)dx
	//      E[F] = ��1/(Ni)�ơ�(Wi*f(Xi,j)/p(Xi,j))*p(Xi,j)dx
	//      E[F] = �ҡ�Wi*f(x)dx
	//      E[F] = ��f(x)dx
	class Scene;
	class Sampler;
	class Integrator : public PolObject {
	public:
		Integrator();
		virtual ~Integrator();

		virtual Vector3f Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const = 0;
	};
}
