#include "camera.h"
#include "scene.h"

namespace pol {
	Camera::Camera(const PropSets& props, Scene& scene)
	:view(view), film(film){
		if (props.HasValue("view")) {
			view = props.GetTransform("view");
		}
		else {
			Vector3f eye = props.GetVector3f("position");
			Vector3f lookat = props.GetVector3f("lookat");
			Vector3f up = props.GetVector3f("up", Vector3f::Up());

			view = Lookat(eye, lookat, up);
		}

		int xRes = props.GetInt("xRes");
		int yRes = props.GetInt("yRes");
		string tonemap = props.GetString("tonemap", "gamma");
		string output = props.GetString("output");
		Float scale = props.GetFloat("scale", 1);
		film = new Film(output, Vector2i(xRes, yRes), tonemap, scale);

		scene.SetCamera(this);
	}

	Camera::~Camera() {
		delete film;
	}

	Vector3f Camera::GetPosition() const {
		Matrix3 rotate = Matrix3(view.m);
		rotate = rotate.Inverse();
		Matrix4 rotate4x4 = Matrix4(rotate);
		Matrix4 translate = rotate4x4 * view.m;

		return Vector3f(-translate.m[0][3], -translate.m[1][3], -translate.m[2][3]);
	}

	Film* Camera::GetFilm() const {
		return film;
	}

	ProjectionCamera::ProjectionCamera(const PropSets& props, Scene& scene)
		:Camera(props, scene) {
		Float fov = props.GetFloat("fov", 60);
		int xRes = props.GetInt("xRes", 512);
		int yRes = props.GetInt("yRes", 512);
		near = props.GetFloat("near", 0.1);
		far = props.GetFloat("far", 100);
		projection = Perspective(fov, Float(xRes) / yRes, near, far);

		//compute sensor area
		Float height = tan(Radians(fov * 0.5)) * near;
		Float width = height * Float(xRes) / yRes;
		area = 4 * width * height;
	}

	ProjectionCamera::~ProjectionCamera() {

	}
}