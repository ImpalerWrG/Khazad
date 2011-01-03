#include <Face.h>


#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>


Face::Face(Ogre::SceneNode* CellSceneNode, CubeCoordinates TargetCoordinates, Direction DirectionType)
{
	SurfaceTypeID = INVALID_INDEX;
	MaterialTypeID = INVALID_INDEX;

    FaceType = FaceShape(CubeShape(false), DirectionType);
    CellNode = CellSceneNode;
	LocationCoordinates = TargetCoordinates;
}

void Face::RefreshEntity()
{
    if (SurfaceTypeID != INVALID_INDEX && MaterialTypeID != INVALID_INDEX && !FaceType.CubeComponent.isEmpty() && !FaceType.CubeComponent.isSolid())
    {
        Ogre::SceneNode* NewNode = CellNode->createChildSceneNode();
        NewNode->setPosition((LocationCoordinates >> CELLBITSHIFT) - (CELLEDGESIZE / 2) + HALFCUBE, (LocationCoordinates & CELLBITFLAG) - (CELLEDGESIZE / 2) + HALFCUBE, 0);

        char buffer[64];
        FaceType.getName(buffer);

        if(!Ogre::MeshManager::getSingleton().getByName(buffer).isNull())
        {
            Ogre::Entity* NewEntity = RENDERER->getSceneManager()->createEntity(buffer);
            NewEntity->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, SurfaceTypeID));

            NewNode->attachObject(NewEntity);
        }
    }
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

void Face::setFaceShapeType(FaceShape NewShape)
{
    if (NewShape != FaceType)
    {
        FaceType = NewShape;

		//Set Cell needsRedraw
    }
}
