#include "triangle.h"

namespace pol {
	TriangleMesh::TriangleMesh(const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
		const vector<Vector2f>& uv, const vector<int>& indices, Bsdf* bsdf, Texture* alphaMask)
		:p(p), n(n), uv(uv), indices(indices), alphaMask(alphaMask) {
		//transform triangle mesh to world space
		for (Vector3f& vertex : this->p) {
			vertex = world.TransformPoint(vertex);
			bbox.Union(vertex);
		}

		if (n.size() == 0) {
			//generate normal if no normal exists
			this->n.resize(this->p.size());
			for (int i = 0; i < indices.size(); i += 3) {
				int idx1 = indices[i + 0];
				int idx2 = indices[i + 1];
				int idx3 = indices[i + 2];
				Vector3f v1 = this->p[idx1];
				Vector3f v2 = this->p[idx2];
				Vector3f v3 = this->p[idx3];
				Vector3f normal = Normalize(Cross(v2 - v1, v3 - v1));
				this->n[idx1] += normal;
				this->n[idx2] += normal;
				this->n[idx3] += normal;
			}

			for (int i = 0; i < this->p.size(); ++i) {
				this->n[i] = Normalize(this->n[i]);
			}
		}
		else {
			for (Vector3f& normal : this->n) {
				normal = world.TransformNormal(normal);
			}
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
			+ ",\n  alpha mask = " + (alphaMask ? indent(alphaMask->ToString()) : null)
			+ ",\n  bounds = " + indent(bbox.ToString());

		return ret;
	}

	vector<Triangle*> CreateTriangleMeshShape(const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
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
		Vector3f dpdu, dpdv;
		//calculate partial differential
		//p = pi + dpdu*ui + dpdv*vi
		//| p2 - p1 |   | u2 - u1  v2 - v1 | | dpdu |
		//|         | = |                  | |      |
		//| p3 - p1 |   | u3 - u1  v3 - v1 | | dpdv |
		Vector2f duv1 = uv2 - uv1;
		Vector2f duv2 = uv3 - uv1;
		Float det = duv1.x * duv2.y - duv1.y * duv2.x;
		if (det == 0) {
			//degenerate
			CoordinateSystem(Cross(e1, e2), dpdu, dpdv);
		}
		else {
			Float invDet = 1 / det;
			dpdu = (duv2.y * e1 - duv2.x * e2) * invDet;
			dpdv = (-duv1.y * e1 + duv1.x * e2) * invDet;
		}

		//alpha mask texture exists?
		if (mesh->alphaMask) {
			Intersection localIsect;
			localIsect.uv = uv;
			localIsect.dpdu = dpdu;
			localIsect.dpdv = dpdv;
			Float alpha = mesh->alphaMask->Evaluate(localIsect).X();
			if (alpha < Epsilon) {
				//if alpha < Epsilon, discard
				return false;
			}
		}

		isect.p = ray(tt);
		isect.n = Normalize(n1 * (1.f - b1 - b2) + n2 * b1 + n3 * b2);
		isect.uv = uv;
		isect.dpdu = dpdu;
		isect.dpdv = dpdv;
		isect.bsdf = bsdf;
		isect.light = light;

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

		//calculate UV
		Vector2f uv = uv1 * (1.f - b1 - b2) + uv2 * b1 + uv3 * b2;
		Vector3f dpdu, dpdv;
		//calculate partial differential
		//p = pi + dpdu*ui + dpdv*vi
		//| p2 - p1 |   | u2 - u1  v2 - v1 | | dpdu |
		//|         | = |                  | |      |
		//| p3 - p1 |   | u3 - u1  v3 - v1 | | dpdv |
		Vector2f duv1 = uv2 - uv1;
		Vector2f duv2 = uv3 - uv1;
		Float det = duv1.x * duv2.y - duv1.y * duv2.x;
		if (det == 0) {
			//degenerate
			CoordinateSystem(Cross(e1, e2), dpdu, dpdv);
		}
		else {
			Float invDet = 1 / det;
			dpdu = (duv2.y * e1 - duv2.x * e2) * invDet;
			dpdv = (-duv1.y * e1 + duv1.x * e2) * invDet;
		}

		//alpha mask texture exists?
		if (mesh->alphaMask) {
			Intersection localIsect;
			localIsect.uv = uv;
			localIsect.dpdu = dpdu;
			localIsect.dpdv = dpdv;
			Float alpha = mesh->alphaMask->Evaluate(localIsect).X();
			if (alpha < Epsilon) {
				//if alpha < Epsilon, discard
				return false;
			}
		}

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
		}

		return ret;
	}
}