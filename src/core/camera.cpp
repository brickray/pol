#include "camera.h"

namespace pol {
	Camera::Camera(const Transform& view, Film* film)
	:view(view), film(film){

	}

	Camera::~Camera() {
		delete film;
	}

	Film* Camera::GetFilm() const {
		return film;
	}


	ProjectionCamera::ProjectionCamera(const Transform& view, const Transform& projection, Film* film)
		:Camera(view, film), projection(projection) {}

	ProjectionCamera::~ProjectionCamera() {

	}
}