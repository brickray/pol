#include "ltrace.h"
#include "../core/scene.h"
#include "../core/parallel.h"

namespace pol {
	POL_REGISTER_CLASS(LTrace, "ltrace");

	LTrace::LTrace(const PropSets& props, Scene& scene)
		:Integrator(props, scene) {
		maxDepth = props.GetInt("maxDepth", 65536);
		if (maxDepth == -1) maxDepth = 65536;
		rrDepth = props.GetInt("rrDepth", 3);
	}

	Vector3f LTrace::Li(const RayDifferential& ray, const Scene& scene, const Sampler* sampler) const {
		return 0;
	}

	//trace ray from light to camera
	//    We = dS/d(phi)
	// S is the respond of sensor
	//    S = กากาWe*Li*cos(t)*dA*dw
	//      = กากาWe*(Le+กาFr*Li*cos(t)*dw)*cos(t)*dA*dw
	//      = กากาWe*Le*cos(t)*dA*dw + กากากาWe*Fr*Le*cos(t1)*cos(t2)*dA0*dw1*dw2 + ...
	// S(n) = กา...กาWe*Fr1*G1*Fr2*G2...*Frn*Gn*Le*dA0*dA1*dA2*...*dAn
	// for case p0->p1->p2->p3
	// S(4) = กากากากาWe*Fr1*Fr2*Le*cos(t2)*cos(t1)*cos(t)*dA0*dw*dw1*dw2
	//      = กากากากาWe*G<p0,p1>*Fr<p1>*G<p1,p2>*Fr<p2>*G<p2,p3>*Le*dA0*dA1*dA2*dA3
	//Le is direct illumination from light
	void LTrace::Render(const Scene& scene) const {
		Camera* camera = scene.GetCamera();
		Film* film = camera->GetFilm();
		const Sampler* sampler = scene.GetSampler();
		const Distribution1D* lightDistribution = scene.LightLookup(Vector3f::Zero());
		int nSamples = sampler->GetSampleCount();

		vector<RenderBlock> rbs;
		InitRenderBlock(scene, rbs);

		Parallel::ParallelLoop([&](const RenderBlock& rb) {
			Sampler* samplerClone = sampler->Clone();
			int sx = rb.sx, sy = rb.sy;
			int ex = rb.sx + rb.w, ey = rb.sy + rb.h;
			for (int i = sx; i < ex; ++i) {
				for (int j = sy; j < ey; ++j) {
					samplerClone->Prepare(j * film->res.x + i);
					for (int s = 0; s < nSamples; ++s) {
						int lightIndex = lightDistribution->SampleDiscrete(samplerClone->Next1D());
						Float choicePdf = lightDistribution->DiscretePdf(lightIndex);
						Light* light = scene.GetLight(lightIndex);
						Float pdfA, pdfW;
						Vector3f radiance, normal;
						Ray emitRay;
						light->SampleLight(samplerClone->Next2D(), samplerClone->Next2D(), radiance, normal, emitRay, pdfW, pdfA);
						Vector3f beta = radiance * fabs(Dot(normal, emitRay.d)) / (pdfA * pdfW * choicePdf);

						Ray shadowRay;
						Vector3f we;
						Float cameraPdf;
						Vector2i pRaster;
						camera->SampleWe(emitRay.o, we, shadowRay, cameraPdf, pRaster);
						if (cameraPdf != 0.f && !scene.Occluded(shadowRay)) {
							film->AddSample(pRaster, radiance);
						}

						Intersection isect;
						Ray ray = emitRay;
						for (int bounces = 0; bounces < maxDepth; ++bounces) {
							if (!scene.Intersect(ray, isect)) break;

							Vector3f pos = isect.p;
							Bsdf* bsdf = isect.bsdf;
							Vector3f localIn = isect.shFrame.ToLocal(-ray.d);

							if (!bsdf->IsDelta()) {
								Vector3f we;
								Vector2i pRaster;
								Ray shadowRay;
								Float cameraPdf;
								camera->SampleWe(pos, we, shadowRay, cameraPdf, pRaster);
								if (cameraPdf != 0 && !scene.Occluded(shadowRay)) {
									Vector3f fr;
									Float scatterPdf;
									Vector3f localOut = isect.shFrame.ToLocal(-shadowRay.d);
									bsdf->Fr(isect, localIn, localOut, fr, scatterPdf);
									if (scatterPdf != 0) {
										Vector3f L = we * fr * beta / cameraPdf;
										film->AddSample(pRaster, L);
									}
								}
							}

							Vector3f out, fr;
							Float pdf;
							bsdf->SampleBsdf(isect, localIn, samplerClone->Next2D(), out, fr, pdf);
							if (pdf == 0) break;
							beta *= fr / pdf;

							out = isect.shFrame.ToWorld(out);

							ray = Ray(pos, out);

							if (bounces > rrDepth) {
								Float luminance = Clamp(1 - GetLuminance(beta), Float(0), Float(1));
								if (sampler->Next1D() < luminance) break;
								beta /= (1 - luminance);
							}
						}
					}
				}
			}

			}, rbs);
	}

	string LTrace::ToString() const {
		string ret;
		ret += "Light Tracing[\n  maxDepth = " + to_string(maxDepth)
			+ ",\n  rrDepth = " + to_string(rrDepth)
			+ "\n]";

		return ret;
	}
}