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

MapPath PathManager::FindPath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return MapPath();
}

float PathManager::EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return -1;
}


