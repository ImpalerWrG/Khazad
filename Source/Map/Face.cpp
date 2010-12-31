#include <Face.h>


#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>


Face::Face(Ogre::SceneNode* CellSceneNode, CubeCoordinates TargetCoordinates)
{
	SurfaceTypeID = INVALID_INDEX;
	MaterialTypeID = INVALID_INDEX;

    ShapeType = CubeShape(false);
    CellNode = CellSceneNode;
	LocationCoordinates = TargetCoordinates;
}

void Face::RefreshEntity()
{
    if (SurfaceTypeID != INVALID_INDEX && MaterialTypeID != INVALID_INDEX && !ShapeType.isEmpty() && !ShapeType.isSolid())
    {
        Ogre::SceneNode* NewNode = CellNode->createChildSceneNode();
        NewNode->setPosition((LocationCoordinates >> CELLBITSHIFT) - (CELLEDGESIZE / 2) + HALFCUBE, (LocationCoordinates & CELLBITFLAG) - (CELLEDGESIZE / 2) + HALFCUBE, 0);

        char buffer[64];
        sprintf(buffer, "Slope%i", ShapeType.Key());

        if(!Ogre::MeshManager::getSingleton().getByName(buffer).isNull())
        {
            Ogre::Entity* NewEntity = RENDERER->getSceneManager()->createEntity(buffer);
            NewEntity->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, SurfaceTypeID));

            NewNode->attachObject(NewEntity);
        }
    }



    /*
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
    */
}

Face::~Face()
{
    //Set Cell needsRedraw
}

void Face::setFaceMaterialType(int16_t NewMaterialTypeID)
{
	if (NewMaterialTypeID != MaterialTypeID)
	{
		MaterialTypeID = NewMaterialTypeID;

		//Set Cell needsRedraw
	}
}

void Face::setFaceSurfaceType(int16_t NewSurfaceTypeID)
{
    if (NewSurfaceTypeID != SurfaceTypeID)
    {
        SurfaceTypeID = NewSurfaceTypeID;

		//Set Cell needsRedraw
    }
}

void Face::setShapeType(CubeShape NewShape)
{
    if (NewShape != ShapeType)
    {
        ShapeType = NewShape;

		//Set Cell needsRedraw
    }
}
