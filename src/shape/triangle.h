#pragma once

#include "../core/shape.h"
#include "../core/texture.h"

namespace pol {
	class Triangle;
	class TriangleMesh : public PolObject {
	public:
		vector<Vector3f> p;
		vector<Vector3f> n;
		vector<Vector2f> uv;
		vector<int> indices;
		Texture* alphaMask;
		bool hasTexcoord;
		//bounding box of  triangle mesh
		BBox bbox;

		//temporary storage
		//once the SetLight function has been called,
		//triangles will be all cleared
		vector<Triangle*> triangles;

	public:
		TriangleMesh(const PropSets& props, Scene& scene);

		string ToString() const;
	};

	class Triangle : public Shape {
	private:
		const shared_ptr<TriangleMesh> mesh;
		int faceIndex;
	public:
		Triangle(const PropSets& props, Scene& scene, const shared_ptr<TriangleMesh>& mesh, int faceIndex);

		virtual Float SurfaceArea() const;
		virtual BBox WorldBBox() const;
		virtual bool Intersect(Ray& ray, Intersection& isect) const;
		virtual bool Occluded(const Ray& ray) const;
		virtual void SampleShape(const Vector2f& u, Vector3f& pos, Vector3f& nor, Float& pdf, bool& solidAngle) const;
		virtual Float Pdf(const Vector3f& pOnLight, const Vector3f& pOnSurface, bool& solidAngle) const;

		//return a human-readable string summary
		virtual string ToString() const;
	};
}