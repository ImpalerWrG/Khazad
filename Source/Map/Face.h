#ifndef FACE__HEADER
#define FACE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <TileShapes.h>
#include <Ogre.h>

class Face
{
public:

	Face(Ogre::SceneNode* CellSceneNode, CubeCoordinates TargetCoordinates);
	~Face();

	void setFaceMaterialType(int16_t NewMaterialTypeID);
	void setFaceSurfaceType(int16_t NewSurfaceTypeID);
	void setShapeType(TileShape NewShape);

    void RefreshEntity();

    int16_t getFaceMaterialType()           { return MaterialTypeID; }
    int16_t getFaceSurfaceType()            { return SurfaceTypeID; }
    TileShape getFaceShapeType()            { return ShapeType; }

private:

    TileShape ShapeType;

    Ogre::SceneNode* CellNode;
    CubeCoordinates LocationCoordinates;

    int16_t SurfaceTypeID;
    int16_t MaterialTypeID;
};

#endif // FACE__HEADER
