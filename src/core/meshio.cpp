#include "meshio.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tinyobjloader/tiny_obj_loader.h"

#include <fstream>
#include <string>

//https://github.com/kopaka1822/ObjWriter
namespace objwriter
{
	template<class TVertex, class TIndex, class TString, class TStream>
	class ObjWriterT
	{
	public:
		explicit ObjWriterT(TStream& stream) :
			m_file(stream)
		{}

		void comment(const TString& comment)
		{
			m_file << "# " << comment << std::endl;
		}
		void materialLib(const TString& materialLib)
		{
			m_file << "mtllib " << materialLib << std::endl;
		}
		void objectName(const TString& objName)
		{
			m_file << "o " << objName << std::endl;
		}
		void vertex(TVertex v1, TVertex v2, TVertex v3)
		{
			m_file << "v "
				<< v1 << " "
				<< v2 << " "
				<< v3 << std::endl;
		}
		void vertex(TVertex v1, TVertex v2, TVertex v3, TVertex v4)
		{
			m_file << "v "
				<< v1 << " "
				<< v2 << " "
				<< v3 << " "
				<< v4 << std::endl;
		}
		void texcoord(TVertex t1, TVertex t2)
		{
			m_file << "vt "
				<< t1 << " "
				<< t2 << std::endl;
		}
		void texcoord(TVertex t1, TVertex t2, TVertex t3)
		{
			m_file << "vt "
				<< t1 << " "
				<< t2 << " "
				<< t3 << std::endl;
		}
		void normal(TVertex n1, TVertex n2, TVertex n3)
		{
			m_file << "vn "
				<< n1 << " "
				<< n2 << " "
				<< n3 << std::endl;
		}
		void group(const TString& groupName)
		{
			m_file << "g " << groupName << std::endl;
		}
		void material(const TString& materialName)
		{
			m_file << "usemtl " << materialName << std::endl;
		}

		/**
		 * \param level smoothing level between 1 and 32
		 */
		void smoothLevel(int level)
		{
			m_file << "s " << level << std::endl;
		}
		void smoothOff()
		{
			m_file << "s off" << std::endl;
		}
		void face(TIndex i1, TIndex i2, TIndex i3)
		{
			m_file << "f "
				<< i1 << " "
				<< i2 << " "
				<< i3 << std::endl;
		}
		// vertex + normal indices
		void face_vn(TIndex i1, TIndex n1, TIndex i2, TIndex n2, TIndex i3, TIndex n3)
		{
			m_file << "f "
				<< i1 << "//" << n1 << " "
				<< i2 << "//" << n2 << " "
				<< i3 << "//" << n3 << std::endl;
		}
		// vertex + texture indices
		void face_vt(TIndex i1, TIndex t1, TIndex i2, TIndex t2, TIndex i3, TIndex t3)
		{
			m_file << "f "
				<< i1 << "/" << t1 << " "
				<< i2 << "/" << t2 << " "
				<< i3 << "/" << t3 << std::endl;
		}
		// vertex + texture + normal indices
		void face_vtn(TIndex i1, TIndex t1, TIndex n1, TIndex i2, TIndex t2, TIndex n2, TIndex i3, TIndex t3, TIndex n3)
		{
			m_file << "f "
				<< i1 << "/" << t1 << "/" << n1 << " "
				<< i2 << "/" << t2 << "/" << n2 << " "
				<< i3 << "/" << t3 << "/" << n3 << std::endl;
		}
		void parameterVertex(TVertex u, TVertex v)
		{
			m_file << "vp "
				<< u << " "
				<< v << std::endl;
		}
		void parameterVertex(TVertex u, TVertex v, TVertex w)
		{
			m_file << "vp "
				<< u << " "
				<< v << " "
				<< w << std::endl;
		}
	private:
		TStream& m_file;
	};

	using ObjWriter = ObjWriterT<float, int, std::string, std::fstream>;
}

namespace pol {
	void MeshIO::LoadModelFromFile(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, string filename, unsigned int flags) {
		int nPos = filename.find_last_of('.');
		string extension = filename.substr(nPos + 1);
		if (extension != ".obj") {
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename, flags);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				fprintf(stderr, "Error when improt model: %s\n", importer.GetErrorString());
				exit(1);
			}

			processNode(p, n, uv, indices, scene->mRootNode, scene);
		}
		else {
			loadObj(p, n, uv, indices, filename);
		}
		fprintf(stdout, "Load Model sucessfully: %s\n", filename.c_str());
		fprintf(stdout, "Merge [%d] triangles\n", indices.size() / 3);
	}

	void MeshIO::WriteObj(const vector<Vector3f>& p, const vector<Vector3f>& n, const vector<Vector2f>& uv, const vector<int>& indices, string filename) {
		fstream file;
		file.open(filename.c_str(), ios::out);
		objwriter::ObjWriter w(file);
		for (int v = 0; v < p.size(); ++v) {
			Vector3f vert = p[v];
			w.vertex(vert.X(), vert.Y(), vert.Z());
		}

		for (int i = 0; i < indices.size(); i += 3) {
			Vector3i face = Vector3i(indices[i], indices[i + 1], indices[i + 2]);
			w.face(face.x + 1, face.y + 1, face.z + 1);
		}

		file.close();
	}

	void MeshIO::loadObj(vector<Vector3f>& p, vector<Vector3f>& n, vector<Vector2f>& uv, vector<int>& indices, string filename) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());

		if (!warn.empty()) {
			std::cout << warn << std::endl;
		}

		if (!err.empty()) {
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

		int nVertices = attrib.vertices.size() / 3;
		p.resize(nVertices);
		for (size_t v = 0; v < nVertices; ++v) {
			p[v] = Vector3f(attrib.vertices[3 * v], attrib.vertices[3 * v + 1], attrib.vertices[3 * v + 2]);
		}

		int nNormals = attrib.normals.size() / 3;
		n.resize(nNormals);
		for (size_t v = 0; v < nNormals; ++v) {
			n[v] = Vector3f(attrib.normals[3 * v], attrib.normals[3 * v + 1], attrib.normals[3 * v + 2]);
		}

		if (attrib.texcoords.size() > 0) {
			int nUVs = attrib.texcoords.size() / 2;
			uv.resize(nUVs);
			for (size_t v = 0; v < nUVs; ++v) {
				uv[v] = Vector2f(attrib.texcoords[2 * v], attrib.texcoords[2 * v + 1]);
			}
		}

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			for (size_t f = 0; f < shapes[s].mesh.indices.size(); ++f) {
				indices.push_back(shapes[s].mesh.indices[f].vertex_index);
			}
		}
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
			vertex[0] = aimesh->mVertices[i].x;
			vertex[1] = aimesh->mVertices[i].y;
			vertex[2] = aimesh->mVertices[i].z;
			normal[0] = aimesh->mNormals[i].x;
			normal[1] = aimesh->mNormals[i].y;
			normal[2] = aimesh->mNormals[i].z;
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