#include "quad.h"

namespace pol {
	Quad::Quad(const Transform& world, Bsdf* bsdf)
		:Shape(bsdf), world(world) {
		//precompute normal
		normal = Normalize(world.TransformNormal(Vector3f::Up()));
	}

	Float Quad::SurfaceArea() const {
		Vector3f scale;
		world.ExtractScale(scale);
		return 4 * abs(scale.X()) * abs(scale.Z());
	}

	BBox Quad::WorldBBox() const {
		BBox local(Vector3f(-1, 0, -1),
			Vector3f(1, 0, 1));

		return world.TransformBBox(local);
	}

	bool Quad::Intersect(Ray& ray, Intersection& isect) const {
		//first transform ray from world space to quad's local space
		Ray r = world.TransformRayInverse(ray);

		//compute t
		if (r.d.Y() == 0) return false;
		Float t = -r.o.Y() / r.d.Y();
		if (t < r.tmin || t > r.tmax) return false;

		Vector3f pHit = r(t);
		if (pHit.X() < -1 || pHit.X() > 1) return false;
		if (pHit.Z() < -1 || pHit.Z() > 1) return false;

		//calc uv
		Float u = (pHit.X() + 1) / 2;
		Float v = (pHit.Z() + 1) / 2;

		//x = 2*u - 1
		//y = 0
		//z = 2*v - 1
		//dxdu = 2
		//dydu = 0
		//dzdu = 0
		//dxdv = 0
		//dydv = 0
		//dzdv = 2
		//calculate partial derivative of p to u
		Float dxdu = 2;
		Float dydu = 0;
		Float dzdu = 0;
		//calculate partial derivative of p to v
		Float dxdv = 0;
		Float dydv = 0;
		Float dzdv = 2;

		//record intersect information
		ray.tmax = t;
		isect.p = ray(t);
		isect.n = normal;
		isect.uv = Vector2f(u, v);
		isect.dpdu = Vector3f(dxdu, dydu, dzdu);
		isect.dpdv = Vector3f(dxdv, dydv, dzdv);
		isect.bsdf = const_cast<Bsdf*>(bsdf);
		isect.light = light;
		//transform the Intersection
		isect(world);

		return true;
	}

	bool Quad::Occluded(const Ray& ray) const {
		//first transform ray from world space to quad's local space
		Ray r = world.TransformRayInverse(ray);

		//compute t
		if (r.d.Y() == 0) return false;
		Float t = -r.o.Y() / r.d.Y();
		if (t < r.tmin || t > r.tmax) return false;

		Vector3f pHit = r(t);
		if (pHit.X() < -1 || pHit.X() > 1) return false;
		if (pHit.Z() < -1 || pHit.Z() > 1) return false;

		return true;
	}

	void Quad::SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const {
		pos = Vector3f(2 * u.x - 1, 0, 2 * u.y - 1);
		pos = world.TransformPoint(pos);
		nor = normal;
		pdf = 1 / SurfaceArea();
		solidAngle = false;
	}

	Float Quad::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const {
		solidAngle = false;
		return 1 / SurfaceArea();
	}

	string Quad::ToString() const {
		string ret;
		ret += "Quad[\n  world = " + indent(world.ToString())
			+ ",\n" + "  bsdf = " + indent(bsdf->ToString())
			+ ",\n" + "  bounds = " + indent(WorldBBox().ToString())
			+ ",\n  normal = " + normal.ToString()
			+ "\n]";
		
		return ret;
	}

	Quad* CreateQuadShape(Bsdf* bsdf, const Vector3f& t, const Vector3f& r, const Vector3f& s) {
		Transform world = TRS(t, r, s);

		return new Quad(world, bsdf);
	}
}