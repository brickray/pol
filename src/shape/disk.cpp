#include "disk.h"

namespace pol {
	Disk::Disk(const Transform& world, Bsdf* bsdf, Float radius)
		:Shape(bsdf), world(world), radius(radius) {
		//precompute normal
		normal = world.TransformNormal(Vector3f::up);
	}

	Float Disk::SurfaceArea() const {
		return PI * radius * radius;
	}

	BBox Disk::WorldBBox() const {
		BBox local(Vector3f(-radius, 0, -radius),
			Vector3f(radius, 0, radius));

		return world.TransformBBox(local);
	}

	bool Disk::Intersect(Ray& ray, Intersection& isect) const {
		//first transform ray from world space to disk's local space
		Ray r = world.TransformRayInverse(ray);
		//compute t
		Float t = -r.o.y / r.d.y;
		if (t < r.tmin || t > r.tmax) return false;

		Vector3f pHit = r(t);
		Float lensq = pHit.LengthSquare();
		if (lensq > radius* radius) return false;

		//calc uv
		Float u = sqrt(lensq) / radius;
		Float v = atan2(pHit.z, pHit.x);
		if (v < 0) v += TWOPI;
		v *= INV2PI;

		//x = r*cos(phi)
		//y = 0
		//z = r*sin(phi)
		//u = r/rmax
		//v = phi/(2*pi)
		//x = rmax*u*cos(2*pi*v)
		//y = 0
		//z = rmax*u*sin(2*pi*v)
		//dxdu = rmax*cos(2*pi*v)
		//dydu = 0
		//dzdu = rmax*sin(2*pi*v)
		//dxdv = -rmax*u*sin(2*pi*v)*2*pi
		//dydv = 0
		//dzdv = rmax*u*cos(2*pi*v)*2*pi
		Float cosphi = isect.shFrame.CosPhi(pHit);
		Float sinphi = isect.shFrame.SinPhi(pHit);
		//calculate partial derivative of p to u
		Float dxdu = radius * cosphi;
		Float dydu = 0;
		Float dzdu = radius * sinphi;
		//calculate partial derivative of p to v
		Float dxdv = -radius * u * sinphi * TWOPI;
		Float dydv = 0;
		Float dzdv = radius * u * cosphi * TWOPI;

		//record intersect information
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

	bool Disk::Occluded(const Ray& ray) const {
		//first transform ray from world space to disk's local space
		Ray r = world.TransformRayInverse(ray);
		//compute t
		Float t = -r.o.y / r.d.y;
		if (t < r.tmin || t > r.tmax) return false;

		Vector3f pHit = r(t);
		if (pHit.LengthSquare() > radius* radius) return false;

		return true;
	}

	void Disk::SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const {
		Vector2f p = Warp::ConcentricDisk(u);
		pos = Vector3f(p.x * radius, 0, p.y * radius);
		pos = world.TransformPoint(pos);
		nor = normal;
		pdf = 1 / SurfaceArea();
		solidAngle = false;
	}

	Float Disk::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		return 1 / SurfaceArea();
	}

	string Disk::ToString() const {
		string ret;
		ret += "Disk[\n  world = " + indent(world.ToString())
			+ ",\n" + "  radius = " + to_string(radius)
			+ ",\n" + "  bsdf = " + indent(bsdf->ToString())
			+ ",\n  normal = " + normal.ToString()
			+ "\n]";

		return ret;
	}

	Disk* CreateDiskShape(Bsdf* bsdf, const Vector3f& t, const Vector3f& r, Float radius) {
		Transform toWorld = TRS(t, r, Vector3f::one);
		return new Disk(toWorld, bsdf, radius);
	}
}