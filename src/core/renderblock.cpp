#include "renderblock.h"
#include "scene.h"

namespace pol {
	void InitRenderBlock(const Scene& scene, vector < RenderBlock>& rbs) {
		Film* film = scene.GetCamera()->GetFilm();
		int screenWidth = film->res.x;
		int screenHeight = film->res.y;

#define BLOCK_SIZE 32
		int nw = (screenWidth + BLOCK_SIZE - 1) / BLOCK_SIZE;
		int nh = (screenHeight + BLOCK_SIZE - 1) / BLOCK_SIZE;
		//reserve vector for performance purpose
		rbs.reserve(nw * nh);
		//rendering start from mid of screen
		int startX = nw / 2;
		int startY = nh / 2;
		//define direction clockwise by default
		int dir[2] = { 0, 0 };
		int offsetDir = 0;
		int loop = 1, curLoop = 0;
		//calc the sort of render block
		int offsetX[4] = { 1, 0, -1, 0 };
		int offsetY[4] = { 0, -1, 0, 1 };
		int tasks = 1;
		//first push the mid block
		RenderBlock render_block = {
			startX * BLOCK_SIZE,
			startY * BLOCK_SIZE,
			BLOCK_SIZE,
			BLOCK_SIZE
		};
		rbs.push_back(render_block);

		//loop
		while (tasks < nw * nh) {
			startX = startX + offsetX[offsetDir];
			startY = startY + offsetY[offsetDir];

			//update
			if (++curLoop >= loop) {
				offsetDir = (++offsetDir) % 4;
				if (offsetDir == 0 || offsetDir == 2) {
					++loop;
				}

				curLoop = 0;
			}


			if (startX < 0 || startX >= nw || startY < 0 || startY >= nh)
				continue;
			RenderBlock render_block = {
				startX * BLOCK_SIZE,
				startY * BLOCK_SIZE,
				startX * BLOCK_SIZE + BLOCK_SIZE > screenWidth ? screenWidth - startX * BLOCK_SIZE : BLOCK_SIZE,
				startY* BLOCK_SIZE + BLOCK_SIZE > screenHeight ? screenHeight - startY * BLOCK_SIZE : BLOCK_SIZE
			};

			//push render block
			rbs.push_back(render_block);

			//increase tasks
			++tasks;
		}
	}
}