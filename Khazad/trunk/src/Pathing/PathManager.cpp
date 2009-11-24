#include <stdafx.h>

#include <PathManager.h>
#include <Singleton.h>


DECLARE_SINGLETON(PathManager)

PathManager::PathManager()
{

}

PathManager::~PathManager()
{

}

bool PathManager::Init()
{
    return true;
}

void PathManager::CreateMapAbstraction()
{

}

void PathManager::EditMapAbstraction()
{

}

void PathManager::DeleteMapAbstraction()
{

}

Path FindPath(MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return Path();
}

float EstimatePathLength(MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return -1;
}

