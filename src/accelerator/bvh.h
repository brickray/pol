#pragma once

#include "../core/accelerator.h"

namespace pol {
	class Bvh : public Accelerator {
	public:
		struct BvhNode {
			vector<Shape*> primitives;
			BBox bbox;
		//	left node is current idx plus one;
		//  left = curIdx + 1
			int right;
			int axis;
			bool isleaf;

			BvhNode()
				:right(-1), isleaf(false) {

			}
		};

	private:
		vector<BvhNode*> linearNodes;

	public:
		Bvh();

		virtual BBox GetRootBBox() const {
			return linearNodes[0]->bbox;
		}

		virtual int GetNodesCount() const{
			return linearNodes.size();
		}

		virtual bool Build(const vector<Shape*>& primitives);
		virtual bool Intersect(Ray& ray, Intersection& isect) const;
		virtual bool Occluded(const Ray& ray) const;

		virtual string ToString() const;

	private:
		void split(const vector<Shape*>& primitives, const BBox& bbox, int& nextFree);
		BvhNode* createLeaf(const vector<Shape*>& primitives, const BBox& bbox) const;
	};

	Bvh* CreateBvhAccelerator();
}