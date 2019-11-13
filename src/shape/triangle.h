#pragma once

#include "../core/shape.h"
#include "../core/texture.h"

namespace pol {
	struct TriangleMesh {
		vector<Vector3f> p;
		vector<Vector3f> n;
		vector<Vector2f> uv;
		vector<int> indices;
		Texture* alphaMask;
		bool hasTexcoord;
		//bounding box of  triangle mesh
		BBox bbox;

		TriangleMesh(const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
			const vector<Vector2f>& uv, const vector<int>& indices, Bsdf* bsdf, Texture* alphaMask = nullptr);
	
		string ToString() const;
	};

	class Triangle : public Shape {
	private:
		const shared_ptr<TriangleMesh> mesh;
		int faceIndex;
	public:
		Triangle(const Transform& world, Bsdf* bsdf, const shared_ptr<TriangleMesh>& mesh, int faceIndex);

		virtual Float SurfaceArea() const;
		virtual BBox WorldBBox() const;
		virtual bool Intersect(Ray& ray, Intersection& isect) const;
		virtual bool Occluded(const Ray& ray) const;
		virtual void SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const;
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const;

		//return a human-readable string summary
		virtual string ToString() const;
	};

	vector<Triangle*> CreateTriangleMeshShape(const Transform& world, const vector<Vector3f>& p, const vector<Vector3f>& n,
		const vector<Vector2f>& uv, const vector<int>& indices, Bsdf* bsdf, Texture* alphaMask = nullptr);
}