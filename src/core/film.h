#pragma once

#include "../pol.h"
#include <mutex>

namespace pol {
	class Film {
	public:
		//file name to store image
		string filename;
		//film resolution
		Vector2i res;
		//type of tonemap
		string tonemap;
		//image data
		vector<Vector3f> image;
		Float scale;

		mutex* locks;

	public:
		Film(const string& filename, const Vector2i& res, string tonemap, Float scale);
		~Film();

		void AddPixel(int p, const Vector3f& c);
		void AddPixel(const Vector2i& p, const Vector3f& c);
		void AddSample(int p, const Vector3f& c);
		void AddSample(const Vector2i& p, const Vector3f& c);
		bool WriteImage(Float weight);

	private:
		Vector3f filmic(const Vector3f& c) const;
		Vector3f gamma(const Vector3f& c) const;
	};

	Film* CreateFilm(const string& filename, const Vector2i& res, string tonemap = "gamma", Float scale = 1);
}