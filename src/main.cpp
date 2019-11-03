#include "pol.h"
#include "core/timer.h"
#include "core/parallel.h"
#include "core/ray.h"
#include "core/scene.h"
#include "core/distribution.h"
#include "shape/sphere.h"
#include "shape/quad.h"
#include "shape/disk.h"
#include "camera/pinhole.h"
#include "sampler/random.h"
#include "integrator/ao.h"
#include "integrator/direct.h"
#include "integrator/path.h"
#include "bsdf/lambertian.h"
#include "bsdf/mirror.h"
#include "bsdf/dielectric.h"
#include "bsdf/roughconductor.h"
#include "texture/constant.h"
#include "texture/checkerboard.h"
#include "texture/image.h"
#include "light/point.h"
#include "light/spot.h"
#include "light/distant.h"
#include "light/area.h"
#include "light/infinite.h"
#include "accelerator/bvh.h"
#include "core/imageio.h"
#include "core/memory.h"
#include "core/mipmap.h"

using namespace pol;
int main(int argc, char** argv) {
	int w, h;
	vector<Vector3f> data;
	ImageIO::LoadTexture("../lines.png", w, h, true, data);
	Scene scene;
	Film* film = CreateFilm("../result.png", Vector2i(512, 512));
	Camera* camera = CreatePinholeCamera(Lookat(Vector3f(0, 1, 6.8), Vector3f(0, 1, 0), Vector3f(0, 1, 0)),
		Perspective(19.5, 1, 0.1, 100), film);

	Bvh* accelerator = CreateBvhAccelerator();
	Sampler* sampler = CreateRandomSampler(64);
//	Integrator* integrator = CreateAoIntegrator(0.5);
//	Integrator* integrator = CreateDirectIntegrator();
	Integrator* integrator = CreatePathIntegrator();
	Checkerboard* checker = CreateCheckerboardTexture(Vector3f(0.325000, 0.310000, 0.250000),
		Vector3f(0.725000, 0.710000, 0.680000), 20, 20);
	Image* concrete = CreateImageTexture(w, h, data, FilterMode::E_TRILINEAR);
	Constant* general = CreateConstantTexture(Vector3f(0.75, 0.75, 0.75));
	Constant* red = CreateConstantTexture(Vector3f(0.75, 0.25, 0.25));
	Constant* blue = CreateConstantTexture(Vector3f(0.25, 0.25, 0.75));
	Constant* white = CreateConstantTexture(Vector3f::one);
	Constant* gray = CreateConstantTexture(Vector3f(0.578596, 0.578596, 0.578596));
	Constant* roughness = CreateConstantTexture(Vector3f(0.1));
	Mirror* mirror = CreateMirrorBsdf(white);
	Dielectric* dielectric = CreateDielectricBsdf(white, 1, 1.5);
	RoughConductor* al = CreateRoughConductorBsdf(white, roughness, roughness, Vector3f(1.657460, 0.880369, 0.521229), Vector3f(9.223869, 6.269523, 4.837001));
	Lambertian* crete = CreateLambertianBsdf(concrete);
	Lambertian* matte = CreateLambertianBsdf(general);
	Lambertian* l = CreateLambertianBsdf(red);
	Lambertian* r = CreateLambertianBsdf(blue);
	Lambertian* f = CreateLambertianBsdf(checker);
	Point* point = CreatePointLight(Vector3f(2, 2, 2), Vector3f(0, 1.7, 0));
	Spot* spot = CreateSpotLight(Vector3f(2, 2, 2), Vector3f(-0.9, 1.7, 0), Vector3f(0, -1, 0), 20, 15);
	Distant* distant = CreateDistantLight(Vector3f(2, 2, 2), Vector3f(1, -1, -1));
	Infinite* infinite = CreateInfiniteLight(RotateY(90), "../envmap.exr");

	Sphere* s = CreateSphereShape(al, Vector3f(-0.45, 0.4, -0.1), 0.4);
	Sphere* s1 = CreateSphereShape(matte, Vector3f(0.45, 0.4, 0.4), 0.4);
	Quad* floor = CreateQuadShape(matte, Vector3f(0, 0, 0), Vector3f::zero, Vector3f::one);
	Quad* ceil = CreateQuadShape(matte, Vector3f(0, 2, 0), Vector3f(180, 0, 0), Vector3f::one);
	Quad* back = CreateQuadShape(matte, Vector3f(0, 1, -1), Vector3f(90, 0, 0), Vector3f::one);
	Quad* left = CreateQuadShape(l, Vector3f(-1, 1, 0), Vector3f(0, 0, -90), Vector3f::one);
	Quad* right = CreateQuadShape(r, Vector3f(1, 1, 0), Vector3f(0, 0, 90), Vector3f::one);
//	Quad* light = CreateQuadShape(matte, Vector3f(-0.005, 1.98, -0.03), Vector3f(180, 0, 0), Vector3f(0.235, 1, 0.19));
//	Disk* light = CreateDiskShape(matte, Vector3f(0, 1.98, 0), Vector3f(180, 0, 0), 0.3);
	Sphere* light = CreateSphereShape(matte, Vector3f(0, 1.5, 0), 0.2);
	Area* area = CreateAreaLight(Vector3f(12, 12, 12), light);
	light->SetLight(area);

	scene.SetAccelerator(accelerator);
	scene.SetCamera(camera);
	scene.SetSampler(sampler);
	scene.SetIntegrator(integrator);
	scene.AddTexture(general);
	scene.AddTexture(concrete);
	scene.AddTexture(red);
	scene.AddTexture(blue);
	scene.AddTexture(white);
	scene.AddTexture(checker);
	scene.AddBsdf(mirror);
	scene.AddBsdf(dielectric);
	scene.AddBsdf(matte);
	scene.AddBsdf(crete);
	scene.AddBsdf(r);
	scene.AddBsdf(l);
	scene.AddBsdf(f);
	//scene.AddLight(point);
	//scene.AddLight(spot);
	//scene.AddLight(distant);
	scene.AddLight(area);
	//scene.AddLight(infinite);
	scene.AddPrimitive(s);
	scene.AddPrimitive(s1);
	scene.AddPrimitive(floor);
	scene.AddPrimitive(ceil);
	scene.AddPrimitive(back);
	scene.AddPrimitive(left);
	scene.AddPrimitive(right);
	scene.AddPrimitive(light);

	scene.Prepare("spatial");

	printf("%s\n", scene.ToString().c_str());

	Timer timer;
	timer.Start();
	scene.Render();
	timer.End();
	printf("\nrendering time:%fs\n", timer.GetElapsed());
}
