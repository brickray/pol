#pragma once

#include "object.h"
#include "film.h"

namespace pol {
	class Camera : public PolObject {
	protected:
		//world to view transform
		Transform view;
		//film to store color infomation
		Film* film;

	public:
		Camera(const Transform& view, Film* film);
		virtual ~Camera();

		//generate primiary ray from given sample point
		virtual Ray GenerateRay(const Vector2f& cameraSample, const Vector2f& dofSample) const = 0;
	
		Film* GetFilm() const;
	};

	class ProjectionCamera : public Camera {
	protected:
		//projection transform
		Transform projection;

	public:
		ProjectionCamera(const Transform& view, const Transform& projection, Film* film);
		virtual ~ProjectionCamera();
	};
}