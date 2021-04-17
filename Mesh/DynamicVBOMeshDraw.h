#ifndef DYNAMIC_VBO_MESH_DRAW_H
#define DYNAMIC_VBO_MESH_DRAW_H

#include "Mesh.h"

// template <int ElementType = DynamicVBOMeshDraw::TRIANGLE>
class DynamicVBOMeshDraw {
public:

	static const int INDEX_INVALID = -1;

	static const int POINT = 1;
	static const int LINE = 2;
	static const int TRIANGLE = 3;
	static const int QUAD = 4;

	int vao = INDEX_INVALID;

	int vertexVBO = INDEX_INVALID;

	int indexPointVBO = INDEX_INVALID;
	int indexLineVBO = INDEX_INVALID;
	int indexTriangleVBO = INDEX_INVALID;
	int indexQuadVBO = INDEX_INVALID;

	int pointCount = 0;
	int lineCount = 0;
	int triangleCount = 0;
	int quadCount = 0;

	bool isFree = true;
	
	DynamicVBOMeshDraw() {};

	template <typename VertType>
	DynamicVBOMeshDraw (Mesh<VertType>& mesh) {
		init(mesh);
	};

	DynamicVBOMeshDraw (const DynamicVBOMeshDraw& other) = delete;
	
	DynamicVBOMeshDraw (DynamicVBOMeshDraw&& other) {
		movOp(other);
	}
	
	DynamicVBOMeshDraw& operator = (const DynamicVBOMeshDraw& other) = delete;
	
	DynamicVBOMeshDraw& operator = (DynamicVBOMeshDraw&& other) {
		return movOp(other);
	}

	DynamicVBOMeshDraw& movOp (DynamicVBOMeshDraw& other) {
		vao = other.vao;
		vertexVBO = other.vertexVBO;
		indexPointVBO = other.indexPointVBO;
		indexLineVBO = other.indexLineVBO;
		indexTriangleVBO = other.indexTriangleVBO;
		indexQuadVBO = other.indexQuadVBO;

		pointCount = other.pointCount;
		lineCount = other.lineCount;
		triangleCount = other.triangleCount;
		quadCount = other.quadCount;
		isFree = false;
		other.isFree = true;

		return *this;
	}

	template <typename VertType>
	void init (Mesh<VertType>& mesh) {
		if (mesh.vertexList.size() == 0)
			return;

		if (mesh.elementIndex.size() == 0)
			return;

		isFree = false;

		std::vector<int> pointElemnts;
		std::vector<int> lineElemnts;
		std::vector<int> triangleElemnts;
		std::vector<int> quadElemnts;

		glGenVertexArrays(1, (GLuint*)&vao);
		glBindVertexArray(vao);

		for (int i = 0; i < mesh.elementIndex.size(); i++) {
			auto& face = mesh.elementIndex[i];

			if (face.size() == 1) {
				pointCount++;
				for (auto&& index : face) {
					pointElemnts.push_back(index);
				}
			}

			if (face.size() == 2) {
				lineCount++;
				for (auto&& index : face) {
					lineElemnts.push_back(index);
				}
			}

			if (face.size() == 3) {
				triangleCount++;
				for (auto&& index : face) {
					triangleElemnts.push_back(index);
				}
			}

			if (face.size() == 4) {
				quadCount++;
				for (auto&& index : face) {
					quadElemnts.push_back(index);
				}
			}
		}

		auto storeElements = [] (int &indexVBO, std::vector<int>& buffer) {
			if (buffer.size() == 0) {
				indexVBO = INDEX_INVALID;
				return false;
			}

			glGenBuffers(1, (GLuint*)&indexVBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size() * sizeof(int),
					&(buffer[0]), GL_DYNAMIC_DRAW);	
		};

		storeElements(indexPointVBO, pointElemnts);
		storeElements(indexLineVBO, lineElemnts);
		storeElements(indexTriangleVBO, triangleElemnts);
		storeElements(indexQuadVBO, quadElemnts);

		glGenBuffers(1, (GLuint*)&vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertexList.size() * sizeof(mesh.vertexList[0]),
				&(mesh.vertexList[0]), GL_DYNAMIC_DRAW);

		char *baseAddr = (char *)&(mesh.vertexList[0]);

		if constexpr (VertType::template has_desc<VertexPosition>()) {
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexPosition>()) - baseAddr));
		}

		if constexpr (VertType::template has_desc<VertexNormal>()) {
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexNormal>()) - baseAddr));
		}

		if constexpr (VertType::template has_desc<VertexTexCoord>()) {
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexTexCoord>()) - baseAddr));
		}

		if constexpr (VertType::template has_desc<VertexColor>()) {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, sizeof(mesh.vertexList[0]),
					(void *)((char *)&(mesh.vertexList[0].template get<VertexColor>()) - baseAddr));
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	template <typename VertType>
	void updateVert (Mesh<VertType>& mesh, int start, int size) {
		if (mesh.vertexList.size() <= start)
			return;

		if (mesh.elementIndex.size() <= start)
			return;


		// std::cout << "start " << start << " size " << size << std::endl;
		// std::cout << mesh.vertexList[start] << std::endl;
		// std::cout << "this " << mesh.vertexList.size() - start << std::endl;

		// std::cout << size << std::endl;

		// std::cout << "offset: " << sizeof(mesh.vertexList[start]) * start << std::endl;
		// std::cout << "size: " << std::min((int)mesh.vertexList.size() - start, size) 
		// 		* sizeof(mesh.vertexList[start]) << std::endl;
		// std::cout << "ptrdiff: " << &(mesh.vertexList[start]) - &(mesh.vertexList[0]) << std::endl;
		// std::cout << "ptr: " << &(mesh.vertexList[start]) << std::endl;

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);

		// int bufferSize = 0;
		// glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		// std::cout << bufferSize << std::endl;

		glBufferSubData(GL_ARRAY_BUFFER, 
				sizeof(mesh.vertexList[start]) * start,
				std::min((int)mesh.vertexList.size() - start, size) * sizeof(mesh.vertexList[start]),
				&(mesh.vertexList[start]));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	template <typename VertType>
	void updateElem (Mesh<VertType>& mesh, int start, int size, int elementType) {
		if (mesh.vertexList.size() <= start)
			return;

		if (mesh.elementIndex.size() <= start)
			return;

		std::vector<int> pointElemnts;
		std::vector<int> lineElemnts;
		std::vector<int> triangleElemnts;
		std::vector<int> quadElemnts;

		glBindVertexArray(vao);

		for (int i = start; i < mesh.elementIndex.size() && i < start + size; i++) {
			auto& face = mesh.elementIndex[i];

			if (face.size() == 1) {
				pointCount++;
				for (auto&& index : face) {
					pointElemnts.push_back(index);
				}
			}

			if (face.size() == 2) {
				lineCount++;
				for (auto&& index : face) {
					lineElemnts.push_back(index);
				}
			}

			if (face.size() == 3) {
				triangleCount++;
				for (auto&& index : face) {
					triangleElemnts.push_back(index);
				}
			}

			if (face.size() == 4) {
				quadCount++;
				for (auto&& index : face) {
					quadElemnts.push_back(index);
				}
			}
		}

		auto updateElements = [=] (int &indexVBO, std::vector<int>& buffer, int start, int size) {
			if (buffer.size() == 0) {
				indexVBO = INDEX_INVALID;
				return false;
			}

			auto chosenLenght = [&]() {
				switch (elementType) {
					case POINT: 	return pointCount 		* elementType;
					case LINE: 		return lineCount 		* elementType;
					case TRIANGLE: 	return triangleCount 	* elementType;
					case QUAD: 		return quadCount 		* elementType;
						default: return 0;
				}
			};

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 
					start * elementType * sizeof(int),
					std::min(chosenLenght(), (int)buffer.size()) * sizeof(int),
					&(buffer[0]));	
		};

		if (elementType == POINT)
			updateElements(indexPointVBO, pointElemnts, start, size);

		if (elementType == LINE);
			updateElements(indexLineVBO, lineElemnts, start, size);
		
		if (elementType == TRIANGLE);
			updateElements(indexTriangleVBO, triangleElemnts, start, size);

		if (elementType == QUAD)
			updateElements(indexQuadVBO, quadElemnts, start, size);

		glBindVertexArray(0);
	}

	void draw(ShaderProgram& shader) {
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		
		if (indexPointVBO != INDEX_INVALID) {
			glDrawElements(GL_POINTS, pointCount * 1, GL_UNSIGNED_INT, (char*)NULL + 0);
		}

		if (indexLineVBO != INDEX_INVALID) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexLineVBO);
			glDrawElements(GL_LINES, lineCount * 2, GL_UNSIGNED_INT, (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		if (indexTriangleVBO != INDEX_INVALID) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexTriangleVBO);
			glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		if (indexQuadVBO != INDEX_INVALID) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuadVBO);
			glDrawElements(GL_QUADS, quadCount * 4, GL_UNSIGNED_INT, (char*)NULL + 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void freeMem() {
		glDeleteBuffers(1, (GLuint*)&vertexVBO);

		if (indexPointVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexPointVBO);
		
		if (indexLineVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexLineVBO);
		
		if (indexTriangleVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexTriangleVBO);

		if (indexQuadVBO != INDEX_INVALID)
			glDeleteBuffers(1, (GLuint*)&indexQuadVBO);
	}

	~DynamicVBOMeshDraw() {
		if (!isFree)
			freeMem();
	}
};

#endif