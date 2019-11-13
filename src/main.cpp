#include "pol.h"
#include "core/timer.h"
#include "core/parallel.h"
#include "core/ray.h"
#include "core/scene.h"
#include "core/distribution.h"
#include "shape/sphere.h"
#include "shape/quad.h"
#include "shape/disk.h"
#include "shape/triangle.h"
#include "shape/heightfield.h"
#include "shape/deformable.h"
#include "camera/pinhole.h"
#include "sampler/random.h"
#include "integrator/ao.h"
#include "integrator/direct.h"
#include "integrator/path.h"
#include "bsdf/lambertian.h"
#include "bsdf/mirror.h"
#include "bsdf/dielectric.h"
#include "bsdf/roughconductor.h"
#include "bsdf/fresnelblend.h"
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
#include "core/meshio.h"

using namespace pol;

Scene VeachMis() {
	Scene scene;
	Film* film = CreateFilm("../result.png", Vector2i(960, 540));
	Camera* camera = CreatePinholeCamera(Lookat(Vector3f(28.2792, 3.5, 0), Vector3f(0, 3.5, 0), Vector3f(0, 1, 0)),
		Perspective(20.114292, Float(1280) / 720, 0.1, 100), film);

	Bvh* accelerator = CreateBvhAccelerator();
	Sampler* sampler = CreateRandomSampler(16);
	//Integrator* integrator = CreateAoIntegrator(0.5);
	Integrator* integrator = CreateDirectIntegrator();
	//Integrator* integrator = CreatePathIntegrator();

	Constant* black = CreateConstantTexture(Vector3f::Zero());
	Lambertian* emission = CreateLambertianBsdf(black);
	Sphere* s1 = CreateSphereShape(emission, Vector3f(0, 6.5, -2.8), 1);
	Sphere* s2 = CreateSphereShape(emission, Vector3f(0, 6.5, 0), 0.5);
	Sphere* s3 = CreateSphereShape(emission, Vector3f(0, 6.5, 2.7), 0.05);
	Area* l1 = CreateAreaLight(Vector3f(7.599088, 7.599088, 7.599088), s1);
	Area* l2 = CreateAreaLight(Vector3f(30.396353, 30.396353, 30.396353), s2);
	Area* l3 = CreateAreaLight(Vector3f(3039.635254, 3039.635254, 3039.635254), s3);
	s1->SetLight(l1);
	s2->SetLight(l2);
	s3->SetLight(l3);
	
	vector<Vector3f> vertices, normals;
	vector<Vector2f> uvs;
	vector<int> indices;

	Constant* gray = CreateConstantTexture(Vector3f(0.5));
	Lambertian* floor = CreateLambertianBsdf(gray);
	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/floor.obj");
	vector<Triangle*> triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, floor);
	for (int i = 0; i < triangles.size(); ++i) {
		scene.AddPrimitive(triangles[i]);
	}

	Constant* sp1 = CreateConstantTexture(Vector3f(0.3));
	Constant* alpha1 = CreateConstantTexture(Vector3f(0.01));
	RoughConductor* r1 = CreateRoughConductorBsdf(sp1, alpha1, alpha1, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate1.obj");
	triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r1);
	for (int i = 0; i < triangles.size(); ++i) {
		scene.AddPrimitive(triangles[i]);
	}

	Constant* alpha2 = CreateConstantTexture(Vector3f(0.05));
	RoughConductor* r2 = CreateRoughConductorBsdf(sp1, alpha2, alpha2, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate2.obj");
	triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r2);
	for (int i = 0; i < triangles.size(); ++i) {
		scene.AddPrimitive(triangles[i]);
	}

	Constant* alpha3 = CreateConstantTexture(Vector3f(0.1));
	RoughConductor* r3 = CreateRoughConductorBsdf(sp1, alpha3, alpha3, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate3.obj");
	triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r3);
	for (int i = 0; i < triangles.size(); ++i) {
		scene.AddPrimitive(triangles[i]);
	}

	Constant* alpha4 = CreateConstantTexture(Vector3f(0.25));
	RoughConductor* r4 = CreateRoughConductorBsdf(sp1, alpha4, alpha4, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate4.obj");
	triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r4);
	for (int i = 0; i < triangles.size(); ++i) {
		scene.AddPrimitive(triangles[i]);
	}

	scene.SetAccelerator(accelerator);
	scene.SetCamera(camera);
	scene.SetSampler(sampler);
	scene.SetIntegrator(integrator);
	scene.AddLight(l1);
	scene.AddLight(l2);
	scene.AddLight(l3);
	scene.AddPrimitive(s1);
	scene.AddPrimitive(s2);
	scene.AddPrimitive(s3);

	return scene;
}

int main(int argc, char** argv) {
//	int w, h, w1, h1;
//	vector<Vector3f> data;
//	ImageIO::LoadTexture("../assets/terrain-texture.jpg", w, h, true, data);
//	vector<Vector3f> alpha;
//	ImageIO::LoadTexture("../assets/urticaleaf_alpha.png", w1, h1, false, alpha);
//
//	vector<Vector3f> vertices, normals;
//	vector<Vector2f> uvs;
//	vector<int> indices;
//	
//	Scene scene;
//	Film* film = CreateFilm("../result.png", Vector2i(960, 540));
//	Camera* camera = CreatePinholeCamera(Lookat(Vector3f(3.04068, 3.17153, 3.20454), Vector3f(0.118789, 0.473398, 0.161081), Vector3f(0, 1, 0)),
//		Perspective(20.114292, Float(1280) / 720, 0.1, 100), film);
//
//	Bvh* accelerator = CreateBvhAccelerator();
//	Sampler* sampler = CreateRandomSampler(32);
////	Integrator* integrator = CreateAoIntegrator(0.5);
////	Integrator* integrator = CreateDirectIntegrator();
//	Integrator* integrator = CreatePathIntegrator();
//	Checkerboard* checker = CreateCheckerboardTexture(Vector3f(0.325000, 0.310000, 0.250000),
//		Vector3f(0.725000, 0.710000, 0.680000), 20, 20);
//	Image* concrete = CreateImageTexture(w, h, data, FilterMode::E_TRILINEAR);
//	Image* alphaTexture = CreateImageTexture(w1, h1, alpha);
//	Constant* general = CreateConstantTexture(Vector3f(0.75, 0.75, 0.75));
//	Constant* red = CreateConstantTexture(Vector3f(0.75, 0.25, 0.25));
//	Constant* blue = CreateConstantTexture(Vector3f(0.25, 0.25, 0.75));
//	Constant* white = CreateConstantTexture(Vector3f::One());
//	Constant* gray = CreateConstantTexture(Vector3f(0.578596, 0.578596, 0.578596));
//	Constant* dark = CreateConstantTexture(Vector3f(0.2));
//	Constant* ss = CreateConstantTexture(Vector3f(0.067215, 0.067215, 0.067215));
//	Constant* sd = CreateConstantTexture(Vector3f(0.243117, 0.059106, 0.000849));
//	Constant* roughness = CreateConstantTexture(Vector3f(0.001));
//	Mirror* mirror = CreateMirrorBsdf(white);
//	Dielectric* dielectric = CreateDielectricBsdf(white, 1, 1.5);
//	RoughConductor* al = CreateRoughConductorBsdf(white, roughness, roughness, Vector3f(1.657460, 0.880369, 0.521229), Vector3f(9.223869, 6.269523, 4.837001));
//	FresnelBlend* outer = CreateFresnelBlendBsdf(sd, ss, roughness, roughness);
//	Lambertian* stand = CreateLambertianBsdf(dark);
//	Lambertian* crete = CreateLambertianBsdf(concrete);
//	Lambertian* matte = CreateLambertianBsdf(general);
//	Lambertian* l = CreateLambertianBsdf(red);
//	Lambertian* r = CreateLambertianBsdf(blue);
//	Lambertian* f = CreateLambertianBsdf(checker);
//	Point* point = CreatePointLight(Vector3f(2, 2, 2), Vector3f(0, 1.7, 0));
//	Spot* spot = CreateSpotLight(Vector3f(2, 2, 2), Vector3f(-0.9, 1.7, 0), Vector3f(0, -1, 0), 20, 15);
//	Distant* distant = CreateDistantLight(Vector3f(4, 4, 4), Vector3f(1, -1, 1));
//	Infinite* infinite = CreateInfiniteLight(RotateY(-113.0162), "../assets/envmap.exr");
//
//	Sphere* s = CreateSphereShape(mirror, Vector3f(-0.45, 0.4, -0.1), 0.4);
//	Sphere* s1 = CreateSphereShape(dielectric, Vector3f(0.45, 0.4, 0.4), 0.4);
//	Quad* floor = CreateQuadShape(f, Vector3f(-0.708772, 0, -0.732108), Vector3f(0, 46.1511, 180), Vector3f(5.43618 / 2));
////	Quad* floor = CreateQuadShape(matte, Vector3f(0, 0, 0), Vector3f::Zero(), Vector3f::One());
//	Quad* ceil = CreateQuadShape(matte, Vector3f(0, 2, 0), Vector3f(180, 0, 0), Vector3f::One());
//	Quad* back = CreateQuadShape(matte, Vector3f(0, 1, -1), Vector3f(90, 0, 0), Vector3f::One());
//	Quad* left = CreateQuadShape(l, Vector3f(-1, 1, 0), Vector3f(0, 0, -90), Vector3f::One());
//	Quad* right = CreateQuadShape(r, Vector3f(1, 1, 0), Vector3f(0, 0, 90), Vector3f::One());
////	Quad* light = CreateQuadShape(matte, Vector3f(-0.005, 1.98, -0.03), Vector3f(180, 0, 0), Vector3f(0.235, 1, 0.19));
////	Disk* light = CreateDiskShape(matte, Vector3f(0, 1.98, 0), Vector3f(180, 0, 0), 0.3);
//	Sphere* light = CreateSphereShape(matte, Vector3f(0, 1.5, 0), 0.2);
//	Area* area = CreateAreaLight(Vector3f(12, 12, 12), light);
//	light->SetLight(area);
//
//	Transform trans = TRS(Vector3f(0.0571719,
//		0.213656,
//		0.0682078), Vector3f(0, 0, 0), Vector3f(0.482906));
//	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/Mesh001.obj");
//	vector<Triangle*> triangles = CreateTriangleMeshShape(trans, vertices, normals, uvs, indices, outer);
//	//vector<Triangle*> triangles = CreateHeightFieldShape(trans, "../assets/terrain-heightmap.png", crete);
//	//vector<Triangle*> triangles = CreateSubDivisionShape(4, trans, vertices, normals, uvs, indices, matte);
//	for (int i = 0; i < triangles.size(); ++i) {
//		scene.AddPrimitive(triangles[i]);
//	}
//	vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
//	trans = TRS(Vector3f(0.156382,
//		0.777229,
//		0.161698), Vector3f(0, 0, 0), Vector3f(0.482906));
//	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/Mesh002.obj");
//	triangles = CreateTriangleMeshShape(trans, vertices, normals, uvs, indices, outer);
//	for (int i = 0; i < triangles.size(); ++i) {
//		scene.AddPrimitive(triangles[i]);
//	}
//	vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
//	trans = TRS(Vector3f(0.110507,
//		0.494301,
//		0.126194), Vector3f(0, 0, 0), Vector3f(0.482906));
//	MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/Mesh000.obj");
//	triangles = CreateTriangleMeshShape(trans, vertices, normals, uvs, indices, stand);
//	for (int i = 0; i < triangles.size(); ++i) {
//		scene.AddPrimitive(triangles[i]);
//	}
//
//
//	scene.SetAccelerator(accelerator);
//	scene.SetCamera(camera);
//	scene.SetSampler(sampler);
//	scene.SetIntegrator(integrator);
//	scene.AddTexture(general);
//	scene.AddTexture(concrete);
//	scene.AddTexture(red);
//	scene.AddTexture(blue);
//	scene.AddTexture(white);
//	scene.AddTexture(checker);
//	scene.AddBsdf(mirror);
//	scene.AddBsdf(dielectric);
//	scene.AddBsdf(matte);
//	scene.AddBsdf(crete);
//	scene.AddBsdf(r);
//	scene.AddBsdf(l);
//	scene.AddBsdf(f);
//	//scene.AddLight(point);
//	//scene.AddLight(spot);
//	//scene.AddLight(distant);
//	//scene.AddLight(area);
//	scene.AddLight(infinite);
//	//scene.AddPrimitive(s);
//	//scene.AddPrimitive(s1);
//	scene.AddPrimitive(floor);
//	/*scene.AddPrimitive(ceil);
//	scene.AddPrimitive(back);
//	scene.AddPrimitive(left);
//	scene.AddPrimitive(right);
//	scene.AddPrimitive(light);*/
	Scene scene = VeachMis();

	scene.Prepare("spatial");

	printf("%s\n", scene.ToString().c_str());

	Timer timer;
	timer.Start();
	scene.Render();
	timer.End();
	printf("\nrendering time:%fs\n", timer.GetElapsed());
}
