#include "pinhole.h"

namespace pol {
	POL_REGISTER_CLASS(Pinhole, "pinhole");

	Pinhole::Pinhole(const PropSets& props, Scene& scene)
		:ProjectionCamera(props, scene) {
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
		Ray ray(Vector3f(0.f), Normalize(vc));
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
		RayDifferential ray(Vector3f(0.f), Normalize(vc));
		ray.rxOrigin = ray.ryOrigin = Vector3f(0.f);
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

	void Pinhole::SampleWe(const Vector3f& pos, Vector3f& we, Ray& shadowRay, Float& pdf, Vector2i& pRaster) const {
		//transform pos to camera space
		Vector3f pCamera = view.TransformPoint(pos);
		Vector3f dir = pCamera;
		Vector3f ndir = Normalize(dir);
		Float length = dir.Length();
		shadowRay = Ray(Vector3f::Zero(), ndir, Epsilon, length - Epsilon);
		shadowRay = view.TransformRayInverse(shadowRay);

		Vector2f res = film->res;
		Vector2f invRes = Float(1) / res;
		Vector3f pNDC = projection.TransformPoint(ndir);
		if (pNDC.X() < -(1 + invRes.x) || pNDC.X() > (1 - invRes.x) ||
			pNDC.Y() < -(1 + invRes.y) || pNDC.Y() > (1 - invRes.y)) {
			//ray can not hit the sensor plane
			//no contribution
			pdf = 0;
			return;
		}
		Vector2f screen = (Vector2f(pNDC.X(), pNDC.Y()) + Float(1)) * Float(0.5) * res + Float(0.5);
		screen.x = Clamp(screen.x, Float(0), Float(res.x - 1));
		screen.y = Clamp(screen.y, Float(0), Float(res.y - 1));

		Float costheta = Dot(ndir, Vector3f(0, 0, -1));
		we = near * near / (area * costheta * costheta * costheta * costheta);
		pdf = length * length / costheta;
		pRaster = screen;
	}

	void Pinhole::PdfWe(const Vector3f& dir, Float& pdfA, Float& pdfW) const {
		Float costheta = Dot(dir, Vector3f(0, 0, -1));
		pdfA = 1;
		pdfW = near * near / (area * costheta * costheta * costheta);
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
}