#include "film.h"
#include "imageio.h"
#include "directory.h"

namespace pol {
	Film::Film(const string& filename, const Vector2i& res, string tonemap, Float scale)
		:filename(filename), res(res), tonemap(tonemap), scale(scale) {
		//resize image buffer
		image.resize(res.x * res.y);
		locks = new mutex[res.x * res.y];
	}

	Film::~Film() {
		delete[] locks;
	}

	void Film::AddPixel(int p, const Vector3f& c) {
		POL_ASSERT(p < res.x * res.y);

		image[p] = c;
	}

	void Film::AddPixel(const Vector2i& p, const Vector3f& c) {
		POL_ASSERT(p.x < res.x && p.y < res.y);

		int pix = p.y * res.x + p.x;
		image[pix] = c;
	}

	void Film::AddSample(int p, const Vector3f& c) {
		POL_ASSERT(p < res.x * res.y);

		lock_guard<mutex> lock(locks[p]);
		image[p] += c;
	}

	void Film::AddSample(const Vector2i& p, const Vector3f& c) {
		POL_ASSERT(p.x < res.x && p.y < res.y);

		int pix = p.y * res.x + p.x;
		lock_guard<mutex> lock(locks[pix]);
		image[pix] += c;
	}

	bool Film::WriteImage(Float weight) {
		for (Vector3f& c : image) {
			c *= scale * weight;
			if (tonemap == "gamma") c = gamma(c);
			else if (tonemap == "filmic") c = filmic(c);
			else c = filmic(c);
		}

		bool success = ImageIO::SavePng(Directory::GetFullPath(filename).c_str(), res.x, res.y, image);
		
		return success;
	}

	Vector3f Film::filmic(const Vector3f& in) const {
		Vector3f c = in - Vector3f(0.004);
		c = Max(c, Vector3f(0.f));
		c = (c * (Float(6.2) * c + Float(0.5))) / (c * (Float(6.2) * c + Float(1.7)) + Float(0.06));
	
		return c;
	}

	Vector3f Film::gamma(const Vector3f& c) const {
		auto toSRGB = [](Float value)->Float {
			if (value < 0.0031308)
				return 12.92 * value;
			return 1.055 * pow(value, 1.0 / 2.4) - 0.055;
		};

		return Vector3f(toSRGB(c.X()), toSRGB(c.Y()), toSRGB(c.Z()));
	}

	Film* CreateFilm(const string& filename, const Vector2i& res, string tonemap, Float scale) {
		return new Film(filename, res, tonemap, scale);
	}
}