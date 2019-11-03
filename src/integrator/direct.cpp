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
		bool found = scene.Intersect(r, isect);

		Vector3f in = -r.d;
		Vector3f localIn = isect.shFrame.ToLocal(in);
		Vector3f p = isect.p;
		Vector3f n = isect.n;
		Vector2f uv = isect.uv;
		Bsdf* bsdf = isect.bsdf;

		if (found) {
			if (isect.light) {
				//intersect area light?
				L += isect.light->Le(in, isect.n);
				return L;
			}
		}
		else {
			//background
			Light* light = scene.GetInfiniteLight();
			if (light) L = light->Le(in, Vector3f::zero);
			return L;
		}

		//prepare differentials 
		isect.ComputeDifferentials(ray);

		if (!isect.bsdf->IsDelta()) {
			//sample light
			const Distribution1D* lightDistribution = scene.LightLookup(p);
			int lightIdx = lightDistribution->SampleDiscrete(sampler->Next1D());
			Float choicePdf = lightDistribution->DiscretePdf(lightIdx);
			Light* light = scene.GetLight(lightIdx);

			Vector3f radiance;
			Float lightPdf;
			Ray shadowRay;
			light->SampleLight(isect, sampler->Next2D(), radiance, lightPdf, shadowRay);
			if (lightPdf != 0 && !scene.Occluded(shadowRay)) {
				lightPdf *= choicePdf;

				Vector3f fr;
				Float bsdfPdf;
				Vector3f localOut = isect.shFrame.ToLocal(shadowRay.d);
				//here infinite light can easily make bsdfPdf = 0
				//because it choose point on whole sphere randomly
				bsdf->Fr(isect, localIn, localOut, fr, bsdfPdf);

				if (bsdfPdf != 0) {
					Float weight = 1;
					if (!light->IsDelta())
						weight = PowerHeuristic(1, lightPdf, 1, bsdfPdf);

					L += weight * fr * radiance * Frame::AbsCosTheta(localOut) / lightPdf;
				}
			}

			//sample bsdf
			Vector3f out;
			Vector3f fr;
			Float bsdfPdf;
			bsdf->SampleBsdf(isect, localIn, sampler->Next2D(), out, fr, bsdfPdf);
			//if (!bsdfPdf) return L;
			//the above sentence makes a bug, it should not return when bsdfPdf = 0
			if (bsdfPdf) {
				out = isect.shFrame.ToWorld(out);
				Ray scatterRay(p, out);
				Intersection scatterIsect;
				if (scene.Intersect(scatterRay, scatterIsect)) {
					Light* light = scatterIsect.light;
					Vector3f radiance;
					Float lightPdf = 0;
					if (light) {
						radiance = light->Le(-out, scatterIsect.n);
						lightPdf = light->Pdf(scatterIsect.p, p);
						lightPdf *= lightDistribution->DiscretePdf(scene.GetLightIndex(light));
					}
					if (!IsBlack(radiance)) {
						Float weight = PowerHeuristic(1, bsdfPdf, 1, lightPdf);
						L += weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
					}
				}
				else if (scene.GetInfiniteLight()) {
					Light* light = scene.GetInfiniteLight();
					Vector3f radiance = light->Le(-out, Vector3f::zero);
					Float lightPdf = light->Pdf(p + out, p);
					lightPdf *= lightDistribution->DiscretePdf(scene.GetLightIndex(light));
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