#ifndef SHAPEMANAGER__HEADER
#define SHAPEMANAGER__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <TileShapes.h>
#include <Ogre.h>

#include <boost/unordered_map.hpp>

class ShapeManager
{
	DECLARE_SINGLETON_CLASS(ShapeManager)

public:

	~ShapeManager();

	Ogre::MeshPtr getFaceMesh(FaceShape Coordinate);

private:

	boost::unordered_map<uint32_t, Ogre::MeshPtr> FaceEntityMap;

};

#define SHAPE (ShapeManager::GetInstance())

#endif // SHAPEMANAGER__HEADER
