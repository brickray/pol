#include "deformable.h"
#include "triangle.h"
#include "../core/memory.h"
#include "../core/meshio.h"

namespace pol {
	//         _______             ______________
	//        /\     /\           /\            /\
	//       /___\_/___\        /    \        /    \ 
	//       \   / \   /      /        \    /        \
	//        \/_____\/     /____________\/____________\ 
	struct HalfEdgeVertex;
	struct HalfEdgeFace;
	struct HalfEdgeEdge {
		//same edge, but opposite direction
		HalfEdgeEdge* pair = nullptr;
		//next edge, belongs to same face when it is not boundary edge
		HalfEdgeEdge* next = nullptr;
		//vertex at the base of the half edge
		HalfEdgeVertex* vertex = nullptr;
		//the face that half edge belongs to
		HalfEdgeFace* face = nullptr;
		//true if edge is boundary
		bool boundary = false;

		//return true if edge is boundary
		__forceinline bool IsBoundary() const {
			return boundary;
		}

		string ToString() const;
	};

	struct HalfEdgeVertex {
		//out going edge
		HalfEdgeEdge* outgoingEdge = nullptr;
		//vertex
		Vector3f p;
		Vector3f newP;

		HalfEdgeVertex(const Vector3f& p)
			:p(p) {

		}

		//return true if vertex is boundary vertex
		__forceinline bool IsBoundary() const {
			return outgoingEdge->IsBoundary();
		}

		//in the interior of a triangle mesh, most vertices are adjacent to six faces and have six
		//neighbor vertices directly connected to them with edges.On the boundaries of an open mesh,
		//most vertices are adjacent to three faces and four vertices.The number of vertices directly 
		//adjacent to a vertex is called the vertex's VALENCE. Interior vertices with valence other than
		//six, or boundary vertices with valence other than four, are called extraordinary vertices, 
		//otherwise, they are called regular.
		__forceinline int Valence() const {
			HalfEdgeEdge* startEdge = outgoingEdge->pair;
			HalfEdgeEdge* e = startEdge;
			int valence = 1;
			bool boundary = IsBoundary();
			//compute valence of the vertex
			//always true regardless the vertex is (interior or boundary)
			while (e->next->pair != startEdge) {
				valence++;
				e = e->next->pair;
			}

			return valence;
		}

		string ToString() const {
			string ret;

			ret += "HalfEdgeVertex[\n  vertex = " + p.ToString()
				+ ",\n  valence = " + to_string(Valence())
				+ ",\n  boundary = " + (IsBoundary() ? "true" : "false")
				+ "\n]";

			return ret;
		}
	};

	string HalfEdgeEdge::ToString() const {
		string ret;
		ret += "HalfEdgeEdge[\n  start = " + vertex->p.ToString()
			+ ",\n  end = " + pair->vertex->p.ToString()
			+ ",\n  boundary = " + (boundary ? "true" : "false")
			+ "\n]";

		return ret;
	}

	struct HalfEdgeFace {
		//one of the half-edges bordering the face
		HalfEdgeEdge* edge = nullptr;

		//return next face
		//return nullptr if edge is boundary
		//counterwise clock
		__forceinline HalfEdgeFace* NextFace(const HalfEdgeEdge* e) const {
			if (edge->pair->IsBoundary()) return nullptr;

			return edge->pair->face;
		}
		
		//return face's vertices
		__forceinline vector<Vector3f> AllVertices() const {
			vector<Vector3f> vertices;
			vertices.push_back(edge->vertex->p);

			HalfEdgeEdge* e = edge;
			while (e->next != edge) {
				e = e->next;
				vertices.push_back(e->vertex->p);
			}

			return vertices;
		}

		string ToString() const {
			string ret;
			ret += "HalfEdgeFace[\n  edge1 = " + indent(edge->ToString())
				+ ",\n  edge2 = " + indent(edge->next->ToString())
				+ ",\n  edge3 = " + indent(edge->next->next->ToString())
				+ "\n]";

			return ret;
		}
	};

	class HalfEdgeMesh {
	public:
		HalfEdgeVertex* verts;
		int nVerts;
		HalfEdgeFace* faces;
		int nFaces;
		vector<HalfEdgeEdge*> edges;

		void Clear() {
			FreeAligned(verts);
			FreeAligned(faces);
			for (HalfEdgeEdge* e : edges) {
				delete e;
			}
		}

		string ToString() const {
			string ret;
			ret += "HalfEdgeMesh[\n  nVertices = " + to_string(nVerts)
				+ ",\n  nEdges = " + to_string(edges.size())
				+ ",\n  nFaces = " + to_string(nFaces)
				+ "\n]";

			return ret;
		}
	};

#define NEXT(i) ((i + 1) % 3)
#define PREV(i) ((i - 1) % 3)
#define INDEX(f, v) (3 * f + v)
	//transform mesh form polygon soup to half edge
	HalfEdgeMesh* PolygonSoupToHalfEdge(const vector<Vector3f>& p, const vector<int>& indices) {
		//the input must describe a manifold, oriented surface, where the orientation
		// of a polygon is determined by the order of vertices in the list. Polygons
		// must have three vertices
		HalfEdgeMesh* mesh = new HalfEdgeMesh();

		typedef pair<int, int> Index;
		typedef map<Index, HalfEdgeEdge*> EdgeMap;
		typedef map<Index, HalfEdgeEdge*>::iterator EMIter;
		EdgeMap edgeMap;

		int nVertices = p.size();
		mesh->nVerts = nVertices;
		mesh->verts = AllocAligned<HalfEdgeVertex>(sizeof(HalfEdgeVertex) * nVertices);
		for (int i = 0; i < nVertices; ++i) {
			mesh->verts[i] = HalfEdgeVertex(p[i]);
		}

		int nFaces = indices.size() / 3;
		mesh->nFaces = nFaces;
		mesh->faces = AllocAligned<HalfEdgeFace>(sizeof(HalfEdgeFace) * nFaces);

		for (int f = 0; f < nFaces; ++f) {
			HalfEdgeEdge* halfEdges[3];
			//loop over halfedges of this face
			for (int v = 0; v < 3; ++v) {
				//get index of vertex
				int a = indices[INDEX(f, v)];
				int b = indices[INDEX(f, NEXT(v))];
				Index ab(a, b);
				HalfEdgeEdge* hab = nullptr;
				if (edgeMap.find(ab) != edgeMap.end()) {
					//fatal error, ext
					exit(1);
				}
				else {
					hab = new HalfEdgeEdge();
					mesh->edges.push_back(hab);
					if (v == 0) {
						//initialize  face
						mesh->faces[f].edge = hab;
					}
					edgeMap[ab] = hab;
					hab->vertex = &mesh->verts[a];
					hab->vertex->outgoingEdge = hab;
					hab->face = &mesh->faces[f];

					// keep a list of halfedges in this face, so that we can later
					// link them together in a loop (via their "next" pointers)
					halfEdges[v] = hab;
				}

				//find pairs if exists
				Index ba(b, a);
				if (edgeMap.find(ba) != edgeMap.end()) {
					HalfEdgeEdge* hba = edgeMap[ba];
					hab->pair = hba;
					hba->pair = hab;
				}
				else {
					//if we didn't find pairs.
					hab->pair = nullptr;
				}
			}

			//now we can link them together
			for (int e = 0; e < 3; ++e) {
				halfEdges[e]->next = halfEdges[NEXT(e)];
			}
		}

		//for each vertex on the boundary, advance its halfedge pointer to one that
		//is also on the boundary.
		for (int v = 0; v < nVertices; ++v) {
			HalfEdgeEdge* start = mesh->verts[v].outgoingEdge;
			HalfEdgeEdge* edge = start;
			do {
				if (edge->pair == nullptr) {
					mesh->verts[v].outgoingEdge = edge;
					break;
				}

				edge = edge->pair->next;
			} while (edge != start);
		}

		//next we construct new faces for each boundary component.
		for (EMIter iter = edgeMap.begin(); iter != edgeMap.end(); ++iter) {
			vector<HalfEdgeEdge*> boundaryEdge;
			HalfEdgeEdge* start = iter->second;
			if (start->pair) continue;

			HalfEdgeEdge* edge = start;

			do {
				HalfEdgeEdge* twins = new HalfEdgeEdge();
				mesh->edges.push_back(twins);
				boundaryEdge.push_back(twins);
				edge->pair = twins;
				edge->boundary = true;
				twins->pair = edge;
				twins->boundary = true;
				twins->vertex = edge->next->vertex;

				//loop for find next boundary edge if exist
				edge = edge->next;
				while (edge->pair && edge != start) {
					edge = edge->pair->next;
				}

			} while (edge != start);

			int size = boundaryEdge.size();
			for (int e = 0; e < size; ++e) {
				int p = (e - 1 + size) % size;
				boundaryEdge[e]->next = boundaryEdge[p];
			}
		}

		//pointer to boundary edge
		for (int v = 0; v < nVertices; ++v) {
			mesh->verts[v].outgoingEdge = mesh->verts[v].outgoingEdge->pair->next;
		}

		return mesh;
	}

	__forceinline Float Beta(int valence) {
		Float a = 0.375 + 0.25 * cos(TWOPI / valence);
		return 1.0 / valence * (0.625 - (a * a));
	}

	__forceinline Float LoopGamma(int valence) {
		return Float(1) / (valence + Float(3) / (8 * Beta(valence)));
	}

	__forceinline Vector3f NewInteriorVertex(HalfEdgeVertex* vertex) {
		Vector3f p;
		Float valence = vertex->Valence();
		Float beta = Beta(valence);
		p += (1 - valence * beta) * vertex->p;

		HalfEdgeEdge* start = vertex->outgoingEdge;
		p += beta * start->next->vertex->p;

		HalfEdgeEdge* e = start->pair->next;
		while (e != start) {
			p += beta * e->next->vertex->p;
			e = e->pair->next;
		}

		return p;
	}

	__forceinline Vector3f NewBoundaryVertex(HalfEdgeVertex* vertex) {
		Vector3f p;
		Float valence = vertex->Valence();
		Float beta = 0.125/*1 / 8*/;
		p += (1 - 2 * beta) * vertex->p;
		
		HalfEdgeEdge* start = vertex->outgoingEdge;
		p += beta * start->next->vertex->p;

		HalfEdgeEdge* e = start->pair->next;
		while (!e->IsBoundary()) {
			e = e->pair->next;
		}
		p += beta * e->next->vertex->p;

		return p;
	}

	bool CreateSubDivisionShape(int level, TriangleMesh* mesh) {
		vector<Vector3f> lp = mesh->p;
		vector<int> li = mesh->indices;

		map<HalfEdgeVertex*, int> subDivHEV;
		vector<Vector3f> subDivVertices;
		vector<int> subDivIndices;
		for (int i = 0; i < level; ++i) {
			subDivHEV.clear();
			subDivVertices.clear();
			subDivIndices.clear();
			//transform polygon soup to half edge
			HalfEdgeMesh* mesh = PolygonSoupToHalfEdge(lp, li);

			vector<HalfEdgeVertex*> vertices(mesh->nVerts);
			for (int v = 0; v < mesh->nVerts; ++v)
				vertices[v] = &mesh->verts[v];
			vector<HalfEdgeFace*> faces(mesh->nFaces);
			for (int f = 0; f < mesh->nFaces; ++f)
				faces[f] = &mesh->faces[f];
			int nEdges = mesh->edges.size() / 2;

			int nLastVertices = vertices.size();
			int nNewVertices = nEdges;
			int nFaces = faces.size();
			vector<HalfEdgeVertex*> newVertices(nLastVertices + nNewVertices);
			HalfEdgeVertex* verts = AllocAligned<HalfEdgeVertex>(nLastVertices + nNewVertices);
			for (int v = 0; v < nLastVertices; ++v) {
				verts[v] = *vertices[v];
				newVertices[v] = &verts[v];
			}
			for (int v = nLastVertices; v < nLastVertices + nNewVertices; ++v) {
				newVertices[v] = &verts[v];
			}

			//	vector<HalfEdgeFace*> newFaces(nFaces * 4);

			for (int v = 0; v < nLastVertices; ++v) {
				HalfEdgeVertex* vert = vertices[v];
				if (!vert->IsBoundary()) {
					//beta = 1 / 16 when vertex is regular
					vertices[v]->newP = NewInteriorVertex(vert);
				}
				else {
					vertices[v]->newP = NewBoundaryVertex(vert);
				}
			}

			//generante new vertex and update new mesh topology
			int vertsPointer = nLastVertices;
			int facesPointer = 0;
			map<HalfEdgeEdge*, HalfEdgeVertex*> edges;
			for (int f = 0; f < nFaces; ++f) {
				HalfEdgeFace* face = faces[f];
				vector<HalfEdgeVertex*> edgeVertices;
				//first edge
				HalfEdgeEdge* start = face->edge;
				HalfEdgeEdge* e = start;
				do {
					HalfEdgeVertex* vert;
					if (edges.find(e->pair) == edges.end()) {
						vert = newVertices[vertsPointer++];
						Vector3f p0 = e->vertex->p;
						Vector3f p1 = e->next->vertex->p;
						if (e->IsBoundary()) {
							vert->p = Float(0.5) * (p0 + p1);
							vert->newP = vert->p;
						}
						else {
							Vector3f p2 = e->next->next->vertex->p;
							Vector3f p3 = e->pair->next->next->vertex->p;
							vert->p = Float(0.375/*3 / 8*/) * (p0 + p1) +
								Float(0.125/*1 / 8*/) * (p2 + p3);
							vert->newP = vert->p;
						}

						edges[e] = vert;
					}
					else {
						vert = edges[e->pair];
					}

					edgeVertices.push_back(vert);

					e = e->next;
				} while (e != start);

				//update new mesh topology

				int index[6];
				HalfEdgeVertex* v[6];
				v[0] = face->edge->vertex; v[1] = face->edge->next->vertex; v[2] = face->edge->next->next->vertex;
				v[3] = edgeVertices[0]; v[4] = edgeVertices[1]; v[5] = edgeVertices[2];
				for (int n = 0; n < 6; ++n) {
					if (subDivHEV.find(v[n]) != subDivHEV.end()) {
						index[n] = subDivHEV[v[n]];
						continue;
					}

					subDivHEV[v[n]] = subDivHEV.size();
					subDivVertices.push_back(v[n]->newP);
					index[n] = subDivHEV[v[n]];
				}
				subDivIndices.push_back(index[0]); subDivIndices.push_back(index[3]); subDivIndices.push_back(index[5]);
				subDivIndices.push_back(index[3]); subDivIndices.push_back(index[1]); subDivIndices.push_back(index[4]);
				subDivIndices.push_back(index[5]); subDivIndices.push_back(index[4]); subDivIndices.push_back(index[2]);
				subDivIndices.push_back(index[3]); subDivIndices.push_back(index[4]); subDivIndices.push_back(index[5]);
			}

			lp = subDivVertices;
			li = subDivIndices;

			FreeAligned(verts);
			mesh->Clear();
		}

	//	MeshIO::WriteObj(subDivVertices, vector<Vector3f>(), vector<Vector2f>(), subDivIndices, "../t.obj");

	//	printf("vertices size = %d,face size = %d\n", subDivVertices.size(), subDivIndices.size() / 3);

		mesh->p = subDivVertices;
		mesh->indices = subDivIndices;

		return true;
	}
}