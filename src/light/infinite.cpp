#include "infinite.h"

namespace pol {
	Infinite::Infinite(int w, int h, const vector<Vector3f>& data) {

	}

	bool Infinite::IsDelta() const {
		return false;
	}

	bool Infinite::IsInfinite() const {
		return true;
	}

	Float Infinite::Luminance() const {
		return 0;
	}

	void Infinite::SampleLight(const Intersection& isect, const Vector2f& u, Vector3f& rad, Float& pdf, Ray& shadowRay) const {

	}

	Float Infinite::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		return 0;
	}

	string Infinite::ToString() const {
		string ret;
		ret += "Infinite[\n]";

		return ret;
	}
}