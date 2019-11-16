#pragma once

#include "../pol.h"

namespace pol {
	struct RenderBlock {
		int sx, sy; //block start with(sx, sy)
		int w, h; //block end with(sx + w, sy + h)
	};

	class Scene;
	void InitRenderBlock(const Scene& scene, vector<RenderBlock>& rbs);
}