#include <stdafx.h>

#include <PathManager.h>
#include <Singleton.h>
#include <Map.h>
#include <grid.h>
#include <astar.h>
#include <heuristics.h>


DECLARE_SINGLETON(PathManager)

PathManager::PathManager()
{

}

PathManager::~PathManager()
{
    DeleteMapAbstraction();
}

bool PathManager::Init()
{
    ManhattenHeuristic = new Manhatten();
    EuclideanHeuristic = new Euclidean();
    MaxHeuristicType = new MaxHeuristic();
    DijkstraHeuristic = new Dijkstra();

    return true;
}

void PathManager::CreateMapAbstraction()
{
    MapGrid = new KhazadGrid();

    AstarImplementation = new AStar(MapGrid, ManhattenHeuristic, EuclideanHeuristic);
}

void PathManager::EditMapAbstraction()
{

}

void PathManager::DeleteMapAbstraction()
{
    delete MapGrid;
    MapGrid = NULL;

    delete AstarImplementation;
    AstarImplementation = NULL;
}

MapPath* PathManager::FindPath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    if (AstarImplementation != NULL)
    {
        return AstarImplementation->FindPath(StartCoords, GoalCoords);
    }
    return NULL;
}

float PathManager::EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return -1;
}

uint32_t PathManager::getDirectionFlags(MapCoordinates Coordinates)
{
    if (MapGrid != NULL)
    {
        return MapGrid->getDirectionFlags(Coordinates);
    }
}
