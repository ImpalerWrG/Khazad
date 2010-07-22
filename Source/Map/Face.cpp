#include <Face.h>


#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>


Face::Face(Ogre::SceneNode* NewPosition, Direction DirectionType)
{
	PositiveAxisSurfaceTypeID = INVALID_INDEX;
	NegativeAxisSurfaceTypeID = INVALID_INDEX;
	PositionNode = NewPosition;


    // TODO use 6 different Tiles fore each Axial Direction all centered onthe cube center

    if (isDirectionPositive(DirectionType))
    {
        Ogre::Entity *ent1 = RENDERER->getSceneManager()->createEntity("DownTile");
        OgreEntityPositive = ent1;
        OgreEntityPositive->setCastShadows(false);
        PositionNode->attachObject(OgreEntityPositive);
    }
    else
    {
        Ogre::Entity *ent2 = RENDERER->getSceneManager()->createEntity("DownTile");
        OgreEntityNegative = ent2;
        OgreEntityNegative->setCastShadows(false);
        PositionNode->attachObject(OgreEntityNegative);
    }

	MaterialTypeID = INVALID_INDEX;
	PositionNode = NewPosition;
}

Face::~Face()
{
	delete OgreEntityPositive;
	delete OgreEntityNegative;
	delete PositionNode;
}

void Face::setFaceMaterialType(int16_t NewMaterialTypeID)
{
	if (NewMaterialTypeID != MaterialTypeID)  // Restet both Entities
	{
		if (PositiveAxisSurfaceTypeID != INVALID_INDEX)
		{
			OgreEntityPositive->setMaterial(TEXTURE->getOgreMaterial(NewMaterialTypeID, PositiveAxisSurfaceTypeID));
		}
		if (NegativeAxisSurfaceTypeID != INVALID_INDEX)
		{
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
				OgreEntityNegative->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, NegativeAxisSurfaceTypeID));
			}
		}
	}
}
