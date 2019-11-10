#include "scene.h"

namespace pol {
	Scene::Scene() {
		camera = nullptr;
		sampler = nullptr;
		integrator = nullptr;
		accelerator = nullptr;
		infinite = nullptr;
	}

	Scene::~Scene() {
		POL_SAFE_DELETE(camera);
		POL_SAFE_DELETE(sampler);
		POL_SAFE_DELETE(integrator);
		POL_SAFE_DELETE(accelerator);

		for (Texture* texture : textures) POL_SAFE_DELETE(texture);
		for (Bsdf* bsdf : bsdfs) POL_SAFE_DELETE(bsdf);
		for (Shape* shape : primitives) POL_SAFE_DELETE(shape);
		for (Light* light : lights) POL_SAFE_DELETE(light);
	}

	void Scene::SetCamera(Camera* c) {
		if (camera) return;

		camera = c;
	}

	void Scene::SetSampler(Sampler* s) {
		if (sampler) return;

		sampler = s;
	}

	void Scene::SetIntegrator(Integrator* i) {
		if (integrator) return;

		integrator = i;
	}

	void Scene::SetAccelerator(Accelerator* a) {
		if (accelerator) return;

		accelerator = a;
	}

	void Scene::AddPrimitive(Shape* s) {
		primitives.push_back(s);
	}

	void Scene::AddLight(Light* l) {
		if (l->IsInfinite()) {
			if(!infinite) infinite = l;
			else {
				//two infinite light? not allowed

			}
		}
		lights.push_back(l);
	}

	void Scene::AddBsdf(Bsdf* b) {
		bsdfs.push_back(b);
	}

	void Scene::AddTexture(Texture* t) {
		textures.push_back(t);
	}

	void Scene::Prepare(const string& lightStrategy) {
		//build accelerator
		if (accelerator) {
			if (primitives.size() > 20) {
				bool success = accelerator->Build(primitives);
				if (!success) return;

				worldBBox = accelerator->GetRootBBox();
			}
			else {
				//if primitive count is small,
				//brute force intersect maybe fast
				POL_SAFE_DELETE(accelerator);
				accelerator = nullptr;
			}
		}

		//light prepare
		for (Light* light : lights) {
			light->Prepare(*this);
		}

		//construct light distribution.
		if(lightStrategy == "uniform" || lights.size() == 1){
			lightDistribution = new UniformLightDistribution(*this);
		}
		else if (lightStrategy == "power") {
			lightDistribution = new PowerLightDistribution(*this);
		}
		else if (lightStrategy == "spatial") {
			lightDistribution = new SpatialLightDistribution(*this);
		}
		else {
			//unknown strategy, use spatial method
			lightDistribution = new SpatialLightDistribution(*this);
		}
	}

	bool Scene::Intersect(Ray& ray, Intersection& isect) const {
		bool intersect = false;
		if (accelerator) {
			intersect = accelerator->Intersect(ray, isect);
		}
		else {
			//brute force
			for (const Shape* shape : primitives) {
				intersect |= shape->Intersect(ray, isect);
			}
		}

		isect.geoFrame = Frame(isect.n);
		isect.shFrame = isect.geoFrame;

		return intersect;
	}

	bool Scene::Occluded(const Ray& ray) const {
		if (accelerator) {
			return accelerator->Occluded(ray);
		}
		else {
			//brute force
			for (const Shape* shape : primitives) {
				if (shape->Occluded(ray))
					return true;
			}
		}

		return false;
	}

	void Scene::Render() const {
		Film* film = camera->GetFilm();
		Sampler* sampler = this->sampler;
		int sampleCount = sampler->GetSampleCount();

		for (int i = 0; i < film->res.x; ++i) {
			for (int j = 0; j < film->res.y; ++j) {
				sampler->Prepare(j * film->res.x + i);
				Vector3f color(0);
				for (int s = 0; s < sampleCount; ++s) {
					Vector2f offset = sampler->Next2D() - Vector2f(0.5);
					Vector2f sample = Vector2f(i, j) + offset;
					RayDifferential ray = camera->GenerateRayDifferential(sample, sampler->Next2D());
		
					color += integrator->Li(ray, *this, sampler);
				}

				color /= Float(sampleCount);
				film->AddPixel(Vector2f(i, j), color);
			}

			printf("Rendering Progress[%.3f%%]\r", Float(i) / (film->res.x - 1) * 100);
		}

		film->WriteImage();
	}

	//return a brief string summary of the instance(for debugging purposes)
	string Scene::ToString() const {
		string ret;
		string null = "nullptr";

		ret += "Scene[\n";
		//bounds
		ret += "  Scene Bounds = " + indent(worldBBox.ToString()) + ",\n";
		//accelerator
		ret += "  Accelerator = " + (accelerator != nullptr ? indent(accelerator->ToString()) : null) + ",\n";
		//camera
		ret += "  Camera = " + indent(camera->ToString()) + ",\n";
		//sampler
		ret += "  Sampler = " + indent(sampler->ToString()) + ",\n";
		//integrator
		ret += "  Integrator = " + indent(integrator->ToString()) + ",\n";
		//lights
		ret += "  Lights = [\n";
		for (const Light* light : lights) {
			ret += "    " + indent(light->ToString(), 4);
			ret += "\n";
		}
		ret += "  ],\n";
		//primitives
		ret += "  Primitives = [\n";
		for (const Shape* shape : primitives) {
			string info = shape->ToString();
			if (info == "") continue;
			ret += "    " + indent(info, 4);
			ret += ",\n";
		}
		ret += "  ]\n";
		ret += "]\n";
		

		return ret;
	}
}