#ifndef FIXEDFUNCTIONMESHDRAW_H_INCLUDED
#define FIXEDFUNCTIONMESHDRAW_H_INCLUDED

#include "Mesh.h"
#include "MathLib.h"

class FixedFunctionMeshDraw {
public:
	FixedFunctionMeshDraw() {}

	template <typename VertexType>
	static void drawVertex (VertexType& vertex) {
		vertex.template useIfExists<VertexColor>([](auto& color) {
				glColor4fv(color.getPtr());
			}
		);

		vertex.template useIfExists<VertexNormal>([](auto& normal) {
				glNormal3fv(normal.getPtr());
			}
		);

		vertex.template useIfExists<VertexTexCoord>([](auto& texCoord) {
				glTexCoord2fv(texCoord.getPtr());
			}
		);

		vertex.template useIfExists<VertexPosition>([](auto& position) {
				glVertex3fv(position.getPtr());
			}
		);
	}

	static void useMaterial (Material material, ShaderProgram& shader) {
		glColor4fv(material.difuseColor.getPtr());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.difuseTexture.openglTexture);

		shader.setInt("texture", 0);
	} 

	template <typename VertexType>
	static void draw (Mesh<VertexType>& mesh, ShaderProgram& shader) {
		int currentMaterial = -2; 
		for (int i = 0; i < mesh.elementIndex.size(); i++) {
			auto& face = mesh.elementIndex[i];
			
			if (currentMaterial != mesh.materialIndex[i]){
				currentMaterial = mesh.materialIndex[i];

				useMaterial(mesh.getMaterialByIndex(currentMaterial), shader); 
			}

			if (face.size() == 0) {
				; // draw nothing
			}
			else if (face.size() == 1) {
				// draw a point
				glBegin(GL_POINTS); 
					for (auto&& index : face) 
						drawVertex(mesh.vertexList[index]);
				glEnd(); 
			}
			else if (face.size() == 2) {
				// draw a point
				glBegin(GL_LINES); 
					for (auto&& index : face) 
						drawVertex(mesh.vertexList[index]);
				glEnd(); 
			}
			else if (face.size() == 3) {
				// draw a point
				glBegin(GL_TRIANGLES); 
					for (auto&& index : face) 
						drawVertex(mesh.vertexList[index]);
				glEnd(); 
			}
			else if (face.size() == 4) {
				// draw a point
				glBegin(GL_QUADS); 
					for (auto&& index : face) {
						drawVertex(mesh.vertexList[index]);
					}
				glEnd(); 
			}
			else {
				glBegin(GL_POLYGON);
					for (auto&& index : face) 
						drawVertex(mesh.vertexList[index]);
				glEnd();	
			}
		}
	}
};


#endif