#pragma once

#include "../pol.h"
#include "intersection.h"

namespace pol {
	//non power of 2 mipmap generation
	//https://www.nvidia.com/object/np2_mipmapping.html
	//
	//two rules for the sequence of level widths are
	//Rounding Down: wid[1] = floor(width>>1), maxLevel = floor(log2(width))
	//Rounding Up: wid[1] = ceil(width>>1), maxLevel = ceil(log2(width))
	//Examples:
	//base width = 127, rounding down:   127, 63, 31, 15, 7, 3, 1
	//base width = 127, rounding up:     127, 64, 32, 16, 8, 4, 2, 1
	//if the base width happens to be a power of 2, rounding up and down yield identical
	//results. In general, the scale factor between successive levels no longer 2, Rounding up
	//is generally preferable because less information is lost between levels, But current 
	//D3D and OpenGL APIs support rouding down only.
	//here we use Rounding Down method
	//For the rounding down setting, we observe the following behavior
	//1. For position x in the current mipmap level, we always sample 2*x, 2*x+1. 2*x+2 in the previous level.
	//2. The first sample always ends up on the upward slope
	//3. The second sample always in the flat center area.
	//4. The third sample always ends up on the downward slope
	//5. The sample points move with a speed of -1/n along the filter kernel
	enum class WrapMode {
		E_REPEAT,
		E_CLAMP,
		E_MIRROR,
	};

	enum class FilterMode {
		E_NEARST,
		E_LINEAR,  //bilinear
		E_TRILINEAR, //trilinear(mipmap)
	};

	struct TexInfo {
		int w, h;
		Vector3f* data;
	};
	class Mipmap {
	private:
		vector<TexInfo> pyramid;
		WrapMode wmode;
		FilterMode fmode;
		
	public:
		Mipmap();
		Mipmap(int w, int h, const vector<Vector3f>& data, FilterMode fmode = FilterMode::E_TRILINEAR, WrapMode wmode = WrapMode::E_REPEAT);
		~Mipmap();

		int PyramidCount() const;
		vector<TexInfo> GetPyramid() const;
		void Build(int w, int h, const vector<Vector3f>& data, FilterMode fmode = FilterMode::E_TRILINEAR, WrapMode wmode = WrapMode::E_REPEAT);
		Vector3f Lookup(const Intersection& isect) const;

		string ToString() const;

	private:
		Vector2f getTexCoordinate(const Vector2f& uv) const;
		Vector3f triangle(int level, const Vector2f& uv) const;
	};
}
