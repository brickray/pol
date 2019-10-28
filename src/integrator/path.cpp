#include "path.h"
#include "../core/scene.h"

namespace pol {
	Path::Path(int maxDepth, int rrDepth)
		:maxDepth(maxDepth == -1 ? 65536 : maxDepth)
		, rrDepth(rrDepth) {

	}

	//path integrator is aimed to solve equation 
	//    Li = Le + ¡ÒFr*Li*cos(t)*dw
	//Le is direct illumination from light
	Vector3f Path::Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const {
		Vector3f L(0);
		Vector3f beta(1);
		Ray r = ray;
		Intersection isect;
		bool specular = false;

		for (int bounces = 0; bounces < maxDepth; ++bounces) {
			bool found = scene.Intersect(r, isect);
			if (!found) {
				//background
				break;
			}

			if (bounces == 0) {
				//prepare differentials 
				isect.ComputeDifferentials(ray);
			}
			else {
				isect.dudx = isect.dvdx = 0;
				isect.dudy = isect.dvdy = 0;
			}

			Vector3f in = -r.d;
			Vector3f localIn = isect.shFrame.ToLocal(in);
			Vector3f p = isect.p;
			Vector3f n = isect.n;
			Vector2f uv = isect.uv;
			Bsdf* bsdf = isect.bsdf;

			if ((specular || bounces == 0) && isect.light) {
				//intersect with light?
				return beta * isect.light->Le(in, n);
			}

			//count direct lighting
			if (!bsdf->IsDelta()) {
				Vector3f Ld(0);
				//sample light
				int lightIdx = scene.LightLookup(sampler->Next1D());
				Float choicePdf = scene.LightPdf(lightIdx);
				Light* light = scene.GetLight(lightIdx);

				Vector3f radiance;
				Float lightPdf;
				Ray shadowRay;
				light->SampleLight(isect, in, sampler->Next2D(), radiance, lightPdf, shadowRay);
				if (lightPdf != 0 && !scene.Occluded(shadowRay)) {
					Vector3f fr;
					Float bsdfPdf;
					Vector3f localOut = isect.shFrame.ToLocal(shadowRay.d);
					bsdf->Fr(isect, localIn, localOut, fr, bsdfPdf);
					if (!bsdfPdf) return L;

					Float weight = 1;
					if (!light->IsDelta())
						weight = PowerHeuristic(1, lightPdf, 1, bsdfPdf);

					Ld += weight * fr * radiance * fabs(isect.shFrame.CosTheta(localOut)) / (choicePdf * lightPdf);
				}

				//sample bsdf
				Vector3f out;
				Vector3f fr;
				Float bsdfPdf;
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
						Ld += weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
					}
				}

				L += beta * Ld;
			}

			//count indirect lighting
			Vector3f out;
			Vector3f fr;
			Float bsdfPdf;
			bsdf->SampleBsdf(isect, localIn, sampler->Next2D(), out, fr, bsdfPdf);
			if (bsdfPdf == 0) break;

			Float costheta = isect.shFrame.CosTheta(out);
			//transform out direction from local coordinate to world coordinate
			out = isect.shFrame.ToWorld(out);
			beta *= fr * fabs(costheta) / bsdfPdf;
			specular = bsdf->IsDelta();

			//russian roulette
			//   e = E[f(x)],   e is expect value of f(x)
			//   { y = (f(x) - pc) / (1 - p),   u > p,  where u is uniform random number,
			//   { y = c,                       otherwise,  p is some threshold
			//   
			//   E[y] = (1 - p)*E[(f(x) - pc) / (1 - p)] + p*(E[c])
			//   E[y] = E[f(x)] - pc + pc = E[f(x)]
			if (bounces > rrDepth) {
				Float luminance = Clamp(1 - GetLuminance(beta), Float(0), Float(1));
				if (sampler->Next1D() < luminance) break;
				beta /= (1 - luminance);
			}

			r = Ray(p, out);
		}

		return L;
	}

	string Path::ToString() const {
		string ret;
		ret += "Path[\n  maxDepth = " + to_string(maxDepth)
			+ ",\n  rrDepth = " + to_string(rrDepth)
			+ "\n]";

		return ret;
	}

	Path* CreatePathIntegrator(int maxDepth, int rrDepth) {
		return new Path(maxDepth, rrDepth);
	}
}