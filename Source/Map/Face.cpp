#include <Face.h>


#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>


Face::Face(Ogre::SceneNode* NewPosition, Direction DirectionType)
{
	PositiveAxisSurfaceTypeID = INVALID_INDEX;
	NegativeAxisSurfaceTypeID = INVALID_INDEX;

	OgreEntityPositive = NULL;
	OgreEntityNegative = NULL;

	PositionNode = NewPosition;

    FaceAxis = AxisFromDirection(DirectionType);

    // TODO use 6 different Tiles fore each Axial Direction all centered onthe cube center

	MaterialTypeID = INVALID_INDEX;
	PositionNode = NewPosition;
}

void Face::InitPositiveFace(Direction DirectionType)
{
    if (DirectionType == DIRECTION_UP)
    {
        OgreEntityPositive = RENDERER->getSceneManager()->createEntity("UpTile");
        OgreEntityPositive->setCastShadows(false);
        PositionNode->attachObject(OgreEntityPositive);
    }
    if (DirectionType == DIRECTION_NORTH)
    {
        OgreEntityPositive = RENDERER->getSceneManager()->createEntity("NorthTile");
        OgreEntityPositive->setCastShadows(false);
        PositionNode->attachObject(OgreEntityPositive);
    }
    if (DirectionType == DIRECTION_EAST)
    {
        OgreEntityPositive = RENDERER->getSceneManager()->createEntity("EastTile");
        OgreEntityPositive->setCastShadows(false);
        PositionNode->attachObject(OgreEntityPositive);
    }
}

void Face::InitNegativeFace(Direction DirectionType)
{
    if (DirectionType == DIRECTION_DOWN)
    {
        OgreEntityNegative = RENDERER->getSceneManager()->createEntity("DownTile");
        OgreEntityNegative->setCastShadows(false);
        PositionNode->attachObject(OgreEntityNegative);
    }
    if (DirectionType == DIRECTION_SOUTH)
    {
        OgreEntityNegative = RENDERER->getSceneManager()->createEntity("SouthTile");
        OgreEntityNegative->setCastShadows(false);
        PositionNode->attachObject(OgreEntityNegative);
    }
    if (DirectionType == DIRECTION_WEST)
    {
        OgreEntityNegative = RENDERER->getSceneManager()->createEntity("WestTile");
        OgreEntityNegative->setCastShadows(false);
        PositionNode->attachObject(OgreEntityNegative);
    }
}

Face::~Face()
{
    PositionNode->detachAllObjects();

    if (OgreEntityPositive != NULL)
    {
        RENDERER->getSceneManager()->destroyMovableObject(OgreEntityPositive);
    }
    if (OgreEntityNegative != NULL)
    {
        RENDERER->getSceneManager()->destroyMovableObject(OgreEntityNegative);
    }

    RENDERER->getSceneManager()->destroySceneNode(PositionNode);
}

void Face::setFaceMaterialType(int16_t NewMaterialTypeID)
{
	if (NewMaterialTypeID != MaterialTypeID)  // Reset both Entities
	{
		if (PositiveAxisSurfaceTypeID != INVALID_INDEX)
		{
		    if (OgreEntityPositive == NULL)
		    {
		        InitPositiveFace(DirectionFromAxis(FaceAxis, true));
		    }

			OgreEntityPositive->setMaterial(TEXTURE->getOgreMaterial(NewMaterialTypeID, PositiveAxisSurfaceTypeID));
		}
		if (NegativeAxisSurfaceTypeID != INVALID_INDEX)
		{
            if (OgreEntityNegative == NULL)
		    {
		        InitNegativeFace(DirectionFromAxis(FaceAxis, false));
		    }

			OgreEntityNegative->setMaterial(TEXTURE->getOgreMaterial(NewMaterialTypeID, NegativeAxisSurfaceTypeID));
		}
		MaterialTypeID = NewMaterialTypeID;
	}
}

void Face::setFaceSurfaceType(int16_t NewSurfaceTypeID, Direction DirectionType)
{
	if (isDirectionPositive(DirectionType))
	{
		if (NewSurfaceTypeID != PositiveAxisSurfaceTypeID)  // Restet Positive only
		{
			PositiveAxisSurfaceTypeID = NewSurfaceTypeID;
			if (MaterialTypeID != INVALID_INDEX)
			{
                if (OgreEntityPositive == NULL)
                {
                    InitPositiveFace(DirectionType);
                }

                if (OgreEntityPositive != NULL)
                {
                    OgreEntityPositive->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, PositiveAxisSurfaceTypeID));
                }
			}
		}
	}
	else
	{
		if (NewSurfaceTypeID != NegativeAxisSurfaceTypeID)  // Restet Negative only
		{
			NegativeAxisSurfaceTypeID = NewSurfaceTypeID;
			if (MaterialTypeID != INVALID_INDEX)
			{
                if (OgreEntityNegative == NULL)
                {
                    InitNegativeFace(DirectionType);
                }
                if (OgreEntityNegative != NULL)
                {
                    OgreEntityNegative->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, NegativeAxisSurfaceTypeID));
                }
			}
		}
	}
}
