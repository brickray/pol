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

			Vector3f in = -r.d;
			Vector3f localIn = isect.shFrame.ToLocal(in);
			Vector3f p = isect.p;
			Vector3f n = isect.n;
			Vector2f uv = isect.uv;
			Bsdf* bsdf = isect.bsdf;

			if (specular || bounces == 0) {
				if (found) {
					//intersect with light?
					if (isect.light) {
						L += beta * isect.light->Le(in, n);
						break;
					}
				}
				else {
					//background
					Light* light = scene.GetInfiniteLight();
					if(light) L += beta * light->Le(in, Vector3f::zero);
					break;
				}
			}

			//break if there is no intersection found
			if (!found) break;

			if (bounces == 0) {
				//prepare differentials 
				isect.ComputeDifferentials(ray);
			}
			else {
				//only primiary ray using differentials information
				//so here set all differentials to 0
				isect.dudx = isect.dvdx = 0;
				isect.dudy = isect.dvdy = 0;
			}

			//count direct lighting
			if (!bsdf->IsDelta()) {
				Vector3f Ld(0);
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

						Ld += weight * fr * radiance * Frame::AbsCosTheta(localOut) / lightPdf;
					}
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
					//here is always area light
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
						Ld += weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
					}
				}
				else if (scene.GetInfiniteLight()) {
					Light* light = scene.GetInfiniteLight();
					Vector3f radiance = light->Le(-out, Vector3f::zero);
					Float lightPdf = light->Pdf(p + out, p);
					lightPdf *= lightDistribution->DiscretePdf(scene.GetLightIndex(light));
					Float weight = PowerHeuristic(1, bsdfPdf, 1, lightPdf);
					Ld += weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
				}

				L += beta * Ld;
			}

			//count indirect lighting
			Vector3f out;
			Vector3f fr;
			Float bsdfPdf;
			bsdf->SampleBsdf(isect, localIn, sampler->Next2D(), out, fr, bsdfPdf);
			if (bsdfPdf == 0) break;

			Float costheta = Frame::AbsCosTheta(out);
			//transform out direction from local coordinate to world coordinate
			out = isect.shFrame.ToWorld(out);
			beta *= fr * costheta / bsdfPdf;
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