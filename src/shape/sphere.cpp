#include "sphere.h"

namespace pol {
	Sphere::Sphere(const Transform& world, Bsdf* bsdf, Float radius)
		:Shape(bsdf), world(world), radius(radius) {

	}

	//equation of sphere's surface area
	// A = 4*pi*r^2
	Float Sphere::SurfaceArea() const {
		return FOURPI * radius * radius;
	}

	BBox Sphere::WorldBBox() const {
		BBox local(Vector3f(-radius), Vector3f(radius));

		return world.TransformBBox(local);
	}

	//sphere equation: x^2 + y^2 + z^2 = r^2
	//ray equation: p^ = o^ + t*d^ where p^ means vector
	//(o.x + t*d.x)^2 + (o.y + t*d.y)^2 + (o.z + t*d.z)^2 = r^2
	//expanding the equation, we get
	//(o.x^2 + o.y^2 + o.z^2) + 2*t(o.x*d.x + o.y*d.y + o.z*d.z) + 
	//t^2*(d.x^2 + d.y^2 + d.z^2) = r^2
	//LengthSquare(o^)+2*t*Dot(o^,d^) + t^2*LengthSquare(d^) = r^2
	//then just solve quadratic equation
	//C = LengthSquare(o^) - r^2, B = 2*Dot(o^, d^), A = LengthSquare(d^) 
	bool Sphere::Intersect(Ray& ray, Intersection& isect) const {
		//we assume length of ray direction is one, i.e., LengthSquare(ray.d) = 1

		//first we transform ray from world space to sphere's local space
		Ray r = world.TransformRayInverse(ray);
		//then calculate coefficient of quadratic equation

		//since the world to local transform just involve translation, so it doesn't
		//change length of ray direction. i.e., A = 1.0
		Float A = 1.0; //r.d.LengthSquare();
		Float B = 2.0 * Dot(r.o, r.d);
		Float C = r.o.LengthSquare() - radius*radius;

		Float t1, t2;
		if (!SolveQuadratic(A, B, C, t1, t2)) return false;

		Float t = t1;
		if (t1 > r.tmax) return false;
		if (t1 < r.tmin) {
			t = t2;
			if (t < r.tmin) return false;
			if (t > r.tmax) return false;
		}

		//calc uv
		Vector3f pHit = Normalize(r(t));
		Float u = acos(pHit.Y()) * INVPI;
		Float v = atan2(pHit.Z(), pHit.X());
		if (v < 0) v += TWOPI;
		v *= INV2PI;

		//x = r*sin(theta)*cos(phi)
		//y = r*cos(theta)
		//z = r*sin(theta)*sin(phi)
		//u = theta / pi
		//v = phi / (2*pi)
		//assume r is one, thus
		//x = sin(u*pi)*cos(2*pi*v)
		//y = cos(u*pi)
		//z = sin(u*pi)*sin(2*pi*v)
		//dxdu = cos(u*pi)*pi*cos(2*pi*v)
		//dydu = -sin(u*pi)*pi
		//dzdu = cos(u*pi)*pi*sin(2*pi*v)
		//dxdv = -sin(u*pi)*sin(2*pi*v)*2*pi
		//dydv = 0
		//dzdv = sin(u*pi)*cos(2*pi*v)*2*pi
		Float costheta = isect.shFrame.CosTheta(pHit);
		Float sintheta = isect.shFrame.SinTheta(pHit);
		Float cosphi = isect.shFrame.CosPhi(pHit);
		Float sinphi = isect.shFrame.SinPhi(pHit);
		//calculate partial derivative of p to u
		Float dxdu = costheta * PI * cosphi;
		Float dydu = -sintheta * PI;
		Float dzdu = costheta * PI * sinphi;
		//calculate partial derivative of p to v
		Float dxdv = -sintheta * sinphi * TWOPI;
		Float dydv = 0;
		Float dzdv = sintheta * cosphi * TWOPI;

		//intersect
		//record intersect information if needed
		ray.tmax = t;
		isect.p = ray(t);
		isect.n = world.TransformNormal(pHit);
		isect.uv = Vector2f(u, v);
		isect.dpdu = Vector3f(dxdu, dydu, dzdu);
		isect.dpdv = Vector3f(dxdv, dydv, dzdv);
		isect.bsdf = const_cast<Bsdf*>(bsdf);
		isect.light = light;
		//transform the Intersection
		isect(world);

		return true;
	}

	bool Sphere::Occluded(const Ray& ray) const {
		//we assume length of ray direction is one, i.e., LengthSquare(ray.d) = 1

		//first we transform ray from world space to sphere's local space
		Ray r = world.TransformRayInverse(ray);
		//then calculate coefficient of quadratic equation

		//since the world to local transform just involve translation, so it doesn't
		//change length of ray direction. i.e., A = 1.0
		Float A = 1.0; //r.d.LengthSquare();
		Float B = 2.0 * Dot(r.o, r.d);
		Float C = r.o.LengthSquare() - radius * radius;

		Float t1, t2;
		if (!SolveQuadratic(A, B, C, t1, t2)) return false;

		Float t = t1;
		if (t1 > r.tmax) return false;
		if (t1 < r.tmin) {
			t = t2;
			if (t < r.tmin) return false;
			if (t > r.tmax) return false;
		}

		return true;
	}

    void Sphere::SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const {
		Vector3f center;
		world.ExtractTranslation(center);
		//pos to center
		Vector3f dir = center - pos;
		Float costhetaMax = sqrtf(1 - radius * radius / dir.LengthSquare());
		Frame frame(Normalize(dir));
		//sample dir
		dir = Warp::UniformCone(u, costhetaMax);
		dir = frame.ToWorld(dir);

		Intersection isect;
		Ray ray(pos, dir);
		Intersect(ray, isect);

		pos = isect.p;
		nor = isect.n;
		pdf = Warp::UniformConePdf(costhetaMax);
		solidAngle = true;
	}

	Float Sphere::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		Vector3f center;
		world.ExtractTranslation(center);
		//pos to center
		Vector3f dir = center - pOnSurface;
		Float costhetaMax = sqrtf(1 - radius * radius / dir.LengthSquare());
		Float pdf = Warp::UniformConePdf(costhetaMax);
		Vector3f normal = (pOnLight - center) / radius;
		dir = pOnSurface - pOnLight;
		Float lensq = dir.LengthSquare();
		return pdf * lensq / Dot(normal, Normalize(dir));
	}

	//return a human-readable string summary
	string Sphere::ToString() const {
		string ret;
		ret += "Sphere[\n  world = " + indent(world.ToString())
			+ ",\n" + "  radius = " + to_string(radius)
			+ ",\n" + "  bsdf = " + indent(bsdf->ToString()) 
			+ "\n]";
		
		return ret;
	}

	Sphere* CreateSphereShape(Bsdf* bsdf, const Vector3f& center, Float radius) {
		Transform toWorld = Translate(center);
		return new Sphere(toWorld, bsdf, radius);
	}
}