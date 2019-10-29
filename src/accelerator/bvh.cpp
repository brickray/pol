#include "bvh.h"
#include "../core/shape.h"

namespace pol {
	Bvh::Bvh() {

	}

	bool Bvh::Build(const vector<Shape*>& primitives) {
		if (primitives.size() == 0) {
			//some log info

			return false;
		}

		BBox rootBBox;
		for (const Shape* shape : primitives) {
			rootBBox.Union(shape->WorldBBox());
		}

		int nextFree = 0;
		split(primitives, rootBBox, nextFree);

		return true;
	}

	Bvh::BvhNode* Bvh::createLeaf(const vector<Shape*>& primitives, const BBox& bbox) const {
		BvhNode* leaf = new BvhNode();
		leaf->isleaf = true;
		leaf->bbox = bbox;
		leaf->primitives.reserve(primitives.size());
		for (int i = 0; i < primitives.size(); ++i)
			leaf->primitives.push_back(primitives[i]);

		return leaf;
	}

	void Bvh::split(const vector<Shape*>& primitives, const BBox& bbox, int& nextFree) {
		//create a leaf if the number of primitives is small
		if (primitives.size() < 4) {
			linearNodes.push_back(createLeaf(primitives, bbox));

			return;
		}

		const int bucketSize = 12;
		struct Bucket {
			BBox bbox;
			int count;
			
			Bucket() {
				count = 0;
			}
		};

		int bestAxis = -1;
		int bestBucket;
		Float bestCost = 2;
		Vector3f diagonal = bbox.Diagonal();
		for (int axis = 0; axis < 3; ++axis) {
			Bucket bucket[bucketSize];

			Float len = diagonal[axis];
			for (const Shape* shape : primitives) {
				BBox shapeBBox = shape->WorldBBox();
				Float center = shapeBBox.Center()[axis];
				int idx = (center - bbox.fmin[axis]) / len * bucketSize;
				idx = idx == bucketSize ? idx - 1 : idx;
				//add shape to corresponding bucket
				bucket[idx].bbox.Union(shapeBBox);
				bucket[idx].count++;
			}

			//
			for (int i = 1; i < bucketSize - 1; ++i) {
				BBox left, right;
				int countLeft = 0, countRight = 0;
				for (int j = 0; j < i; ++j) {
					left.Union(bucket[j].bbox);
					countLeft += bucket[j].count;
				}

				for (int j = i; j < bucketSize; ++j) {
					right.Union(bucket[j].bbox);
					countRight += bucket[j].count;
				}

				//calc cost
				Float cost = 1 + (left.SurfaceArea() + right.SurfaceArea()) / bbox.SurfaceArea();
				if (cost < bestCost) {
					bestAxis = axis;
					bestCost = cost;
					bestBucket = i;
				}
			}
		}

		//can not find axis to split, then just create leaf
		if (bestAxis == -1) {
			linearNodes.push_back(createLeaf(primitives, bbox));
			
			return;
		}

		vector<Shape*> left;
		vector<Shape*> right;
		BBox leftBBox;
		BBox rightBBox;
		Float len = bbox.Diagonal()[bestAxis];
		for (Shape* shape : primitives) {
			BBox shapeBBox = shape->WorldBBox();
			Float center = shapeBBox.Center()[bestAxis];
			int idx = (center - bbox.fmin[bestAxis]) / len;
			idx = idx == bucketSize ? idx - 1 : idx;

			if (idx < bestBucket) {
				left.push_back(shape);
				leftBBox.Union(shapeBBox);
			}
			else {
				right.push_back(shape);
				rightBBox.Union(shapeBBox);
			}
		}

		//create node
		BvhNode* node = new BvhNode();
		node->bbox = bbox;
		linearNodes.push_back(node);
		nextFree++;
		split(left, leftBBox, nextFree);
		node->right = nextFree++;
		split(right, rightBBox, nextFree);
	}

	bool Bvh::Intersect(Ray& ray, Intersection& isect) const {
		int stack[64];
		int stackTop = 0;
		int nodeIdx = 0;
		stack[stackTop++] = 0;
		bool intersect = false;

		while (true) {
			if (!stackTop) break;

			nodeIdx = stack[--stackTop];
			BvhNode* node = linearNodes[nodeIdx];
			if (node->bbox.Intersect(ray)) {
				if (node->isleaf) {
					for (const Shape* shape : node->primitives) {
						intersect |= shape->Intersect(ray, isect);
					}
				}
				else {
					stack[stackTop++] = node->right;
					stack[stackTop++] = nodeIdx + 1;
				}
			}
		}

		return intersect;
	}

	bool Bvh::Occluded(const Ray& ray) const {
		int stack[64];
		int stackTop = 0;
		int nodeIdx = 0;
		stack[stackTop++] = 0;
		bool intersect = false;

		while (true) {
			if (!stackTop) break;

			nodeIdx = stack[--stackTop];
			BvhNode* node = linearNodes[nodeIdx];
			if (node->bbox.Intersect(ray)) {
				if (node->isleaf) {
					for (const Shape* shape : node->primitives) {
						if (shape->Occluded(ray)) return true;
					}
				}
				else {
					stack[stackTop++] = node->right;
					stack[stackTop++] = nodeIdx + 1;
				}
			}
		}

		return false;
	}

	string Bvh::ToString() const {
		string ret;
		ret += "Bvh[\n bbox = " + indent(GetRootBBox().ToString())
			+ ",\n  nodeCount = " + to_string(GetNodesCount())
            + "\n]";

		return ret;
	}

	Bvh* CreateBvhAccelerator() {
		return new Bvh();
	}
}