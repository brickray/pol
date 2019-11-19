#pragma once

#include "object.h"
#include "ray.h"
#include "film.h"

namespace pol {
	class Camera : public PolObject {
	protected:
		//world to view transform
		Transform view;
		//film to store color infomation
		Film* film;

	public:
		Camera(const PropSets& props, Scene& scene);
		virtual ~Camera();

		//generate primiary ray from given sample point
		virtual Ray GenerateRay(const Vector2f& cameraSample, const Vector2f& dofSample) const = 0;
		//generate ray differential, mainly for texture anti-aliasing
		virtual RayDifferential GenerateRayDifferential(const Vector2f& cameraSample, const Vector2f& dofSample) const = 0;

		virtual void SampleWe(const Vector3f& pos, Vector3f& we, Ray& shadowRay, Float& pdf, Vector2i& pRaster) const;
		virtual Float PdfWe() const;

		Film* GetFilm() const;
	};

	class ProjectionCamera : public Camera {
	protected:
		//projection transform
		Transform projection;
		Vector3f dxCamera, dyCamera;
		Float area;

	public:
		ProjectionCamera(const PropSets& props, Scene& scene);
		virtual ~ProjectionCamera();
	};
}