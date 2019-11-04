#pragma once

#include "../pol.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Assimp;

namespace pol {
	class MeshIO {
	public:
		static void LoadModelFromFile(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, string filename, unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	private:
		static void processNode(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, aiNode* node, const aiScene* scene);
		static void processMesh(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, aiMesh* aimesh, const aiScene* scene);
		static Vector3f genTangent(int idx1, int idx2, int idx3);
	};
}