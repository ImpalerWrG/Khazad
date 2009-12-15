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
    MaxDimensionHeuristic = new MaxDimension();
    DijkstraHeuristic = new Dijkstra();
    DiagonalHeuristic = new Diagonal();

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
        if (MapGrid->contains(StartCoords) && MapGrid->contains(GoalCoords))
        {
            if (isPathPossible(PathSystem, StartCoords, GoalCoords))
            {
                return AstarImplementation->FindPath(StartCoords, GoalCoords);
            }
        }
    }
    return NULL;
}

bool PathManager::isPathPossible(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    if (MapGrid != NULL)
    {
        return MapGrid->isPathPossible(StartCoords, GoalCoords);
    }
    return false;
}

float PathManager::EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    return -1;
}

uint32_t PathManager::getDirectionFlags(MapCoordinates Coordinates) const
{
    if (MapGrid != NULL)
    {
        return MapGrid->getDirectionFlags(Coordinates);
    }
    return 0;
}

bool PathManager::contains(MapCoordinates Coordinates) const
{
    if (MapGrid != NULL)
    {
        return MapGrid->contains(Coordinates);
    }
    return false;
}

uint32_t PathManager::getZone(const MapCoordinates &TargetCoords) const
{
    if (MapGrid != NULL)
    {
        return MapGrid->getConnectivityZone(TargetCoords);
    }
    return 0;
}

uint32_t PathManager::getZoneEquivilency(const MapCoordinates &TargetCoords) const
{
    if (MapGrid != NULL)
    {
        return MapGrid->getZoneEquivilency(TargetCoords);
    }
    return 0;
}

int PathManager::getExpandedNodeCount(int SystemIndex) const
{
    return AstarImplementation->getExpandedNodes();
}

int PathManager::getGraphReads(int SystemIndex) const
{
    return AstarImplementation->getGraphReads();
}

bool PathManager::isCacheHit(int SystemIndex) const
{
    return false;
}

void PathManager::ResetProfileData(int SystemIndex)
{
    AstarImplementation->ResetPrifiler();
}
