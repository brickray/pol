#include "scene.h"
#include "renderblock.h"
#include "parallel.h"

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

		for (TextureIterator it = textures.begin(); it != textures.end(); ++it) POL_SAFE_DELETE(it->second);
		for (BsdfIterator it = bsdfs.begin(); it != bsdfs.end(); ++it) POL_SAFE_DELETE(it->second);
		for (Shape* shape : primitives) POL_SAFE_DELETE(shape);
		for (Light* light : lights) POL_SAFE_DELETE(light);

		Parallel::Shutdown();
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
		lights.push_back(l);
	}

	void Scene::AddBsdf(const string& name, Bsdf* b) {
		if (bsdfs.find(name) != bsdfs.end()) {
			fprintf(stderr, "bsdf named [\"%s\"] already exists\n", name.c_str());
			return;
		}

		bsdfs[name] = b;
	}

	void Scene::AddTexture(const string& name, Texture* t) {
		if (textures.find(name) != textures.end()) {
			fprintf(stderr, "texture named [\"%s\"] already exists\n", name.c_str());
			return;
		}

		textures[name] = t;
	}

	void Scene::Prepare(const string& lightStrategy) {
		bool terminal = false;
		if (!integrator) {
			printf("There is no integrator in the scene\n");
			terminal = true;
		}
		if (!sampler) {
			printf("There is no sampler in the scene\n");
			terminal = true;
		}
		if (!camera) {
			printf("There is no camera in the scene\n");
			terminal = true;
		}

		for (const Shape* shape : primitives) {
			if (!shape->GetBsdf()) {
				printf("At least one shape no bsdf\n");
				terminal = true;
			}
		}

		if (terminal) exit(1);

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
			if (light->IsInfinite()) infinite = light;
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

		//init parallel
		Parallel::Startup();
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

		if (isect.dpdu == Vector3f::Zero() || isect.dpdv == Vector3f::Zero()) {
			isect.geoFrame = Frame(isect.n);
			isect.shFrame = isect.geoFrame;
		}
		else {
			Vector3f dpdu = Normalize(isect.dpdu);
			isect.geoFrame = Frame(dpdu, isect.n, Cross(isect.n, dpdu));
			isect.shFrame = isect.geoFrame;
		}

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
		vector<RenderBlock> rbs;
		//get render block
		InitRenderBlock(*this, rbs);

		Film* film = camera->GetFilm();
		Sampler* sampler = this->sampler;
		int sampleCount = sampler->GetSampleCount();

		Parallel::ParallelLoop([&](const RenderBlock& rb) {
			Sampler* samplerClone = sampler->Clone();
			int sx = rb.sx, sy = rb.sy;
			int ex = rb.sx + rb.w, ey = rb.sy + rb.h;
			for (int i = sx; i < ex; ++i) {
				for (int j = sy; j < ey; ++j) {
					samplerClone->Prepare(j * film->res.x + i);
					Vector3f color(0.f);
					for (int s = 0; s < sampleCount; ++s) {
						Vector2f offset = samplerClone->Next2D() - Vector2f(0.5);
						Vector2f sample = Vector2f(i, j) + offset;
						RayDifferential ray = camera->GenerateRayDifferential(sample, samplerClone->Next2D());

						color += integrator->Li(ray, *this, samplerClone);
					}

					color /= Float(sampleCount);
					film->AddPixel(Vector2f(i, j), color);
				}
			}

			delete samplerClone;
		}, rbs);

		while (!Parallel::IsFinish());

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