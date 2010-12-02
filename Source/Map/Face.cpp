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

    if (isDirectionPositive(DirectionType))
    {
        InitPositiveFace(DirectionType);
    }
    else
    {
        InitNegativeFace(DirectionType);
    }

	MaterialTypeID = INVALID_INDEX;
	PositionNode = NewPosition;
}

void Face::InitPositiveFace(Direction DirectionType)
{
    Ogre::Entity* ent1;

    if (DirectionType == DIRECTION_UP)
    {
        ent1 = RENDERER->getSceneManager()->createEntity("UpTile");
    }
    if (DirectionType == DIRECTION_NORTH)
    {
        ent1 = RENDERER->getSceneManager()->createEntity("NorthTile");
    }
    if (DirectionType == DIRECTION_EAST)
    {
        ent1 = RENDERER->getSceneManager()->createEntity("EastTile");
    }

    OgreEntityPositive = ent1;
    OgreEntityPositive->setCastShadows(false);
    PositionNode->attachObject(OgreEntityPositive);
}

void Face::InitNegativeFace(Direction DirectionType)
{
    Ogre::Entity* ent2;

    if (DirectionType == DIRECTION_DOWN)
    {
        ent2 = RENDERER->getSceneManager()->createEntity("DownTile");
    }
    if (DirectionType == DIRECTION_SOUTH)
    {
        ent2 = RENDERER->getSceneManager()->createEntity("SouthTile");
    }
    if (DirectionType == DIRECTION_WEST)
    {
        ent2 = RENDERER->getSceneManager()->createEntity("WestTile");
    }

    OgreEntityNegative = ent2;
    OgreEntityNegative->setCastShadows(false);
    PositionNode->attachObject(OgreEntityNegative);
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
	if (NewMaterialTypeID != MaterialTypeID)  // Restet both Entities
	{
		if (PositiveAxisSurfaceTypeID != INVALID_INDEX)
		{
		    if (OgreEntityPositive == NULL)
		    {
		        //InitPositiveFace(FaceAxis);
		    }

			OgreEntityPositive->setMaterial(TEXTURE->getOgreMaterial(NewMaterialTypeID, PositiveAxisSurfaceTypeID));
		}
		if (NegativeAxisSurfaceTypeID != INVALID_INDEX)
		{
            if (OgreEntityNegative == NULL)
		    {
		        //InitNegativeFace(FaceAxis);
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

				OgreEntityPositive->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, PositiveAxisSurfaceTypeID));
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

				OgreEntityNegative->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, NegativeAxisSurfaceTypeID));
			}
		}
	}
}
