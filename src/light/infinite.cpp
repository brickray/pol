#include "infinite.h"
#include "../core/scene.h"
#include "../core/imageio.h"

namespace pol {
	Infinite::Infinite(const Transform& world, int w, int h, const vector<Vector3f>& data)
		:world(world) {
		image.Build(w, h, data, FilterMode::E_LINEAR, WrapMode::E_REPEAT);

		//consider, for example, a constant-valued environment map: with the p(u,v)
		//sampling technique, all(theta, phi) values are equally likely to be chosen.
		//Due to the maping to directions on the sphere, however, this would lead to 
		//more directions being sampled near the poles of the sphere, not a uniform 
		//sampling of directions on the sphere, which would be a more desirable result.
		//The 1/sin(theta) term in the p(w) PDF corrects for this non-uniform sampling
		//of directions so that correct results are computed in Monte Carlo estimates.
		//Given this state of affairs, however, it's better to have modified the p(u,v)
		//sampling distribution so that it's less likely to select directions near the
		//poles in the first place.
		vector<Float> func(w * h);
		for (int i = 0; i < h; ++i) {
			Float v = Float(i) / h;
			Float theta = v * PI;
			Float sintheta = sin(theta);
			for (int j = 0; j < w; ++j) {
				Float u = Float(j) / w;
				int idx = i * w + j;
				func[idx] = GetLuminance(data[idx]) * sintheta;
			}
		}

		distribution = Distribution2D(&func[0], w, h);
	}

	void Infinite::Prepare(const Scene& scene) {
		scene.GetBBox().BoundingSphere(center, radius);
	}

	bool Infinite::IsDelta() const {
		return false;
	}

	bool Infinite::IsInfinite() const {
		return true;
	}

	//power = ¡Ò¡ÒLe*cos(t)*dW*dA
	//here the power computed is approximate
	//assume Le is constant
	//power = Le¡Ò¡Òcos(t)*dW*dA  where cos(t) are always 1
	//      = Le*4*PI*¡ÒdA
	//      = Le*4*PI*A
	//      = Le*4*PI*PI*radius*radius
	Float Infinite::Luminance() const {
		Vector3f power = Float(FOURPI) * Float(PI) * radius * radius * image.Lookup(Vector2f(0.5, 0.5));
	
		return GetLuminance(power);
	}

	void Infinite::SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {
		Vector2f uv = distribution.SampleContinuous(u, pdf);
		Float theta = uv.y * PI;
		Float phi = uv.x * TWOPI;
		Float sintheta = sin(theta);
		if (sintheta == 0) {
			pdf = 0;
			return;
		}
		Vector3f dir = SphericalCoordinate(theta, phi);
		//transform dir to world space
		//transformation should not contain scale component
		dir = world.TransformVector(dir);
		rad = image.Lookup(uv);
		pdf /= (TWOPI * PI * sintheta);
		shadowRay = Ray(isect.p, dir);
	}

	Float Infinite::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		Vector3f dir = Normalize(pOnLight - pOnSurface);
		dir = world.TransformVectorInverse(dir);
		Float theta = SphericalTheta(dir);
		Float phi = SphericalPhi(dir);
		Float u = phi * INV2PI;
		Float v = theta * INVPI;
		Float sintheta = sin(theta);
		if (sintheta == 0) return 0;
		return distribution.Pdf(Vector2f(u, v)) / (TWOPI * PI * sintheta);
	}

	Vector3f Infinite::Le(const Vector3f& in, const Vector3f& nor) const {
		Vector3f dir = Normalize(world.TransformVectorInverse(-in));
		Float theta = SphericalTheta(dir);
		Float phi = SphericalPhi(dir);
		Float u = phi * INV2PI;
		Float v = theta * INVPI;

		return image.Lookup(Vector2f(u, v));
	}

	string Infinite::ToString() const {
		string ret;
		ret += "Infinite[\n  world = " + indent(world.ToString())
			+ ",\n  image = " + indent(image.ToString())
			+ "\n]";

		return ret;
	}

	Infinite* CreateInfiniteLight(const Transform& world, int w, int h, const vector<Vector3f>& data) {
		return new Infinite(world, w, h, data);
	}

	Infinite* CreateInfiniteLight(const Transform& world, const char* file) {
		int w, h;
		vector<Vector3f> data;
		ImageIO::LoadExr(file, w, h, data);

		return CreateInfiniteLight(world, w, h, data);
	}
}