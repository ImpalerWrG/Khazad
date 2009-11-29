#include <stdafx.h>

#include <PathManager.h>
#include <Singleton.h>
#include <Map.h>
#include <astar.h>
#include <heuristics.h>


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
    //MAP->

    //PathAlgorithm AstarImplementation;
    //PathAlgorithm HeriarchialAstarImplementation;

    //GridGraph* MapGrid;

    //AstarImplementation = new AStar(MapGrid, Manhatten, Euclidean);


}

void PathManager::EditMapAbstraction()
{

}

void PathManager::DeleteMapAbstraction()
{

}

MapPath *PathManager::FindPath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return new FullPath();
}

float PathManager::EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return -1;
}


