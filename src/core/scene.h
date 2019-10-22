#pragma once

#include "../pol.h"
#include "shape.h"
#include "camera.h"
#include "sampler.h"
#include "accelerator.h"
#include "integrator.h"
#include "light.h"
#include "bsdf.h"
#include "texture.h"
#include "distribution.h"

namespace pol {
	class Scene {
	private:
		Camera* camera;
		Sampler* sampler;
		Integrator* integrator;
		Accelerator* accelerator;

		vector<Shape*> primitives;
		vector<Light*> lights;
		vector<Bsdf*> bsdfs;
		vector<Texture*> textures;

		Distribution1D lightDistribution;

	public:
		Scene();
		~Scene();

		void SetCamera(Camera* c);
		void SetSampler(Sampler* s);
		void SetIntegrator(Integrator* i);
		void SetAccelerator(Accelerator* a);

		void AddPrimitive(Shape* s);
		void AddLight(Light* l);
		void AddBsdf(Bsdf* b);
		void AddTexture(Texture* t);

		__forceinline Camera* GetCamera() const { return camera; }
		__forceinline Sampler* GetSampler() const { return sampler; }
		__forceinline Integrator* GetIntegrator() const { return integrator; }
		__forceinline Accelerator* GetAccelerator() const { return accelerator; }
		__forceinline Light* GetLight(int idx) const { POL_ASSERT(idx < lights.size());  return lights[idx]; }
		__forceinline Shape* GetShape(int idx) const { POL_ASSERT(idx < primitives.size()); return primitives[idx]; }
		__forceinline Bsdf* GetBsdf(int idx) const { POL_ASSERT(idx < bsdfs.size()); return bsdfs[idx]; }
		__forceinline Texture* GetTexture(int idx) const { POL_ASSERT(idx < textures.size()); return textures[idx]; }

		//prepare before rendering
		void Prepare(const string& lightStrategy);

		//light lookup
		//choose a light from uniform number u
		__forceinline int LightLookup(Float u) const {
			return lightDistribution.SampleDiscrete(u);
		}
		//return pdf of the 'idx' light
		__forceinline Float LightPdf(int idx) const {
			return lightDistribution.DiscretePdf(idx);
		}

		bool Intersect(Ray& ray, Intersection& isect) const;
		bool Occluded(const Ray& ray) const;
		void Render() const;

		//return a brief string summary of the instance(for debugging purposes)
		string ToString() const;
	};
}