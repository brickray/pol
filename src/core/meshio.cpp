#include "meshio.h"

namespace pol {
	void MeshIO::LoadModelFromFile(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, string filename, unsigned int flags) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, flags);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			fprintf(stderr, "Error when improt model: %s\n", importer.GetErrorString());
			exit(1);
		}

		processNode(p, n, uv, indices, scene->mRootNode, scene);
		fprintf(stdout, "Load Model sucessfully: %s\n", filename.c_str());
		fprintf(stdout, "Merge [%d] triangles\n", indices.size() / 3);
	}

	void MeshIO::processNode(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, aiNode* node, const aiScene* scene) {
		for (int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(p, n, uv, indices, aimesh, scene);
		}

		for (int i = 0; i < node->mNumChildren; ++i) {
			processNode(p, n, uv, indices, node->mChildren[i], scene);
		}
	}

	void MeshIO::processMesh(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, aiMesh* aimesh, const aiScene* scene) {
		for (int i = 0; i < aimesh->mNumVertices; ++i) {
			Vector3f vertex, normal;
			Vector2f texcoord;
			vertex.x = aimesh->mVertices[i].x;
			vertex.y = aimesh->mVertices[i].y;
			vertex.z = aimesh->mVertices[i].z;
			normal.x = aimesh->mNormals[i].x;
			normal.y = aimesh->mNormals[i].y;
			normal.z = aimesh->mNormals[i].z;
			if (aimesh->mTextureCoords[0]) {// have tex coordinate
				texcoord.x = aimesh->mTextureCoords[0][i].x;
				texcoord.y = aimesh->mTextureCoords[0][i].y;
			}
			else {
				texcoord.x = texcoord.y = 0;
			}

			p.push_back(vertex);
			n.push_back(normal);
			uv.push_back(texcoord);
		}

		for (int i = 0; i < aimesh->mNumFaces; ++i) {
			aiFace face = aimesh->mFaces[i];
			int idx1 = face.mIndices[0];
			int idx2 = face.mIndices[1];
			int idx3 = face.mIndices[2];

			indices.push_back(idx1);
			indices.push_back(idx2);
			indices.push_back(idx3);
		}
	}

	Vector3f MeshIO::genTangent(int idx1, int idx2, int idx3) {
		return Vector3f();
	}
}