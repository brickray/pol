#include "heightfield.h"
#include "../core/imageio.h"

namespace pol {
	vector<Triangle*> CreateHeightFieldShape(const Transform& world, int w, int h, const vector<Vector3f>& height, 
		Bsdf* bsdf, Texture* alphaMask) {
		vector<Vector3f> p(w * h);
		vector<Vector3f> n(w * h);
		vector<Vector2f> uv(w * h);
		vector<int> indices(6 * (w - 1) * (h - 1));

		//calculate vertex and uv
		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				int idx = i * w + j;
				Float x = Float(j) / (w - 1);
				Float z = Float(i) / (h - 1);
				p[idx] = Vector3f(x, height[idx].X(), z);
				uv[idx] = Vector2f(x, z);
			}
		}

#define ADDRESS(x, y) (y) * w + (x)
		//generate smooth normal
		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				int idx = i * w + j;
				//corner, just one normal
				if (i == 0 && j == 0) {
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j, i + 1)];
					Vector3f p2 = p[ADDRESS(j + 1, i)];
					n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}
				else if (i == 0 && j == w - 1) {
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j - 1, i)];
					Vector3f p2 = p[ADDRESS(j, i + 1)];
					n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}
				else if (i == h - 1 && j == 0) {
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j + 1, i)];
					Vector3f p2 = p[ADDRESS(j, i - 1)];
					n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}
				else if(i == h - 1 && j == w - 1){
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j, i - 1)];
					Vector3f p2 = p[ADDRESS(j - 1, i)];
					n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}

				//edge two normal
				else if (j == 0 || j == w - 1) {
					int offset = j == 0 ? 1 : -1;
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j, i + offset)];
					Vector3f p2 = p[ADDRESS(j + offset, i)];
					Vector3f p3 = p[ADDRESS(j, i - offset)]; 
					n[idx] += Normalize(Cross(p1 - p0, p2 - p0));
					n[idx] += Normalize(Cross(p2 - p0, p3 - p0));
					n[idx] = Normalize(n[idx]);
				}
				else if (i == 0 || i == h - 1) {
					int offset = i == 0 ? 1 : -1;
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j - offset, i)];
					Vector3f p2 = p[ADDRESS(j, i + offset)];
					Vector3f p3 = p[ADDRESS(j + offset, i)];
					n[idx] += Normalize(Cross(p1 - p0, p2 - p0));
					n[idx] += Normalize(Cross(p2 - p0, p3 - p0));
					n[idx] = Normalize(n[idx]);
				}

				//center four normal
				else {
					Vector3f p0 = p[ADDRESS(j, i)];
					Vector3f p1 = p[ADDRESS(j, i - 1)];
					Vector3f p2 = p[ADDRESS(j - 1, i)];
					Vector3f p3 = p[ADDRESS(j, i + 1)];
					Vector3f p4 = p[ADDRESS(j + 1, i)];
					n[idx] += Normalize(Cross(p1 - p0, p2 - p0));
					n[idx] += Normalize(Cross(p2 - p0, p3 - p0));
					n[idx] += Normalize(Cross(p3 - p0, p4 - p0));
					n[idx] += Normalize(Cross(p4 - p0, p1 - p0));
					n[idx] = Normalize(n[idx]);
				}
			}
		}
#undef ADDRESS

		//calculate face index
		int pointer = 0;
		for (int i = 0; i < h - 1; ++i) {
			for (int j = 0; j < w - 1; ++j) {
				indices[pointer++] = (i + 0) * w + j + 0;
				indices[pointer++] = (i + 1) * w + j + 0;
				indices[pointer++] = (i + 1) * w + j + 1;

				indices[pointer++] = (i + 0) * w + j + 0;
				indices[pointer++] = (i + 1) * w + j + 1;
				indices[pointer++] = (i + 0) * w + j + 1;
			}
		}

		return CreateTriangleMeshShape(world, p, n, uv, indices, bsdf, alphaMask);
	}

	vector<Triangle*> CreateHeightFieldShape(const Transform& world, const char* file, Bsdf* bsdf, Texture* alphaMask) {
		int w, h;
		vector<Vector3f> height;
		ImageIO::LoadTexture(file, w, h, false, height);

		return CreateHeightFieldShape(world, w, h, height, bsdf, alphaMask);
	}
}