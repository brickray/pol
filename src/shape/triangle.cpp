#include "triangle.h"

namespace pol {
	TriangleMesh::TriangleMesh(const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
		const vector<Vector2f>& uv, const vector<int>& indices, Bsdf* bsdf, Texture* alphaMask)
		:p(p), n(n), uv(uv), indices(indices), alphaMask(alphaMask) {
		//transform triangle mesh to world space
		for (Vector3f& vertex : this->p) {
			vertex = world.TransformPoint(vertex);
		}

		for (Vector3f& normal : this->n) {
			normal = world.TransformNormal(normal);
		}

		hasTexcoord = uv.size() != 0;
	}

	string TriangleMesh::ToString() const {
		string ret;
		string null = "nullptr";
		ret += "TriangleMesh[\n  triangles count = " + to_string(indices.size() / 3)
			+ ",\n  vertices count = " + to_string(p.size())
			+ ",\n  normal count = " + to_string(n.size())
			+ ",\n  uv count = " + to_string(uv.size())
			+ ",\n  alpha mask = " + (alphaMask ? indent(alphaMask->ToString()) : null);

		return ret;
	}

	vector<Triangle*> CreateTriangleMesh(const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
		const vector<Vector2f>& uv, const vector<int>& indices, Bsdf* bsdf, Texture* alphaMask) {
		//auto release
		shared_ptr<TriangleMesh> mesh = shared_ptr<TriangleMesh>(new TriangleMesh(world, p, n, uv, indices, bsdf, alphaMask));

		int nTriangles = indices.size() / 3;
		vector<Triangle*> triangles;
		triangles.reserve(nTriangles);
		for (int i = 0; i < nTriangles; ++i) {
			triangles.push_back(new Triangle(world, bsdf, mesh, i));
		}

		return triangles;
	}

	Triangle::Triangle(const Transform& world, Bsdf* bsdf, const shared_ptr<TriangleMesh>& mesh, int faceIndex)
		:Shape(bsdf), mesh(mesh), faceIndex(3 * faceIndex) {

	}

	Float Triangle::SurfaceArea() const {
		Vector3f v1 = mesh->p[mesh->indices[faceIndex + 0]];
		Vector3f v2 = mesh->p[mesh->indices[faceIndex + 1]];
		Vector3f v3 = mesh->p[mesh->indices[faceIndex + 2]];

		return Cross(v2 - v1, v3 - v1).Length() * 0.5;
	}

	BBox Triangle::WorldBBox() const {
		BBox worldBBox;
		Vector3f v1 = mesh->p[mesh->indices[faceIndex + 0]];
		Vector3f v2 = mesh->p[mesh->indices[faceIndex + 1]];
		Vector3f v3 = mesh->p[mesh->indices[faceIndex + 2]];
		worldBBox.Union(v1);
		worldBBox.Union(v2);
		worldBBox.Union(v3);

		return worldBBox;
	}

	bool Triangle::Intersect(Ray& ray, Intersection& isect) const {
		int idx1 = mesh->indices[faceIndex + 0];
		int idx2 = mesh->indices[faceIndex + 1];
		int idx3 = mesh->indices[faceIndex + 2];
		Vector3f v1 = mesh->p[idx1];
		Vector3f v2 = mesh->p[idx2];
		Vector3f v3 = mesh->p[idx3];
		Vector3f n1 = mesh->n[idx1];
		Vector3f n2 = mesh->n[idx2];
		Vector3f n3 = mesh->n[idx3];
		Vector2f uv1;
		Vector2f uv2;
		Vector2f uv3;
		if (mesh->hasTexcoord) {
			uv1 = mesh->uv[idx1];
			uv2 = mesh->uv[idx2];
			uv3 = mesh->uv[idx3];
		}

		Vector3f e1 = v2 - v1;
		Vector3f e2 = v3 - v1;
		Vector3f s1 = Cross(ray.d, e2);
		float divisor = Dot(s1, e1);
		if (fabs(divisor) < 1e-8f)
			return false;
		float invDivisor = 1.0 / divisor;
		Vector3f s = ray.o - v1;
		float b1 = Dot(s, s1) * invDivisor;
		if (b1 < 0.0 || b1 > 1.0)
			return false;

		Vector3f s2 = Cross(s, e1);
		float b2 = Dot(ray.d, s2) * invDivisor;
		if (b2 < 0.0 || b1 + b2 > 1.0)
			return false;

		float tt = Dot(e2, s2) * invDivisor;
		if (tt < ray.tmin || tt > ray.tmax)
			return false;

		//calculate UV
		Vector2f uv = uv1* (1.f - b1 - b2) + uv2 * b1 + uv3 * b2;

		isect.p = ray(tt);
		isect.n = Normalize(n1 * (1.f - b1 - b2) + n2 * b1 + n3 * b2);
		isect.uv = uv;
		isect.dpdu = 0;
		isect.dpdv = 0;
		isect.bsdf = bsdf;
		isect.light = light;

		//alpha mask texture exists?
		if (mesh->alphaMask) {
			Float alpha = mesh->alphaMask->Evaluate(isect).x;
			if (alpha < Epsilon) {
				//if alpha < Epsilon, discard
				return false;
			}
		}

		//finally, set tmax
		ray.tmax = tt;
		return true;
	}

	bool Triangle::Occluded(const Ray& ray) const {
		int idx1 = mesh->indices[faceIndex + 0];
		int idx2 = mesh->indices[faceIndex + 1];
		int idx3 = mesh->indices[faceIndex + 2];
		Vector3f v1 = mesh->p[idx1];
		Vector3f v2 = mesh->p[idx2];
		Vector3f v3 = mesh->p[idx3];
		Vector3f n1 = mesh->n[idx1];
		Vector3f n2 = mesh->n[idx2];
		Vector3f n3 = mesh->n[idx3];
		Vector2f uv1;
		Vector2f uv2;
		Vector2f uv3;
		if (mesh->hasTexcoord) {
			uv1 = mesh->uv[idx1];
			uv2 = mesh->uv[idx2];
			uv3 = mesh->uv[idx3];
		}

		Vector3f e1 = v2 - v1;
		Vector3f e2 = v3 - v1;
		Vector3f s1 = Cross(ray.d, e2);
		float divisor = Dot(s1, e1);
		if (fabs(divisor) < 1e-8f)
			return false;
		float invDivisor = 1.0 / divisor;
		Vector3f s = ray.o - v1;
		float b1 = Dot(s, s1) * invDivisor;
		if (b1 < 0.0 || b1 > 1.0)
			return false;

		Vector3f s2 = Cross(s, e1);
		float b2 = Dot(ray.d, s2) * invDivisor;
		if (b2 < 0.0 || b1 + b2 > 1.0)
			return false;

		float tt = Dot(e2, s2) * invDivisor;
		if (tt < ray.tmin || tt > ray.tmax)
			return false;

		return true;
	}
	void Triangle::SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const {
		int idx1 = mesh->indices[faceIndex + 0];
		int idx2 = mesh->indices[faceIndex + 1];
		int idx3 = mesh->indices[faceIndex + 2];
		Vector3f v1 = mesh->p[idx1];
		Vector3f v2 = mesh->p[idx2];
		Vector3f v3 = mesh->p[idx3];
		Vector3f n1 = mesh->n[idx1];
		Vector3f n2 = mesh->n[idx2];
		Vector3f n3 = mesh->n[idx3];

		Vector2f uv = Warp::UniformTriangle(u);
		pos = v1 * (1 - uv.x - uv.y) + v2 * uv.x + v3 * uv.y;
		nor = n1 * (1 - uv.x - uv.y) + n2 * uv.y + n3 * uv.y;
		pdf = 1 / SurfaceArea();
		solidAngle = false;
	}

	Float Triangle::Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface) const {
		return 1 / SurfaceArea();
	}

	//return a human-readable string summary
	string Triangle::ToString() const {
		string ret = "";
		if (faceIndex == 0) {
			ret += mesh->ToString();
			ret += ",\n  bsdf = " + indent(bsdf->ToString());
			ret += "\n]";

			return ret;
		}

		return ret;
	}
}