#include "bssrdf.h"
#include "bsdf.h"
#include "warp.h"
#include "scene.h"

namespace pol {
	JensenBssrdf::JensenBssrdf(const Vector3f& absorb, const Vector3f& scatter, Float eta, Float g)
		:absorb(absorb), scatter(scatter)
		, eta(eta), g(g) {
		Float p = fdr(eta);
		coeff = (1 + p) / (1 - p);
	}

	Vector3f JensenBssrdf::rd(Float d2) const {
		// see Donner. C 2006 Chapter 5 for the full derivation
		// of the following diffusion dipole approximiation equation
		Vector3f sigmaA = absorb;
		Vector3f sigmaS = scatter;
		Vector3f sigmaSPrime = sigmaS * (1 - g);
		Vector3f sigmaTPrime = sigmaA + sigmaSPrime;
		Vector3f albedoPrime = sigmaSPrime / sigmaTPrime;
		Vector3f sigmaTr = Sqrt(3 * sigmaA * sigmaTPrime);
		Vector3f D = 1 / sigmaTPrime; //diffusion constant
		Vector3f zr = 1 / sigmaTPrime; //distance beneath surface where we put the positive dipole light
		Vector3f zv = zr + 4 * coeff * D; //distance above surface where we put the negative dipole light
		Vector3f dr = Sqrt(zr * zr + d2); //distance from x to the positive dipole light
		Vector3f dv = Sqrt(zv * zv + d2); //distance from x to the negative dipole light
		Vector3f trdr = sigmaTr * dr;
		Vector3f trdv = sigmaTr * dv;
		Vector3f rd = albedoPrime * INV4PI * (zr * (trdr + 1) * Exp(-trdr) / (dr * dr * dr) + zv * (trdv + 1) * Exp(-trdv) / (dv * dv * dv));
		return rd;
	}

	//Fdr = ¡ÒFr(eta, n.w)*(n.w)*dw
	Float JensenBssrdf::fdr(Float eta) const {
		// see Donner. C 2006 Chapter 5
		// the internal Fresnel reflectivity
		// approximated with a simple polynomial expansion
		if (eta < 1) {
			return -0.4399 + 0.7099 / eta - 0.3319 / (eta * eta) +
				0.0636 / (eta * eta * eta);
		}
		else {
			return -1.4399 / (eta * eta) + 0.7099 / eta + 0.6681 +
				0.0636 * eta;
		}
	}

	Vector3f JensenBssrdf::getSigmaTr() const {
		Vector3f sigmaSPrime = scatter * (1 - g);
		Vector3f sigmaTPrime = absorb + sigmaSPrime;
		Vector3f sigmaTr = Sqrt(3 * absorb * sigmaTPrime);
		return sigmaTr;
	}

	void JensenBssrdf::sampleProbeRay(const Intersection& isect, const Vector2f& u, Float sigmaTr, Float rMax, Ray& probeRay, float& pdf) const {
		Vector2f sample = Warp::GaussianDisk(u, sigmaTr, rMax);
		Float halfChordLength = sqrt(rMax * rMax - Dot(sample, sample));
		Frame frame(isect.n);
		Vector3f p(sample.x, halfChordLength, sample.y);
		p = frame.ToWorld(p);
		p += isect.p;
		probeRay.o = p;
		probeRay.d = -isect.n;
		probeRay.tmax = 2 * halfChordLength;
		pdf = Warp::GaussianDiskPdf(sample.x, sample.y, sigmaTr, rMax);
		return;
	}

	Vector3f JensenBssrdf::singleScatter(const Scene& scene, const Intersection& isect, const Vector3f& in, const Sampler* sampler) const {
		Float cosi = abs(Dot(in, isect.n));
		Float cost = sqrt(1 - (1 - cosi * cosi) / (eta * eta));
		Float ft = 1 - DielectricFresnel(cosi, cost, 1, eta);
		Float sigmaTr = GetLuminance(getSigmaTr());
		Vector3f sigmaS = scatter;
		Vector3f sigmaT = absorb + sigmaS;
		Vector3f refractDir;

		Vector3f ss(0);
		Float d = Warp::ExponentialDistance(sampler->Next1D(), sigmaTr);
		Vector3f pos = isect.p + refractDir * d;
		Float posPdf = Warp::ExponentialDistancePdf(d, sigmaTr);
		Intersection sampleIsect;
		sampleIsect.p = pos;

		const Distribution1D* distribution = scene.LightLookup(isect.p);
		int idx = distribution->SampleDiscrete(sampler->Next1D());
		Float choicePdf = distribution->DiscretePdf(idx);
		Light* light = scene.GetLight(idx);
		Float lightPdf;
		Ray shadowRay;
		Vector3f radiance;
		light->SampleLight(sampleIsect, sampler->Next2D(), radiance, lightPdf, shadowRay);
		if (lightPdf == 0) {
			return 0;
		}

		Float tmax = shadowRay.tmax;
		Intersection outIsect;
		if (scene.Intersect(shadowRay, outIsect)) {
			if (outIsect.bssrdf) {
				shadowRay.tmin = shadowRay.tmax + Epsilon;
				shadowRay.tmax = tmax;
				if (scene.Occluded(shadowRay)) return 0;

				Float phase = INV4PI;
				Float coso = abs(Dot(outIsect.n, shadowRay.d));
				Float cosot = sqrt(1 - (1 - coso * coso) / (eta * eta));
				Float fti = 1 - DielectricFresnel(coso, cosot, 1, eta);
				Float G = abs(Dot(outIsect.n, refractDir)) / coso;
				Vector3f sigmaTc = (1 + G) * sigmaT;
				Float si = (isect.p - pos).LengthSquare();
				Float et = 1 / eta;
				Float siPrime = si * coso / sqrt(1 - et * et * (1 - coso * coso));
				ss += (ft * fti * phase * sigmaS / sigmaTc) * Exp(-(siPrime + si) * sigmaT) / (lightPdf * choicePdf * posPdf);
			}
		}

		return ss;
	}

	Vector3f JensenBssrdf::multipleScatter(const Scene& scene, const Intersection& isect, const Vector3f& in, const Sampler* sampler) const {
		Vector3f p = isect.p;
		Float cosi = abs(Dot(in, isect.n));
		Float cost = sqrt(1 - (1 - cosi * cosi) / (eta * eta));
		Float ft = 1 - DielectricFresnel(cosi, cost, 1, eta);
		Float sigmaTr = GetLuminance(getSigmaTr());

		Float rMax = sqrt(6.214608 / sigmaTr);

		Vector3f ms(0);
		Ray probeRay;
		Float pdf;
		sampleProbeRay(isect, sampler->Next2D(), sigmaTr, rMax, probeRay, pdf);
		Intersection probeIsect;
		while (scene.Intersect(probeRay, probeIsect)) {
			if (probeIsect.bssrdf) {
				Vector3f Rd = rd((probeIsect.p - p).LengthSquare());

				const Distribution1D* distribution = scene.LightLookup(probeIsect.p);
				int idx = distribution->SampleDiscrete(sampler->Next1D());
				Float choicePdf = distribution->DiscretePdf(idx);
				Light* light = scene.GetLight(idx);
				Float lightPdf;
				Ray shadowRay;
				Vector3f radiance;
				light->SampleLight(probeIsect, sampler->Next2D(), radiance, lightPdf, shadowRay);
				if (lightPdf == 0) {

				}

				if (scene.Occluded(shadowRay)) {

				}

				Float coso = abs(Dot(shadowRay.d, probeIsect.n));
				Float cosot = sqrt(1 - (1 - coso * coso) / (eta * eta));
				Vector3f irradiance = radiance * coso / (lightPdf * choicePdf);
				Float fti = 1 - DielectricFresnel(coso, cosot, 1, eta);
				pdf *= abs(Dot(probeRay.d, probeIsect.n));

				ms += INVPI * ft * fti * Rd * irradiance / pdf;
			}
		}

		return ms;
	}

	//Float FresnelMoment1(Float eta) {
	//	Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta, eta5 = eta4 * eta;
	//	if (eta < 1) {
	//		return 0.45966f - 1.73965f * eta + 3.37668f * eta2 - 3.904945 * eta3 +
	//			2.49277f * eta4 - 0.68441f * eta5;
	//	}
	//	else {
	//		return -4.61686f + 11.1136f * eta - 10.4646f * eta2 + 5.11455f * eta3 -
	//			1.27198f * eta4 + 0.12746f * eta5;
	//	}

	//}

	//Float FresnelMoment2(Float eta) {
	//	Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta,
	//		eta5 = eta4 * eta;
	//	if (eta < 1) {
	//		return 0.27614f - 0.87350f * eta + 1.12077f * eta2 - 0.65095f * eta3 +
	//			0.07883f * eta4 + 0.04860f * eta5;
	//	}
	//	else {
	//		Float r_eta = 1 / eta, r_eta2 = r_eta * r_eta, r_eta3 = r_eta2 * r_eta;
	//		return -547.033f + 45.3087f * r_eta3 - 218.725f * r_eta2 +
	//			458.843f * r_eta + 404.557f * eta - 189.519f * eta2 +
	//			54.9327f * eta3 - 9.00603f * eta4 + 0.63942f * eta5;
	//	}
	//}

	//void ComputeBeamDiffusionBssrdf(Float g, Float eta) {

	//}

	//Vector3f SeparableBssrdf::Sw(const Vector3f& w) const {
	//	Float c = 1 - 2 * FresnelMoment1(1 / eta);
	//	Float cosi = Frame::AbsCosTheta(w);
	//	Float sint2 = Frame::SinTheta2(w) / eta;
	//	Float cost = sqrt(Clamp(1 - sint2, Float(0), Float(1)));
	//	return (1 - DielectricFresnel(cosi, cost, 1, eta)) / (c * PI);
	//}

	//TabulatedBssrdf::TabulatedBssrdf(const Intersection& isect, Float eta, const Vector3f& sigmaA, const Vector3f& sigmaS, const BssrdfTable& table) 
	//	:table(table){
	//	sigmaT = sigmaA + sigmaS;
	//	albedo = sigmaS / sigmaT;
	//	if (isnan(albedo.X())) albedo[0] = 0;
	//	if (isnan(albedo.Y())) albedo[1] = 0;
	//	if (isnan(albedo.Z())) albedo[2] = 0;
	//}

	//BssrdfTable::BssrdfTable(int nRhoSamples, int nRadiusSamples) {

	//}

	//Float BssrdfTable::BeamDiffusionMS(Float sigmaS, Float sigmaA, Float g, Float eta, Float r) {
	//	const int nSamples = 100;
	//	Float eD = 0;
	//	Float sigmaSPrime = sigmaS * (1 - g);
	//	Float sigmaTPrime = sigmaA + sigmaSPrime;
	//	Float albedoPrime = sigmaSPrime / sigmaTPrime;
	//	Float dG = (2 * sigmaA + sigmaSPrime) / (3 * sigmaTPrime * sigmaTPrime);
	//	Float sigmaTr = sqrt(sigmaA / dG);
	//	Float fm1 = FresnelMoment1(eta), fm2 = FresnelMoment2(eta);
	//	Float ze = -2 * dG * (1 + 3 * fm2) / (1 - 2 * fm1);
	//	Float cPhi = 0.25 * (1 - 2 * fm1), cE = 0.5 * (1 - 3 * fm2);

	//	for (int i = 0; i < nSamples; ++i) {
	//		Float zr = -log(1 - (i + 0.5) / nSamples) / sigmaTPrime;
	//		Float zv = -zr + 2 * ze;
	//		Float dr = sqrt(r * r + zr * zr);
	//		Float dv = sqrt(r * r + zv * zv);
	//		Float trdr = sigmaTr * dr;
	//		Float trdv = sigmaTr * dv;
	//		Float phiD = INV4PI / dG * (exp(-trdr) / dr - exp(-trdv) / dv);
	//		Float eDn = INV4PI * (zr * (1 + trdr) * exp(-trdr) / (dr * dr * dr) -
	//			zv * (1 + trdv) * exp(-trdv) / (dv * dv * dv));
	//		
	//		Float E = phiD * cPhi + eDn * cE;
	//		//corrects an overestimation that can occur when r is small and the light source is close to
	//		//the surface
	//		Float kappa = 1 - exp(-2 * sigmaTPrime * (dr + zr));
	//		//first albedoPrime factor in the scale is needed due to the ratio of the importance sampling
	//		//weight of the sampling strategy.
	//		//second albedoPrime factor accounts for the additional scattering event in Grosjean's nonclassical
	//		//monopole
	//		eD += kappa * albedoPrime * albedoPrime * E;
	//	}

	//	eD /= nSamples;
	//	return eD;
	//}

	//Float BssrdfTable::BeamDiffusionSS(Float sigmaS, Float sigmaA, Float g, Float eta, Float r) {
	//	const int nSamples = 100;
	//	Float sigmaT = sigmaA + sigmaS;
	//	Float albedo = sigmaS / sigmaT;
	//	Float tCrit = r * sqrt(eta * eta - 1);
	//	Float eSS = 0;
	//	
	//	for (int i = 0; i < nSamples; ++i) {
	//		Float ti = tCrit - log(1 - (i + 0.5) / nSamples) / sigmaT;
	//		Float d = sqrt(r * r + ti * ti);
	//		Float cost = ti / d;
	//		Float phase = INV4PI;
	//		eSS += albedo * exp(-sigmaT * (d + tCrit)) / (d * d) * phase *
	//			(1 - DielectricFresnel(1, cost, 1, eta)) * abs(cost);
	//	}

	//	eSS /= nSamples;
	//	return eSS;
	//}

	//void BssrdfTable::ComputeBeamDiffusionBssrdf(Float g, Float eta) {
	//	radiusSamples[0] = 0;
	//	radiusSamples[1] = 2.5e-3;
	//	for (int i = 2; i < nRadiusSamples; ++i) {
	//		radiusSamples[i] = radiusSamples[i - 1] * 1.2;
	//	}

	//	for (int i = 0; i < nAlbedoSamples; ++i) {
	//		albedoSamples[i] = (1 - exp(-8 * i / Float(nAlbedoSamples - 1))) / (1 - exp(-8));
	//	}

	//	for (int i = 0; i < nAlbedoSamples; ++i) {
	//		for (int j = 0; j < nRadiusSamples; ++j) {
	//			Float albedo = albedoSamples[j];
	//			Float r = radiusSamples[j];
	//			profile[i * nRadiusSamples + j] = 2 * PI * r *
	//				(BeamDiffusionSS(albedo, 1 - albedo, g, eta, r) +
	//				BeamDiffusionSS(albedo, 1 - albedo, g, eta, r));
	//		}
	//	}
	//}
}