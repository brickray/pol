#include "lightdistrib.h"
#include "scene.h"

namespace pol {
	LightDistribution::LightDistribution() {

	}

	LightDistribution::~LightDistribution() {

	}

	UniformLightDistribution::UniformLightDistribution(const Scene& scene) {
		vector<Float> luminance;
		vector<Light*> lights = scene.GetLight();
		luminance.reserve(lights.size());
		for (const Light* light : lights) {
			luminance.push_back(1);
		}
		distribution = Distribution1D(&luminance[0], luminance.size());
	}

	const Distribution1D* UniformLightDistribution::Lookup(const Vector3f& p) const {
		return &distribution;
	}

	PowerLightDistribution::PowerLightDistribution(const Scene& scene) {
		vector<Float> luminance;
		vector<Light*> lights = scene.GetLight();
		luminance.reserve(lights.size());
		for (const Light* light : lights) {
			luminance.push_back(light->Luminance());
		}
		distribution = Distribution1D(&luminance[0], luminance.size());
	}

	const Distribution1D* PowerLightDistribution::Lookup(const Vector3f& p) const {
		return &distribution;
	}

	SpatialLightDistribution::SpatialLightDistribution(const Scene& scene, int maxVoxel)
		:scene(scene) {
		BBox worldBBox = scene.GetBBox();
		Vector3f diag = worldBBox.Diagonal();
		Float maxLen = diag[diag.MajorComponent()];
		Float unit = maxVoxel / maxLen;
		//calculate the num of voxels along three axis
		for (int i = 0; i < 3; ++i) {
			nVoxels[i] = ceil(diag[i] * unit);
		}

		entries.resize(nVoxels.x * nVoxels.y * nVoxels.z);
	}

	const Distribution1D* SpatialLightDistribution::Lookup(const Vector3f& p) const {
		BBox worldBBox = scene.GetBBox();
		Vector3f offset = worldBBox.Offset(p);
		Vector3i pi = nVoxels * offset;
		pi.x = Clamp(pi.x, 0, nVoxels.x - 1);
		pi.y = Clamp(pi.y, 0, nVoxels.y - 1);
		pi.z = Clamp(pi.z, 0, nVoxels.z - 1);

		int index = pi.z * nVoxels.y * nVoxels.x + pi.y * nVoxels.x + pi.x;
		if (entries[index].valid) {
			return &entries[index].distrib;
		}
		else {
			//compute distribution

			//calculate the bbox that the point p within. 
			Vector3f diag = worldBBox.Diagonal();
			Float maxLen = diag[diag.MajorComponent()];
			Float voxelLen = maxLen / nVoxels[nVoxels.MajorComponent()];
			Vector3f fmin = Vector3f(pi.x * voxelLen, pi.y * voxelLen, pi.z * voxelLen) + worldBBox.fmin;
			Vector3f fmax = fmin + voxelLen;
			BBox voxelBBox = BBox(fmin, fmax);

			// Compute the sampling distribution. Sample a number of points inside
			// voxelBounds using a 3D Halton sequence; at each one, sample each
			// light source and compute a weight based on Li/pdf for the light's
			// sample (ignoring visibility between the point in the voxel and the
			// point on the light source) as an approximation to how much the light
			// is likely to contribute to illumination in the voxel.
			const int nSamples = 128;
			vector<Light*> lights = scene.GetLight();
			vector<Float> distribution(lights.size());
			for (int i = 0; i < nSamples; ++i) {
				Vector3f p = voxelBBox.Lerp(Vector3f(RadicalInverse(0, i),
					RadicalInverse(1, i), RadicalInverse(2, i)));
				
				Intersection isect;
				isect.p = p;

				Vector2f u = Vector2f(RadicalInverse(3, i), RadicalInverse(4, i));
				for (int j = 0; j < lights.size(); ++j) {
					Light* light = lights[j];
					Vector3f radiance;
					Float pdf;
					Ray shadowRay;
					light->SampleLight(isect, u, radiance, pdf, shadowRay);
					if (pdf != 0) {
						distribution[j] += GetLuminance(radiance) / pdf;
					}
				}
			}

			Float sum = 0;
			for (int i = 0; i < distribution.size(); ++i)
				sum += distribution[i];
			
			Float avg = sum / (nSamples * distribution.size());
			Float min = (avg > 0) ? .001 * avg : 1;
			for (size_t i = 0; i < distribution.size(); ++i) {
				distribution[i] = Max(distribution[i], min);
			}

			entries[index].distrib = Distribution1D(&distribution[0], distribution.size());
			entries[index].valid = true;
			return &entries[index].distrib;
		}
	}
}