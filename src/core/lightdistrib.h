#pragma once

#include "../pol.h"
#include "distribution.h"

namespace pol {
	class Scene;
	class LightDistribution {
	public:
		LightDistribution();
		virtual ~LightDistribution();

		virtual const Distribution1D* Lookup(const Vector3f& p) const = 0;
	};

	class UniformLightDistribution : public LightDistribution {
	private:
		Distribution1D distribution;

	public:
		UniformLightDistribution(const Scene& scene);

		virtual const Distribution1D* Lookup(const Vector3f& p) const;
	};

	class PowerLightDistribution : public LightDistribution {
	private:
		Distribution1D distribution;

	public:
		PowerLightDistribution(const Scene& scene);

		virtual const Distribution1D* Lookup(const Vector3f& p) const;
	};

	class SpatialLightDistribution : public LightDistribution {
	private:
		const Scene& scene;
		Vector3i nVoxels;
		struct Entry {
			bool valid = false;
			Distribution1D distrib;
		};
		mutable vector<Entry> entries;

	public:
		SpatialLightDistribution(const Scene& scene, int maxVoxel = 64);

		virtual const Distribution1D* Lookup(const Vector3f& p) const;
	};
}