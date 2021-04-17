#ifndef TANK_UNIT_H
#define TANK_UNIT_H

#include "Unit.h"

class TankUnit : public Unit {
public:
	// will have a static mesh
	// and other props

	using TankVertexType = Vertex<
		Math::Point3f,	VertexPosition,
		Math::Point3f,	VertexNormal,
		Math::Point4f,	VertexColor,
		Math::Point2f,	VertexTexCoord
	>;

	TankUnit (int player = 0, int type = 0)
	: Unit(player, type)
	{}

	Mesh<TankVertexType> createModel() {
		Mesh<TankVertexType> tank;
		Util::addCube(tank, 3, Math::Vec4f(0.44, 0.9, 0.1, 1));
		return tank;
	}

	DeprecatedVBOMeshDraw& getModel() {
		// static Mesh<TankVertexType> mTank = 
		// 		OBJLoader<TankVertexType>().loadMesh(
		// 			"Obj/TankNoTex/", "T-90.obj");
		static Mesh<TankVertexType> mTank = createModel();
		static DeprecatedVBOMeshDraw gTank;

		if (gTank.isFree) {
			gTank = DeprecatedVBOMeshDraw(mTank);
		}

		return gTank;
	}

	virtual void render (DrawContext& drawContext,
			ShaderProgram &defaultShader)
	{
		defaultShader.useProgram();
		defaultShader.setMatrix("projectionMatrix", drawContext.proj);
		defaultShader.setMatrix("viewMatrix", drawContext.view);
		defaultShader.setMatrix("worldMatrix", 
				Math::translation<float>(pos) *
				drawContext.world * Math::scale4<float>(4, 4, 4));
		glColor4fv(
			Math::Point4f(
				player % 2,
				(player / 2) % 2,
				(player / 4) % 2,
				1
			).getPtr()
		);

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		getModel().draw(defaultShader);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
};

#endif