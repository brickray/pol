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
		Vector3f L(0.f);
		Vector3f beta(1);
		Ray r = ray;
		Intersection isect;
		if (scene.Intersect(r, isect)) {
			//intersect with light?
			if (isect.light) {
				return beta * isect.light->Le(-r.d, isect.n);
			}
		}
		else {
			//intersect nothing, maybe infinite light exists
			Light* light = scene.GetInfiniteLight();
			if (light) return beta * light->Le(-r.d, Vector3f::Zero());
		}

		for (int bounces = 0; bounces < maxDepth; ++bounces) {
			Vector3f in = -r.d;
			Vector3f localIn = isect.shFrame.ToLocal(in);
			Vector3f p = isect.p;
			Vector3f n = isect.n;
			Vector2f uv = isect.uv;
			Bsdf* bsdf = isect.bsdf;

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

			const Distribution1D* lightDistribution = scene.LightLookup(p);
			//estimate direct lighting
			if (!bsdf->IsDelta()) {
				//sample light
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
						//delta light has weight 1
						if (!light->IsDelta())
							weight = PowerHeuristic(lightPdf, bsdfPdf);

						L += beta * weight * fr * radiance * Frame::AbsCosTheta(localOut) / lightPdf;
					}
				}
			}

			//bsdf sampling
			Vector3f out, fr;
			Float bsdfPdf;
			bsdf->SampleBsdf(isect, localIn, sampler->Next2D(), out, fr, bsdfPdf);
			if (bsdfPdf == 0) break;

			//prepare for throughput
			Float costheta = Frame::AbsCosTheta(out);
			//transform out direction from local coordinate to world coordinate
			out = isect.shFrame.ToWorld(out);

			//trace a next ray
			r = Ray(p, out);
			//estimate direct light if needed
			if (scene.Intersect(r, isect)) {
				if (isect.light) {
					//hit a light?
					Light* light = isect.light;
					Vector3f radiance;
					Float lightPdf = 0;
					radiance = light->Le(-out, isect.n);
					lightPdf = light->Pdf(isect.p, p);
					lightPdf *= lightDistribution->DiscretePdf(scene.GetLightIndex(light));
					if (!IsBlack(radiance)) {
						Float weight = 1;
						//delta bsdf has weight 1
						if(!bsdf->IsDelta())
						    weight = PowerHeuristic(bsdfPdf, lightPdf);
						L += beta * weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
					}

					break;
				}
			}
			else {
				//if there is infinite light
				Light* light = scene.GetInfiniteLight();
				if (light) {
					Vector3f radiance = light->Le(-out, Vector3f::Zero());
					Float lightPdf = light->Pdf(p + out, p);
					lightPdf *= lightDistribution->DiscretePdf(scene.GetLightIndex(light));
					Float weight = PowerHeuristic(bsdfPdf, lightPdf);
					L += beta * weight * fr * radiance * fabs(Dot(n, out)) / bsdfPdf;
				}

				break;
			}

			//accumulate throughput
			beta *= fr * costheta / bsdfPdf;

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