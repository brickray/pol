#pragma once

#include "../pol.h"

namespace pol {
	class ImageIO {
	public:
		static bool LoadTexture(const char* filename, int& width, int& height, bool srgb, bool flip, vector<Vector3f>& output);
		static bool SavePng(const char* filename, int width, int height, const vector<Vector3f>& input);
		static bool SavePng(const char* filename, int width, int height, const Vector3f* input);
		static bool LoadExr(const char* filename, int& width, int& height, vector<Vector3f>& output);
		static bool SaveExr(const char* filename, int width, int height, const vector<Vector3f>& input);
	};
}