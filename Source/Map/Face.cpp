#include <Face.h>


#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>


Face::Face(Ogre::SceneNode* CellSceneNode, CubeCoordinates TargetCoordinates, Direction DirectionType)
{
	SurfaceTypeID = INVALID_INDEX;
	MaterialTypeID = INVALID_INDEX;

    FaceType = FaceShape(CubeShape(false), DirectionType);
	LocationCoordinates = TargetCoordinates;
}

void Face::RefreshEntity(Ogre::SceneNode* CellNode)
{
    if (SurfaceTypeID != INVALID_INDEX && MaterialTypeID != INVALID_INDEX) // && !FaceType.CubeComponent.isSolid())
    {
        Ogre::SceneNode* NewNode = CellNode->createChildSceneNode();

        int16_t X = (LocationCoordinates >> CELLBITSHIFT);
        int16_t Y = (LocationCoordinates & CELLBITFLAG);
        int16_t Z = 0;

        if (DirectionValueOnAxis(FaceType.FaceDirection, AXIS_X) == 1)
        {
            X -= 1;
        }

        if (DirectionValueOnAxis(FaceType.FaceDirection, AXIS_Y) == 1)
        {
            Y -= 1;
        }

        if (DirectionValueOnAxis(FaceType.FaceDirection, AXIS_Z) == 1)
        {
            Z -= 1;
        }

        NewNode->setPosition(X - (CELLEDGESIZE / 2) + HALFCUBE, Y - (CELLEDGESIZE / 2) + HALFCUBE, Z);

        char buffer[64];
        FaceType.getName(buffer);

        if (!Ogre::MeshManager::getSingleton().getByName(buffer).isNull())
        {
            Ogre::Entity* NewEntity = RENDERER->getSceneManager()->createEntity(buffer);
            NewEntity->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, SurfaceTypeID));
            //NewEntity->setMaterialName("BaseWhite");

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
