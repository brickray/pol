#include "mipmap.h"
#include "memory.h"

namespace pol {
	Mipmap::Mipmap() {
		fmode = FilterMode::E_TRILINEAR;
		wmode = WrapMode::E_REPEAT;
	}

	Mipmap::Mipmap(int w, int h, const vector<Vector3f>& data, FilterMode fmode, WrapMode wmode) {
		Build(w, h, data, fmode, wmode);
	}

	Mipmap::~Mipmap() {
		for (int i = 0; i < PyramidCount(); ++i)
			FreeAligned(pyramid[i].data);
	}

	int Mipmap::PyramidCount() const {
		if (fmode != FilterMode::E_TRILINEAR) return 1;
		return pyramid.size();
	}

	vector<TexInfo> Mipmap::GetPyramid() const {
		return pyramid;
	}

	void Mipmap::Build(int w, int h, const vector<Vector3f>& data, FilterMode fmode, WrapMode wmode) {
		this->fmode = fmode;
		this->wmode = wmode;

		int nLevel = Log2Int(Max(w, h)) + 1;
		pyramid.resize(nLevel);
		pyramid[0].w = w;
		pyramid[0].h = h;
		pyramid[0].data = AllocAligned<Vector3f>(w * h);
		for (int i = 0; i < w * h; ++i)
			pyramid[0].data[i] = data[i];

		//does not need generate mipmap if the trilinear filter does not choice
		if (fmode != FilterMode::E_TRILINEAR) return;

		for (int level = 1; level < nLevel; ++level) {
			int prevW = pyramid[level - 1].w;
			int prevH = pyramid[level - 1].h;
			int nextW = prevW == 1 ? 1 : floor(prevW >> 1);
			int nextH = prevH == 1 ? 1 : floor(prevH >> 1);
			pyramid[level].w = nextW;
			pyramid[level].h = nextH;
			pyramid[level].data = AllocAligned<Vector3f>(nextW * nextH);

			vector<Vector3f> temp(prevH * nextW);
			for (int i = 0; i < prevH; ++i) {
				for (int j = 0; j < nextW; ++j) {
					Float denominator = 2 * nextW + 1;
					Float weight1 = Float(nextW - j) / denominator;
					Float weight2 = Float(nextW) / denominator;
					Float weight3 = Float(1 + j) / denominator;

					//when prevW == 1
					//the x1 = 0, x2 = 0, x3 = 0
					int x1 = 2 * j;
					int x2 = prevW == 1 ? 0 : 2 * j + 1;
					int x3 = j + 1 == nextW ? x2 : 2 * j + 2;
					int idx1 = i * prevW + x1;
					int idx2 = i * prevW + x2;
					int idx3 = i * prevW + x3;
					Vector3f average = weight1 * pyramid[level - 1].data[idx1]
						+ weight2 * pyramid[level - 1].data[idx2]
						+ weight3 * pyramid[level - 1].data[idx3];

					temp[i * nextW + j] = average;
				}
			}

			for (int i = 0; i < nextW; ++i) {
				for (int j = 0; j < nextH; ++j) {
					Float denominator = 2 * nextH + 1;
					Float weight1 = Float(nextH - j) / denominator;
					Float weight2 = Float(nextH) / denominator;
					Float weight3 = Float(1 + j) / denominator;

					//when prevH == 1
					//the x1 = 0, x2 = 0, x3 = 0
					int x1 = 2 * j;
					int x2 = prevH == 1 ? 0 : 2 * j + 1;
					int x3 = j + 1 == nextH ? x2 : 2 * j + 2;
					int idx1 = x1 * nextW + i;
					int idx2 = x2 * nextW + i;
					int idx3 = x3 * nextW + i;
					Vector3f average = weight1 * temp[idx1]
						+ weight2 * temp[idx2]
						+ weight3 * temp[idx3];


					pyramid[level].data[j * nextW + i] = average;
				}
			}
		}
	}

	Vector3f Mipmap::Lookup(const Vector2f& uv, Float width) const {
		Vector3f color;
		switch (fmode) {
		case FilterMode::E_NEARST: {
			TexInfo ti = pyramid[0];
			int x = (ti.w - 1) * uv.x;
			int y = (ti.h - 1) * uv.y;

			color = ti.data[y * ti.w + x];
			break;
		}
		case FilterMode::E_LINEAR: {
			color = triangle(0, uv);
			break;
		}
		case FilterMode::E_TRILINEAR: {
			//compute mipmap level for trilinear filtering
			Float level = PyramidCount() - 1 + Log2(Max(width, Float(1e-8)));

			//perform trilinear interpolation at appropriate mipmap level
			if (level < 0) {
				color = triangle(0, uv);
			}
			else if (level > PyramidCount() - 1) {
				//the last pyramid only contains one pixel,
				//so just set texture coordinate to 0
				color = triangle(PyramidCount() - 1, Vector2f(0, 0));
			}
			else {
				int first = int(level);
				Float t = level - first;
				color = Lerp(triangle(first, uv), triangle(first + 1, uv), t);
			}

			break;
		}
		}

		return color;
	}

	Vector3f Mipmap::Lookup(const Intersection& isect) const {
		Vector2f uv = getTexCoordinate(isect.uv);
		Float width = Max(Max(abs(isect.dudx), abs(isect.dudy)),
			Max(abs(isect.dvdx), abs(isect.dvdy)));
		
		return Lookup(uv, width);
	}

	Vector2f Mipmap::getTexCoordinate(const Vector2f& uv) const {
		Vector2f ret;
		switch (wmode) {
		case WrapMode::E_REPEAT:
			ret.x = uv.x - int(uv.x);
			ret.y = uv.y - int(uv.y);
			if (ret.x < 0) ret.x += 1;
			if (ret.y < 0) ret.y += 1;
			break;
		case WrapMode::E_CLAMP:
			ret.x = Clamp(uv.x, Float(0), Float(1));
			ret.y = Clamp(uv.y, Float(0), Float(1));
			break;
		case WrapMode::E_MIRROR:
			ret.x = abs((int(uv.x) % 2) - uv.x + int(uv.x));
			ret.y = abs((int(uv.y) % 2) - uv.y + int(uv.y));
			break;
		}

		return ret;
	}

	Vector3f Mipmap::triangle(int level, const Vector2f& uv) const {
		TexInfo ti = pyramid[level];
		int x = (ti.w - 1) * uv.x;
		int y = (ti.h - 1) * uv.y;
		int nextX = Clamp(x + 1, 0, ti.w - 1);
		int nextY = Clamp(y + 1, 0, ti.h - 1);
		Float dx = (ti.w - 1) * uv.x - x;
		Float dy = (ti.h - 1) * uv.y - y;
		return (1 - dy) * (1 - dx) * ti.data[y * ti.w + x]
		 	 + (1 - dy) * dx * ti.data[y * ti.w + nextX]
			 + dy * (1 - dx) * ti.data[nextY * ti.w + x]
			 + dy * dx * ti.data[nextY * ti.w + nextX];
	}

	string Mipmap::ToString() const {
		string ret;
		ret += "Mipmap[\n  width = " + to_string(pyramid[0].w)
			+ ",\n  height = " + to_string(pyramid[0].h)
			+ ",\n  pyramid count = " + to_string(PyramidCount())
			+ "\n]";

		return ret;
	}
}