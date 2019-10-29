#include "direct.h"
#include "../core/scene.h"

namespace pol {
	//direct integrator is aimed to solve equation 
	//    Li = Le + ¡ÒFr*Le*cos(t)*dw
	//Le is direct illumination from light
	Vector3f Direct::Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const {
		Vector3f L(0);
		Ray r = ray;

		Intersection isect;
		bool intersect = scene.Intersect(r, isect);
		if (!intersect) {
			//background
			return L;
		}

		//prepare differentials 
		isect.ComputeDifferentials(ray);

		Vector3f in = -r.d;
		Vector3f localIn = isect.shFrame.ToLocal(in);
		Vector3f p = isect.p;
		Vector3f n = isect.n;
		Vector2f uv = isect.uv;
		Bsdf* bsdf = isect.bsdf;

		if (isect.light) {
			//intersect area light?
			return isect.light->Le(in, isect.n);
		}

		if (!isect.bsdf->IsDelta()) {
			//sample light
			int lightIdx = scene.LightLookup(sampler->Next1D());
			Float choicePdf = scene.LightPdf(lightIdx);
			Light* light = scene.GetLight(lightIdx);

			Vector3f radiance;
			Float lightPdf;
			Ray shadowRay;
			light->SampleLight(isect, in, sampler->Next2D(), radiance, lightPdf, shadowRay);
			if (lightPdf == 0 || scene.Occluded(shadowRay)) return L;

			Vector3f fr;
			Float bsdfPdf;
			Vector3f localOut = isect.shFrame.ToLocal(shadowRay.d);
			bsdf->Fr(isect, localIn, localOut, fr, bsdfPdf);
			if (!bsdfPdf) return L;

			Float weight = 1;
			if (!light->IsDelta())
				weight = PowerHeuristic(1, lightPdf, 1, bsdfPdf);

			L += weight * fr * radiance * fabs(Frame::CosTheta(localOut)) / (choicePdf * lightPdf);
		
			//sample bsdf
			Vector3f out;
			bsdf->SampleBsdf(isect, localIn, sampler->Next2D(), out, fr, bsdfPdf);
			if (!bsdfPdf) return L;
			out = isect.shFrame.ToWorld(out);
			Ray scatterRay(p, out);
			Intersection scatterIsect;
			if (scene.Intersect(scatterRay, scatterIsect)) {
				Light* light = scatterIsect.light;
				if (light) {
					Vector3f radiance = light->Le(-out, scatterIsect.n);
					Float lightPdf = light->Pdf(scatterIsect.p, p);
					Float weight = PowerHeuristic(1, bsdfPdf, 1, lightPdf);
					L += weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
				}
			}
		}

		return L;
	}

	string Direct::ToString() const {
		string ret;
		ret += "Direct[\n]";

		return ret;
	}

	Direct* CreateDirectIntegrator() {
		return new Direct();
	}
}