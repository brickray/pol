#include "film.h"
#include "imageio.h"

namespace pol {
	Film::Film(const string& filename, const Vector2i& res, string tonemap)
		:filename(filename), res(res), tonemap(tonemap) {
		//resize image buffer
		image.resize(res.x * res.y);
	}

	Film::~Film() {

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

	bool Film::WriteImage() {
		for (Vector3f& c : image) {
			if (tonemap == "gamma") c = gamma(c);
			else if (tonemap == "filmic") c = filmic(c);
		}

		bool success = ImageIO::SavePng(filename.c_str(), res.x, res.y, image);
		
		return success;
	}

	Vector3f Film::filmic(const Vector3f& in) const {
		Vector3f c = in - Vector3f(0.04);
		c.x = Max(c.x, Float(0));
		c.y = Max(c.y, Float(0));
		c.z = Max(c.z, Float(0));
		c = (c * (Float(6.2) * c + Float(0.5))) / (c * (Float(6.2) * c + Float(1.7)) + Float(0.06));
	
		return c;
	}

	Vector3f Film::gamma(const Vector3f& c) const {
		auto toSRGB = [](Float value)->Float {
			if (value < 0.0031308)
				return 12.92 * value;
			return 1.055 * pow(value, 1.0 / 2.4) - 0.055;
		};

		return Vector3f(toSRGB(c.x), toSRGB(c.y), toSRGB(c.z));
	}

	Film* CreateFilm(const string& filename, const Vector2i& res, string tonemap) {
		return new Film(filename, res, tonemap);
	}
}