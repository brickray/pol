#include "pinhole.h"

namespace pol {
	Pinhole::Pinhole(const Transform& view, const Transform& projection, Film* film)
		:ProjectionCamera(view, projection, film) {

	}

	Ray Pinhole::GenerateRay(const Vector2f& cameraSample, const Vector2f& dofSample) const {
		//screen space to raster space
		//[x, y] -> [0, 1] -> [-1, 1]
		static Vector2f invRes = 1.f / Vector2f(film->res);
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

	string Pinhole::ToString() const {
		string ret;
		ret += "PinholeCamera[\n  view = " + indent(view.ToString())
			+ ",\n  projection = " + indent(projection.ToString())
			+ ",\n  film = [\n    filename = " + film->filename
			+ ",\n    [xRes = " + to_string(film->res.x)
			+ ", yRes = " + to_string(film->res.y)
			+ "]\n  ]\n]";
		
		return ret;
	}

	Pinhole* CreatePinholeCamera(const Transform& view, const Transform& projection, Film* film) {
		return new Pinhole(view, projection, film);
	}
}