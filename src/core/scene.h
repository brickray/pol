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
#include "lightdistrib.h"

namespace pol {
	class Scene {
	private:
		Camera* camera;
		Sampler* sampler;
		Integrator* integrator;
		Accelerator* accelerator;

		vector<Shape*> primitives;
		vector<Light*> lights;
		Light* infinite;
		typedef map<string, Bsdf*>::iterator BsdfIterator;
		typedef map<string, Bsdf*>::const_iterator ConstBsdfIterator;
		typedef map<string, Texture*>::iterator TextureIterator;
		typedef map<string, Texture*>::const_iterator ConstTextureIterator;
		map<string, Bsdf*> bsdfs;
		map<string, Texture*> textures;

		LightDistribution* lightDistribution;
		BBox worldBBox;

	public:
		Scene();
		~Scene();

		void SetCamera(Camera* c);
		void SetSampler(Sampler* s);
		void SetIntegrator(Integrator* i);
		void SetAccelerator(Accelerator* a);

		void AddPrimitive(Shape* s);
		void AddLight(Light* l);
		void AddBsdf(const string& name, Bsdf* b);
		void AddTexture(const string& name, Texture* t);

		__forceinline Camera* GetCamera() const { return camera; }
		__forceinline Sampler* GetSampler() const { return sampler; }
		__forceinline Integrator* GetIntegrator() const { return integrator; }
		__forceinline Accelerator* GetAccelerator() const { return accelerator; }
		__forceinline Light* GetLight(int idx) const { POL_ASSERT(idx < lights.size());  return lights[idx]; }
		__forceinline vector<Light*> GetLight() const { return lights; }
		__forceinline Light* GetInfiniteLight() const { return infinite; }
		__forceinline int GetLightIndex(const Light* l) const { for (int i = 0; i < lights.size(); ++i) if (lights[i] == l) return i; return -1; }
		__forceinline Shape* GetShape(int idx) const { POL_ASSERT(idx < primitives.size()); return primitives[idx]; }
		__forceinline Bsdf* GetBsdf(string& name) const { ConstBsdfIterator it = bsdfs.find(name);  if (it == bsdfs.end()) return nullptr;  return it->second; }
		__forceinline Texture* GetTexture(string& name) const { ConstTextureIterator it = textures.find(name); if (it == textures.end()) return nullptr; return it->second; }
		__forceinline BBox GetBBox() const { return worldBBox; }

		//prepare before rendering
		void Prepare(const string& lightStrategy);

		//light lookup
		//choose a light from uniform number u
		__forceinline const Distribution1D* LightLookup(const Vector3f& p) const {
			return lightDistribution->Lookup(p);
		}

		bool Intersect(Ray& ray, Intersection& isect) const;
		bool Occluded(const Ray& ray) const;
		void Render() const;

		//return a brief string summary of the instance(for debugging purposes)
		string ToString() const;
	};
}