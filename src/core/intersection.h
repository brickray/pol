#pragma once

#include "../pol.h"
#include "frame.h"
#include "ray.h"

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
		//differential of intersect
		Vector3f dpdu, dpdv;
		Float dudx, dudy;
		Float dvdx, dvdy;
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
			:bsdf(nullptr), light(nullptr)
			, dudx(0), dudy(0)
			, dvdx(0), dvdy(0) {

		}

		__forceinline void operator()(const Transform& transform) {
			//here p and n are already transformed
			dpdu = transform.TransformVector(dpdu);
			dpdv = transform.TransformVector(dpdv);
		}

		//(x - p).n = 0
		//x.n = p.n
		//x = o + t*d
		//n.o + t*n.d = p.n
		//t = (p.n - n.o)/n.d
		__forceinline void ComputeDifferentials(const RayDifferential& ray) {
			do {
				//compute auxiliary intersection points with plane
				Float d = Dot(n, p);
				Float tx = (d - Dot(n, ray.rxOrigin)) / Dot(n, ray.rxDirection);
				Float ty = (d - Dot(n, ray.ryOrigin)) / Dot(n, ray.ryDirection);
				if (isinf(tx) || isinf(ty)) break;;
				Vector3f px = ray.rxOrigin + tx * ray.rxDirection;
				Vector3f py = ray.ryOrigin + ty * ray.ryDirection;
				Vector3f dpdx = px - p;
				Vector3f dpdy = py - p;

				//solve p' = p + dpdu*u + dpdv*v
				//let P = p' - p
				// | P.x |   | dpdu.x  dpdv.x || u |
				// | P.y | = | dpdu.y  dpdv.y ||   |
				// | P.z |   | dpdu.z  dpdv.z || v |
				Matrix2 A(dpdu[0], dpdv[0], dpdu[1], dpdv[1]);
				Vector2f Bx(px[0] - p[0], px[1] - p[1]);
				Vector2f By(py[0] - p[0], py[1] - p[1]);
				if (!SolveLinearSystem2x2(A, Bx, dudx, dvdx)) {
					dudx = 0;
					dvdx = 0;
				}
				if (!SolveLinearSystem2x2(A, By, dudy, dvdy)) {
					dudy = 0;
					dvdy = 0;
				}

				//uv mapping
				/*Float su = 5, sv = 5;
				uv = Vector2f(su * uv.x, sv * uv.y);
				dudx *= su;
				dvdx *= sv;
				dudy *= su;
				dvdy *= su;*/
				return;
			} while (false);

			dudx = dudy = 0;
			dvdx = dvdy = 0;
		}
	};
}