#ifndef MESH_H
#define MESH_H

#include "Vertex.h"
#include "MTLLoader.h"

template <typename VertexType>
class Mesh {
public:
	using VertType = VertexType;

	std::vector <VertexType> vertexList; 
	std::vector <Material> materials;
	std::vector <int> materialIndex;
	std::vector <std::vector<int>> elementIndex;

	Mesh() {
		materialIndex.push_back(0);
	}

	Material getMaterialByIndex (int index) {
		if (index >= 0 && index < materials.size())
			return materials[index];
		else
			return Material();
	}

	int getVertCount() {
		return vertexList.size();
	}

	void addVertex (VertexType vertex) {
		vertexList.emplace_back(vertex);
	}

	friend std::ostream& operator << (std::ostream& stream, Mesh& arg) {
		for (auto&& vert : arg.vertexList)
			stream << vert << std::endl;

		for (auto&& polygon : arg.elementIndex) {
			for (auto&& index : polygon) {
				stream << index << " ";
			}
			stream << std::endl;
		}

		return stream;
	}
};

#endif