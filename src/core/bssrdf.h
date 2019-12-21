#pragma once

#include "../pol.h"
#include "intersection.h"

namespace pol {
	class Scene;
	class Bssrdf {
	public:
		virtual Vector3f SampleS(const Scene& scene, Float u1, const Vector2f& u2, Float& pdf) const = 0;
		virtual Vector3f EvaluateSubsurface(const Scene& scene, Float u1, const Vector2f& u2) const;
	};

	class JensenBssrdf : public Bssrdf {
	private:
		Vector3f absorb, scatter;
		Float eta, g; //anisotropy parameter
		Float coeff; //the coefficient takes into account the effect of internal diffuse reflection

	public:
		JensenBssrdf(const Vector3f& absorb, const Vector3f& scatter, Float eta, Float g);

		virtual Vector3f SampleS(const Scene& scene, Float u1, const Vector2f& u2, Float& pdf) const;

	private:
		Vector3f rd(Float d2) const;
		Float fdr(Float eta) const;
		Vector3f getSigmaTr() const;
		void sampleProbeRay(const Intersection& isect, const Vector2f& u, Float sigmaTr, Float rMax, Ray& probeRay, float& pdf) const;

		Vector3f singleScatter(const Scene& scene, const Intersection& isect, const Vector3f& in, const Sampler* sampler) const;
		Vector3f multipleScatter(const Scene& scene, const Intersection& isect, const Vector3f& in, const Sampler* sampler) const;
	};

	/*class SeparableBssrdf : public Bssrdf {
	private:
		Float eta;

	public:
		Vector3f Sw(const Vector3f& w) const;
		virtual Vector3f Sr(Float r) const = 0;

		virtual Vector3f SampleS(const Scene& scene, Float u1, const Vector2f& u2, Float& pdf) const = 0;
	};

	struct BssrdfTable {
		int nAlbedoSamples, nRadiusSamples;
		vector<Float> albedoSamples, radiusSamples;
		vector<Float> profile;
		vector<Float> albedoEff;
		vector<Float> profileCdf;

		BssrdfTable(int nRhoSamples, int nRadiusSamples);
		inline Float EvalProfile(int rhoIndex, int radiusIndex) const {
			return profile[rhoIndex * nRadiusSamples + radiusIndex];
		}

		Float BeamDiffusionMS(Float sigmaS, Float sigmaA, Float g, Float eta, Float r);
		Float BeamDiffusionSS(Float sigmaS, Float sigmaA, Float g, Float eta, Float r);

		void ComputeBeamDiffusionBssrdf(Float g, Float eta);
	};

	class TabulatedBssrdf : public SeparableBssrdf {
	private:
		const BssrdfTable table;
		Vector3f sigmaT, albedo;

	public:
		TabulatedBssrdf(const Intersection& isect, Float eta, const Vector3f& sigmaA, const Vector3f& sigmaS, const BssrdfTable& table);

		virtual Vector3f Sr(Float r) const = 0;
	};*/
}