#ifndef MESH_TOOLS_H
#define MESH_TOOLS_H

#include "Mesh.h"
#include "MathLib.h"

namespace Util
{
	template <typename VertType>
	void addVert (Mesh<VertType>& mesh,
			Math::Vec3f pos,
			Math::Vec4f color,
			Math::Vec3f normal = Math::Vec3f(0, 0, 0),
			Math::Vec2f tex = Math::Vec2f(0, 0))
	{
		VertType newVert;

		newVert.template setIfExists<VertexPosition>(pos);
		newVert.template setIfExists<VertexColor>(color);
		newVert.template setIfExists<VertexNormal>(normal);
		newVert.template setIfExists<VertexTexCoord>(tex);
		mesh.addVertex(newVert);
	}

	template <typename VertType>
	void addPoint (Mesh<VertType>& mesh,
			Math::Vec3f A,
			Math::Vec4f color = Math::Vec4f(1, 1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>()) 
	{
		int index = mesh.getVertCount();

		addVert(mesh, Math::trunc<Math::Vec3f>(transf * A), color);

		mesh.elementIndex.push_back(std::vector<int>{index + 0});
	}

	/// transf is additional transformation to the object to be added
	template <typename VertType>
	void addLine (Mesh<VertType>& mesh,
			Math::Vec3f A,
			Math::Vec3f B, 
			Math::Vec4f color = Math::Vec4f(1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>())
	{
		using namespace Math;
		int index = mesh.getVertCount();

		addVert(mesh, trunc<Vec3f>(transf * Point4f(A, 1)), color);
		addVert(mesh, trunc<Vec3f>(transf * Point4f(B, 1)), color);

		mesh.elementIndex.push_back(std::vector<int>{index + 0, index + 1});
	}

	template <typename VertType>
	void addTriangle (Mesh<VertType>& mesh,
			Math::Vec3f A,
			Math::Vec3f B,
			Math::Vec3f C, 
			Math::Vec4f color = Math::Vec4f(1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>(),
			Math::Vec3f normA = Math::Vec3f(0, 0, 1),
			Math::Vec3f normB = Math::Vec3f(0, 0, 1),
			Math::Vec3f normC = Math::Vec3f(0, 0, 1))
	{
		int index = mesh.getVertCount();
	
		addVert(mesh, Math::trunc<Math::Vec3f>(transf * Math::Vec4f(A, 1)),
				color, Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normA)));
		
		addVert(mesh, Math::trunc<Math::Vec3f>(transf * Math::Vec4f(B, 1)),
				color, Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normB)));
		
		addVert(mesh, Math::trunc<Math::Vec3f>(transf * Math::Vec4f(C, 1)),
				color, Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normC)));
	
		mesh.elementIndex.push_back(std::vector<int>{index + 0, index + 1, index + 2});
	}

	// 2d surfaces will all be on xy, z will be perpendicular on them
	template <typename VertType>
	void addSquare (Mesh<VertType>& mesh,
			float side,
			Math::Vec4f color = Math::Vec4f(1, 1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>(),
			Math::Vec3f normal = Math::Vec3f(0, 0, 1)) 
	{
		int index = mesh.getVertCount();

		addVert(mesh,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f(-side, -side, 0, 1)),
				color,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normal)),
				Math::Vec2f(0, 0));
	
		addVert(mesh,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f( side, -side, 0, 1)),
				color,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normal)),
				Math::Vec2f(0, 1));
	
		addVert(mesh,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f( side,  side, 0, 1)),
				color,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normal)),
				Math::Vec2f(1, 1));
	
		addVert(mesh,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f(-side,  side, 0, 1)),
				color,
				Math::trunc<Math::Vec3f>(transf * Math::Vec4f(normal)),
				Math::Vec2f(1, 0));

		mesh.elementIndex.push_back(std::vector<int>{
				index + 0, index + 1, index + 2, index + 3});
	}

	template <typename VertType>
	void addSquareW (Mesh<VertType>& mesh,
			float side,
			Math::Vec4f color = Math::Vec4f(1, 1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>())
	{
		using namespace Math;

		addLine(mesh, Point3f(-1, -1, 0) * side,
				Point3f(1, -1, 0) * side, color, transf);
		addLine(mesh, Point3f(1, -1, 0) * side,
				Point3f(1, 1, 0) * side, color, transf);
		addLine(mesh, Point3f(1, 1, 0) * side,
				Point3f(-1, 1, 0) * side, color, transf);
		addLine(mesh, Point3f(-1, 1, 0) * side,
				Point3f(-1, -1, 0) * side, color, transf);
	}

	template <typename VertType>
	void addCircle (Mesh<VertType>& mesh,
			float radius,
			int complex,
			Math::Vec4f color = Math::Vec4f(1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>())
	{
		float pi = 3.141592653;
		float div = 2 * pi / complex;
		for (float alpha = 0; alpha < pi * 2 - div / 2; alpha += div) {
			addLine(
				mesh,
				Math::Vec3f(cos(alpha), sin(alpha), 0),
				Math::Vec3f(cos(alpha + div), sin(alpha + div), 0),
				color, 
				transf
			);
		}
	}

	template <typename VertType>
	void addCube (Mesh<VertType>& mesh,
			float side, 
			Math::Vec4f color = Math::Vec4f(1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>())
	{
		Math::Mat4f transform = Math::translation<float>(0, 0, side);
		
		addSquare(mesh, side, color, transf * transform);
		addSquare(mesh, side, color, transf * Math::rot4<float>( 180, 1, 0, 0) * transform);
		addSquare(mesh, side, color, transf * Math::rot4<float>( 180 / 2, 1, 0, 0) * transform);
		addSquare(mesh, side, color, transf * Math::rot4<float>(-180 / 2, 1, 0, 0) * transform);
		addSquare(mesh, side, color, transf * Math::rot4<float>( 180 / 2, 0, 1, 0) * transform);
		addSquare(mesh, side, color, transf * Math::rot4<float>(-180 / 2, 0, 1, 0) * transform);
	}

	template <typename VertType>
	void addSphere (Mesh<VertType>& mesh,
			float radius,
			int complex, 
			Math::Vec4f color = Math::Vec4f(1, 1, 1, 1),
			Math::Mat4f transf = Math::identity<4, float>()) 
	{
		float pi = 3.141592653;
		float div = 2 * pi / complex;
		for (float alpha = 0; alpha < pi - div / 2; alpha += div) {
 			for (float beta = 0; beta < pi * 2 - div / 2; beta += div) {
				addTriangle(
					mesh,
					Math::Vec3f(	cos(alpha) * cos(beta), 
									sin(alpha) * cos(beta), 
									sin(beta)) * radius,
					Math::Vec3f(	cos(alpha + div) * cos(beta), 
									sin(alpha + div) * cos(beta), 
									sin(beta)) * radius,
					Math::Vec3f(	cos(alpha + div) * cos(beta + div), 
									sin(alpha + div) * cos(beta + div), 
									sin(beta + div)) * radius,
					color, 
					transf, 
					Math::Vec3f(	cos(alpha) * cos(beta), 
									sin(alpha) * cos(beta), 
									sin(beta)),
					Math::Vec3f(	cos(alpha + div) * cos(beta), 
									sin(alpha + div) * cos(beta), 
									sin(beta)),
					Math::Vec3f(	cos(alpha + div) * cos(beta + div), 
									sin(alpha + div) * cos(beta + div), 
									sin(beta + div))
				);

				addTriangle(
					mesh,
					Math::Vec3f(	cos(alpha + div) * cos(beta + div), 
									sin(alpha + div) * cos(beta + div), 
									sin(beta + div)) * radius,
					Math::Vec3f(	cos(alpha) * cos(beta + div), 
									sin(alpha) * cos(beta + div), 
									sin(beta + div)) * radius,
					Math::Vec3f(	cos(alpha) * cos(beta), 
									sin(alpha) * cos(beta), 
									sin(beta)) * radius,
					color, 
					transf,
					Math::Vec3f(	cos(alpha + div) * cos(beta + div), 
									sin(alpha + div) * cos(beta + div), 
									sin(beta + div)),
					Math::Vec3f(	cos(alpha) * cos(beta + div), 
									sin(alpha) * cos(beta + div), 
									sin(beta + div)),
					Math::Vec3f(	cos(alpha) * cos(beta), 
									sin(alpha) * cos(beta), 
									sin(beta))
				);
			}
		}
	}
}

#endif