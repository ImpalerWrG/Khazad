#ifndef SHAPEMANAGER__HEADER
#define SHAPEMANAGER__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <TileShapes.h>
#include <Ogre.h>

class ShapeManager
{
	DECLARE_SINGLETON_CLASS(ShapeManager)

public:

	~ShapeManager();

	Ogre::MeshPtr getFaceMesh(FaceShape Coordinate);

private:

	std::map<uint32_t, Ogre::MeshPtr> FaceEntityMap;

};

#define SHAPE (ShapeManager::GetInstance())

#endif // SHAPEMANAGER__HEADER
