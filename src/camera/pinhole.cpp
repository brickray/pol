#include "pinhole.h"

namespace pol {
	Pinhole::Pinhole(const Transform& view, const Transform& projection, Film* film)
		:ProjectionCamera(view, projection, film) {
		Vector2f invRes = Float(1) / Vector2f(film->res);
		auto NDC = [invRes](const Vector2f& pixel)->Vector3f {
			Vector2f raster = (pixel * invRes - Vector2f(0.5))* Float(2);
			Vector3f ndc(raster.x, raster.y, -1.0/*arbitrary*/);

			return ndc;
		};

		Vector3f vc0 = projection.TransformPointInverse(NDC(Vector2f(0, 0)));
		Vector3f vcx = projection.TransformPointInverse(NDC(Vector2f(1, 0)));
		Vector3f vcy = projection.TransformPointInverse(NDC(Vector2f(0, 1)));
		dxCamera = vcx - vc0;
		dyCamera = vcy - vc0;
	}

	Ray Pinhole::GenerateRay(const Vector2f& cameraSample, const Vector2f& dofSample) const {
		//screen space to raster space
		//[x, y] -> [0, 1] -> [-1, 1]
		static Vector2f invRes = Float(1) / Vector2f(film->res);
		Vector2f raster = (cameraSample * invRes - Vector2f(0.5)) * Float(2);
		//construct ndc coord
		Vector3f ndc(raster.x, raster.y, -1.0/*arbitrary*/);
		//transform ndc coord to view coord 
		Vector3f vc = projection.TransformPointInverse(ndc);
		//create ray with origin (0,0,0) and direction (vc)
		Ray ray(Vector3f(0), Normalize(vc));
		//transform ray from view to world
		ray = view.TransformRayInverse(ray);

		return ray;
	}

	RayDifferential Pinhole::GenerateRayDifferential(const Vector2f& cameraSample, const Vector2f& dofSample) const {
		//screen space to raster space
		//[x, y] -> [0, 1] -> [-1, 1]
		static Vector2f invRes = Float(1) / Vector2f(film->res);
		Vector2f raster = (cameraSample * invRes - Vector2f(0.5)) * Float(2);
		//construct ndc coord
		Vector3f ndc(raster.x, raster.y, -1.0/*arbitrary*/);
		//transform ndc coord to view coord 
		Vector3f vc = projection.TransformPointInverse(ndc);
		Vector3f rxDirection = vc + dxCamera;
		Vector3f ryDirection = vc + dyCamera;
		//create ray with origin (0,0,0) and direction (vc)
		RayDifferential ray(Vector3f(0), Normalize(vc));
		ray.rxOrigin = ray.ryOrigin = Vector3f(0);
		ray.rxDirection = Normalize(rxDirection);
		ray.ryDirection = Normalize(ryDirection);
		//transform ray differential from view to world
		ray.o = view.TransformPointInverse(ray.o);
		ray.rxOrigin = view.TransformPointInverse(ray.rxOrigin);
		ray.ryOrigin = view.TransformPointInverse(ray.ryOrigin);
		ray.d = view.TransformVectorInverse(ray.d);
		ray.rxDirection = view.TransformVectorInverse(ray.rxDirection);
		ray.ryDirection = view.TransformVectorInverse(ray.ryDirection);

		return ray;
	}

	string Pinhole::ToString() const {
		string ret;
		ret += "PinholeCamera[\n  view = " + indent(view.ToString())
			+ ",\n  projection = " + indent(projection.ToString())
			+ ",\n  film = [\n    filename = " + film->filename
			+ ",\n    [xRes = " + to_string(film->res.x)
			+ ",  yRes = " + to_string(film->res.y)
			+ "]\n  ]\n]";
		
		return ret;
	}

	Pinhole* CreatePinholeCamera(const Transform& view, const Transform& projection, Film* film) {
		return new Pinhole(view, projection, film);
	}
}