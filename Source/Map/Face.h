#ifndef FACE__HEADER
#define FACE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <TileShapes.h>
#include <Ogre.h>

class Face
{
public:

	Face(Ogre::SceneNode* CellSceneNode, CubeCoordinates TargetCoordinates, Direction DirectionType);
	~Face();

	void setFaceMaterialType(int16_t NewMaterialTypeID);
	void setFaceSurfaceType(int16_t NewSurfaceTypeID);
	void setFaceShapeType(FaceShape NewShape);

    void RefreshEntity(Ogre::SceneNode* CellNode);

    int16_t getFaceMaterialType()           { return MaterialTypeID; }
    int16_t getFaceSurfaceType()            { return SurfaceTypeID; }
    FaceShape getFaceShapeType()            { return FaceType; }

private:

    FaceShape FaceType;
    CubeCoordinates LocationCoordinates;

    int16_t SurfaceTypeID;
    int16_t MaterialTypeID;
};

#endif // FACE__HEADER
