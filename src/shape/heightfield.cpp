#include "heightfield.h"
#include "triangle.h"
#include "../core/imageio.h"

namespace pol {
	bool CreateHeightFieldShape(const char* file, TriangleMesh* mesh) {
		int w, h;
		vector<Vector3f> height;
		if (!ImageIO::LoadTexture(file, w, h, false, height)) return false;
		
		mesh->p.resize(w * h);
		mesh->n.resize(w * h);
		mesh->uv.resize(w * h);
		mesh->indices.resize(6 * (w - 1) * (h - 1));

		//calculate vertex and uv
		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				int idx = i * w + j;
				Float x = Float(j) / (w - 1);
				Float z = Float(i) / (h - 1);
				mesh->p[idx] = Vector3f(x, height[idx].X(), z);
				mesh->uv[idx] = Vector2f(x, z);
			}
		}

#define ADDRESS(x, y) (y) * w + (x)
		//generate smooth normal
		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				int idx = i * w + j;
				//corner, just one normal
				if (i == 0 && j == 0) {
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j, i + 1)];
					Vector3f p2 = mesh->p[ADDRESS(j + 1, i)];
					mesh->n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}
				else if (i == 0 && j == w - 1) {
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j - 1, i)];
					Vector3f p2 = mesh->p[ADDRESS(j, i + 1)];
					mesh->n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}
				else if (i == h - 1 && j == 0) {
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j + 1, i)];
					Vector3f p2 = mesh->p[ADDRESS(j, i - 1)];
					mesh->n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}
				else if (i == h - 1 && j == w - 1) {
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j, i - 1)];
					Vector3f p2 = mesh->p[ADDRESS(j - 1, i)];
					mesh->n[idx] = Normalize(Cross(p1 - p0, p2 - p0));
				}

				//edge two normal
				else if (j == 0 || j == w - 1) {
					int offset = j == 0 ? 1 : -1;
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j, i + offset)];
					Vector3f p2 = mesh->p[ADDRESS(j + offset, i)];
					Vector3f p3 = mesh->p[ADDRESS(j, i - offset)];
					mesh->n[idx] += Normalize(Cross(p1 - p0, p2 - p0));
					mesh->n[idx] += Normalize(Cross(p2 - p0, p3 - p0));
					mesh->n[idx] = Normalize(mesh->n[idx]);
				}
				else if (i == 0 || i == h - 1) {
					int offset = i == 0 ? 1 : -1;
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j - offset, i)];
					Vector3f p2 = mesh->p[ADDRESS(j, i + offset)];
					Vector3f p3 = mesh->p[ADDRESS(j + offset, i)];
					mesh->n[idx] += Normalize(Cross(p1 - p0, p2 - p0));
					mesh->n[idx] += Normalize(Cross(p2 - p0, p3 - p0));
					mesh->n[idx] = Normalize(mesh->n[idx]);
				}

				//center four normal
				else {
					Vector3f p0 = mesh->p[ADDRESS(j, i)];
					Vector3f p1 = mesh->p[ADDRESS(j, i - 1)];
					Vector3f p2 = mesh->p[ADDRESS(j - 1, i)];
					Vector3f p3 = mesh->p[ADDRESS(j, i + 1)];
					Vector3f p4 = mesh->p[ADDRESS(j + 1, i)];
					mesh->n[idx] += Normalize(Cross(p1 - p0, p2 - p0));
					mesh->n[idx] += Normalize(Cross(p2 - p0, p3 - p0));
					mesh->n[idx] += Normalize(Cross(p3 - p0, p4 - p0));
					mesh->n[idx] += Normalize(Cross(p4 - p0, p1 - p0));
					mesh->n[idx] = Normalize(mesh->n[idx]);
				}
			}
		}
#undef ADDRESS

		//calculate face index
		int pointer = 0;
		for (int i = 0; i < h - 1; ++i) {
			for (int j = 0; j < w - 1; ++j) {
				mesh->indices[pointer++] = (i + 0) * w + j + 0;
				mesh->indices[pointer++] = (i + 1) * w + j + 0;
				mesh->indices[pointer++] = (i + 1) * w + j + 1;

				mesh->indices[pointer++] = (i + 0) * w + j + 0;
				mesh->indices[pointer++] = (i + 1) * w + j + 1;
				mesh->indices[pointer++] = (i + 0) * w + j + 1;
			}
		}

		return true;
	}
}