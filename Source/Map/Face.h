#ifndef FACE__HEADER
#define FACE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>

class Face
{
public:

	Face(Ogre::SceneNode* NewPosition, Direction DirectionType);
	~Face();

	void setFaceMaterialType(int16_t NewMaterialTypeID);
	void setFaceSurfaceType(int16_t NewSurfaceTypeID, Direction DirectionType);

    void InitPositiveFace(Direction DirectionType);
    void InitNegativeFace(Direction DirectionType);

    int16_t getFaceMaterialType()           { return MaterialTypeID; }
    int16_t getPositiveAxisSurfateType()    { return PositiveAxisSurfaceTypeID; }
    int16_t getNegativeAxisSurfaceType()    { return NegativeAxisSurfaceTypeID; }

private:

    Axis FaceAxis;

    int16_t PositiveAxisSurfaceTypeID;
    int16_t NegativeAxisSurfaceTypeID;

    int16_t MaterialTypeID;

    Ogre::Entity* OgreEntityPositive;
    Ogre::Entity* OgreEntityNegative;

    Ogre::SceneNode* PositionNode;
};

#endif // FACE__HEADER
