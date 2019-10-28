#include "ao.h"
#include "../core/scene.h"

namespace pol {
	Ao::Ao(Float maxDist)
		:maxDist(maxDist) {

	}

	//ao integrator is aimed to solve equation 
	//         Li = 1/pi¡ÒV(p)cos(t)dw
	//where V(p) is visibility function.
	//V(p) = 1 if visible and 0 otherwise
	Vector3f Ao::Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const {
		Vector3f L(0);

		Ray r = ray;
		Intersection isect;
		//test if intersect, return 0 if invisible
		bool intersect = scene.Intersect(r, isect);
		if (!intersect) return L;

		Vector3f p = isect.p;
		Vector3f n = isect.n;

		//generate out direction randomly
		Vector3f out = Warp::CosineHemiSphere(sampler->Next2D());
		//ensure out direction and ray direction are in the same hemisphere
		if (!SameHemiSphere(-ray.d, n))
			out = -out;
		//transform out direction from local coordinate to world coordinate
		out = isect.shFrame.ToWorld(out);
		r = Ray(p, out, Epsilon, maxDist);
		if (!scene.Occluded(r)) {
			//eval equation
			L = Vector3f(1.0);
		}

		return L;
	}

	string Ao::ToString() const {
		string ret;
		ret += "Ao[\n  maxDist = " + to_string(maxDist) + "\n]";

		return ret;
	}

	Ao* CreateAoIntegrator(Float maxDist) {
		return new Ao(maxDist);
	}
}