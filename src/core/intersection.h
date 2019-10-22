#pragma once

#include "../pol.h"
#include "frame.h"

namespace pol {
	class Bsdf;
	class Light;
	class Intersection {
	public:
		//point of intersect
		Vector3f p; 
		//normal of intersect
		Vector3f n; 
		//texcoord of intersect
		Vector2f uv; 
		//bsdf of intersect
		Bsdf* bsdf;
		//intersect shape is light?
		Light* light;
		//shading frame
		Frame shFrame;
		//geometry frame
		Frame geoFrame;

	public:
		Intersection()
			:bsdf(nullptr), light(nullptr) {

		}
	};
}