#include <ShapeManager.h>
#include <TileShapes.h>
#include <Coordinates.h>

DECLARE_SINGLETON(ShapeManager)

ShapeManager::ShapeManager()
{

}

ShapeManager::~ShapeManager()
{

}

Ogre::MeshPtr ShapeManager::getFaceMesh(FaceShape Shape)
{
	uint32_t Key = Shape.Key();

    if (FaceEntityMap.find(Key) == FaceEntityMap.end())
    {
		if (Shape.FaceDirection == DIRECTION_NONE)
		{
			Ogre::MeshPtr NewFace = CreateSlopeFace(Shape);
			FaceEntityMap[Key] = NewFace;
			return NewFace;
		} else {
			if (Shape.CubeComponent.hasFloor() || Shape.CubeComponent.hasCeiling())
			{
				Ogre::MeshPtr NewFace = CreateFlatFace(Shape);
				FaceEntityMap[Key] = NewFace;
				return NewFace;
			} else {
				Ogre::MeshPtr NewFace = CreateSideFace(Shape);
				FaceEntityMap[Key] = NewFace;
				return NewFace;
			}
		}
    }
    else
    {
        return FaceEntityMap.find(Key)->second;
    }
}

