#include "line.h"
#include "../core/scene.h"

namespace pol {
	POL_REGISTER_CLASS(Line, "line");

	Line::Line(const PropSets& props, Scene& scene)
		:Shape(props, scene) {
		p0 = props.GetVector3f("p0", Vector3f(0, 0, 0));
		p1 = props.GetVector3f("p1", Vector3f(1, 1, 1));
		width0 = props.GetFloat("width0", 0.2);
		width1 = props.GetFloat("width1", 0.2);
		Vector3f dir = Normalize(p1 - p0);
		Vector3f u, w;
		CoordinateSystem(dir, u, w);
		
		Matrix4 m(u.X(), u.Y(), u.Z(), Dot(u, -p0),
			dir.X(), dir.Y(), dir.Z(), Dot(dir, -p0),
			w.X(), w.Y(), w.Z(), Dot(w, -p0),
			0, 0, 0, 1);

		world = Transform(m.Inverse());
	}

	Float Line::SurfaceArea() const {
		Float len = (p1 - p0).Length();
		Float radius = (width0 + width1) * 0.5;
		return 2 * PI * radius * len;
	}

	BBox Line::WorldBBox() const {
		BBox bounds;
		float maxWidth = width0 > width1 ? width0 : width1;
		bounds.Union(p0 - Vector3f(maxWidth, 0, maxWidth));
		bounds.Union(p1 + Vector3f(maxWidth, 0, maxWidth));

		return world.TransformBBox(bounds);
	}

	//x^2 + y^2 = (width0 + z / length * (width1 - width0))^2
	//(ox + t*dx)^2 + (oy + t*dy)^2 = (width0 + (oz + t*dz)/length*width)^2
	//ox^2 + 2ox*t*dx + (t*dx)^2 + oy^2 + 2oy*t*dy + (t*dy)^2 = width0^2 + 2 * width0 * (oz + t*dz)/lenth*width +
	//(oz^2 + 2oz*t*dz + (t*dz)^2)*width^2/length^2
	bool Line::Intersect(Ray& ray, Intersection& isect) const {
		//transform ray to local space
		Ray r = world.TransformRayInverse(ray);

		Float widthsq = (width1 - width0) * (width1 - width0);
		Float lensq = (p1 - p0).LengthSquare();
		Float len = sqrt(lensq);
		Float cons0 = width0 + r.o.Y() * (width1 - width0) / len;
		Float cons1 = r.d.Y() * (width1 - width0) / len;
		Float A = r.d.X() * r.d.X() + r.d.Z() * r.d.Z() - cons1 * cons1;
		Float B = 2 * (r.o.X() * r.d.X() + r.o.Z() * r.d.Z() - cons0 * cons1);
		Float C = r.o.X() * r.o.X() + r.o.Z() * r.o.Z() - cons0 * cons0;
		
		Float det = B * B - 4 * A * C;
		if (det <= 0) return false;
		Float sqrtDet = sqrt(det);

		Float t1 = (-B - sqrtDet) / (2 * A);
		Float t2 = (-B + sqrtDet) / (2 * A);
		if (t1 > t2) swap(t1, t2);

		bool isFar = false;
		Float t = t1;
		if (t < r.tmin || t > r.tmax) {
			t = t2;
			if (t < r.tmin || t > r.tmax) return false;
			isFar = true;
		}

		Vector3f pHit = r(t);
		if (pHit.Y() < 0 || pHit.Y() > len) {
			if (isFar) return false;

			//check t2 that if satisfy
			t = t2;
			if (t < r.tmin || t > r.tmax) return false;
			pHit = r(t);
			if (pHit.Y() < 0 || pHit.Y() > len) return false;
		}

		//x^2 + z^2 - (y / len * (w1 - w0)) ^ 2 = 0
		//gradient vector is perpendicular to isosurface
		//Fx = 2x, Fy = -2 * (y / len^2 * (w1 - w0)^2), Fz = 2z

		ray.tmax = t;
		isect.p = ray(t);
		isect.n = world.TransformNormal(Normalize(Vector3f(2 * pHit.X(), -2 * pHit.Y() * widthsq / lensq, 2 * pHit.Z())));
		isect.uv = Vector2f(0); //does not use
		isect.dpdu = 0;
		isect.dpdv = 0;
		isect.bsdf = const_cast<Bsdf*>(bsdf);
		isect.bssrdf = bssrdf;
		isect.light = light;

		return true;
	}

	bool Line::Occluded(const Ray& ray) const {
		//transform ray to local space
		Ray r = world.TransformRayInverse(ray);

		Float widthsq = (width1 - width0) * (width1 - width0);
		Float lensq = (p1 - p0).LengthSquare();
		Float len = sqrt(lensq);
		Float cons0 = width0 + r.o.Y() * (width1 - width0) / len;
		Float cons1 = r.d.Y() * (width1 - width0) / len;
		Float A = r.d.X() * r.d.X() + r.d.Z() * r.d.Z() - cons1 * cons1;
		Float B = 2 * (r.o.X() * r.d.X() + r.o.Z() * r.d.Z() - cons0 * cons1);
		Float C = r.o.X() * r.o.X() + r.o.Z() * r.o.Z() - cons0 * cons0;

		Float det = B * B - 4 * A * C;
		if (det <= 0) return false;
		Float sqrtDet = sqrt(det);

		Float t1 = (-B - sqrtDet) / (2 * A);
		Float t2 = (-B + sqrtDet) / (2 * A);
		if (t1 > t2) swap(t1, t2);

		bool isFar = false;
		Float t = t1;
		if (t < r.tmin || t > r.tmax) {
			t = t2;
			if (t < r.tmin || t > r.tmax) return false;
			isFar = true;
		}

		Vector3f pHit = r(t);
		if (pHit.Y() < 0 || pHit.Y() > len) {
			if (isFar) return false;

			//check t2 that if satisfy
			t = t2;
			if (t < r.tmin || t > r.tmax) return false;
			pHit = r(t);
			if (pHit.Y() < 0 || pHit.Y() > len) return false;
		}

		return true;
	}

	void Line::SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const {
		//unimplement
	}

	void Line::SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdfA) const {
		//unimplement
	}

	Float Line::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const {
		//unimplement
		return 0;
	}

	//return a human-readable string summary
	string Line::ToString() const {
		string ret;
		ret += "Line[\n  world = " + indent(world.ToString())
			+ ",\n" + "  bounds = " + indent(WorldBBox().ToString())
			+ ",\n" + "  p0 = " + indent(p0.ToString())
			+ ",\n" + "  p1 = " + indent(p1.ToString())
			+ ",\n" + "  width0 = " + to_string(width0)
			+ ",\n" + "  width1 = " + to_string(width1)
			+ ",\n" + "  bsdf = " + indent(bsdf->ToString())
			+ "\n]";

		return ret;
	}
}