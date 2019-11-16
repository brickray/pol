#include "pol.h"
#include "core/scene.h"
#include "core/parser.h"
#include "core/timer.h"

using namespace pol;

Scene VeachMis() {
	Scene scene;
	//Film* film = CreateFilm("../result.png", Vector2i(960, 540));
	//Camera* camera = CreatePinholeCamera(Lookat(Vector3f(28.2792, 3.5, 0), Vector3f(0, 3.5, 0), Vector3f(0, 1, 0)),
	//	Perspective(20.114292, Float(1280) / 720, 0.1, 100), film);

	//Bvh* accelerator = CreateBvhAccelerator();
	//Sampler* sampler = CreateRandomSampler(16);
	////Integrator* integrator = CreateAoIntegrator(0.5);
	//Integrator* integrator = CreateDirectIntegrator();
	////Integrator* integrator = CreatePathIntegrator();

	//Constant* black = CreateConstantTexture(Vector3f::Zero());
	//Lambertian* emission = CreateLambertianBsdf(black);
	//Sphere* s1 = CreateSphereShape(emission, Vector3f(0, 6.5, -2.8), 1);
	//Sphere* s2 = CreateSphereShape(emission, Vector3f(0, 6.5, 0), 0.5);
	//Sphere* s3 = CreateSphereShape(emission, Vector3f(0, 6.5, 2.7), 0.05);
	//Area* l1 = CreateAreaLight(Vector3f(7.599088, 7.599088, 7.599088), s1);
	//Area* l2 = CreateAreaLight(Vector3f(30.396353, 30.396353, 30.396353), s2);
	//Area* l3 = CreateAreaLight(Vector3f(3039.635254, 3039.635254, 3039.635254), s3);
	//s1->SetLight(l1);
	//s2->SetLight(l2);
	//s3->SetLight(l3);
	//
	//vector<Vector3f> vertices, normals;
	//vector<Vector2f> uvs;
	//vector<int> indices;

	//Constant* gray = CreateConstantTexture(Vector3f(0.5));
	//Lambertian* floor = CreateLambertianBsdf(gray);
	//MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/floor.obj");
	//vector<Triangle*> triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, floor);
	//for (int i = 0; i < triangles.size(); ++i) {
	//	scene.AddPrimitive(triangles[i]);
	//}

	//Constant* sp1 = CreateConstantTexture(Vector3f(0.3));
	//Constant* alpha1 = CreateConstantTexture(Vector3f(0.01));
	//RoughConductor* r1 = CreateRoughConductorBsdf(sp1, alpha1, alpha1, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	//vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	//MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate1.obj");
	//triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r1);
	//for (int i = 0; i < triangles.size(); ++i) {
	//	scene.AddPrimitive(triangles[i]);
	//}

	//Constant* alpha2 = CreateConstantTexture(Vector3f(0.05));
	//RoughConductor* r2 = CreateRoughConductorBsdf(sp1, alpha2, alpha2, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	//vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	//MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate2.obj");
	//triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r2);
	//for (int i = 0; i < triangles.size(); ++i) {
	//	scene.AddPrimitive(triangles[i]);
	//}

	//Constant* alpha3 = CreateConstantTexture(Vector3f(0.1));
	//RoughConductor* r3 = CreateRoughConductorBsdf(sp1, alpha3, alpha3, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	//vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	//MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate3.obj");
	//triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r3);
	//for (int i = 0; i < triangles.size(); ++i) {
	//	scene.AddPrimitive(triangles[i]);
	//}

	//Constant* alpha4 = CreateConstantTexture(Vector3f(0.25));
	//RoughConductor* r4 = CreateRoughConductorBsdf(sp1, alpha4, alpha4, Vector3f(0.200438, 0.924033, 1.102212), Vector3f(3.912949, 2.452848, 2.142188));
	//vertices.clear(); normals.clear(); uvs.clear(); indices.clear();
	//MeshIO::LoadModelFromFile(vertices, normals, uvs, indices, "../assets/veach_mis/plate4.obj");
	//triangles = CreateTriangleMeshShape(Transform(), vertices, normals, uvs, indices, r4);
	//for (int i = 0; i < triangles.size(); ++i) {
	//	scene.AddPrimitive(triangles[i]);
	//}

	//scene.SetAccelerator(accelerator);
	//scene.SetCamera(camera);
	//scene.SetSampler(sampler);
	//scene.SetIntegrator(integrator);
	//scene.AddLight(l1);
	//scene.AddLight(l2);
	//scene.AddLight(l3);
	//scene.AddPrimitive(s1);
	//scene.AddPrimitive(s2);
	//scene.AddPrimitive(s3);

	return scene;
}

int main(int argc, char** argv) {
	Scene scene;
	if (!Parser::Parse("../scenes/kitchen/scene.json", scene)) {
		return 1;
	}

//	printf("%s\n", scene.ToString().c_str());

	Timer timer;
	timer.Start();
	scene.Render();
	timer.End();
	printf("\nrendering time:%fs\n", timer.GetElapsed());
}
