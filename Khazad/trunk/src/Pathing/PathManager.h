#ifndef PATH_MANAGER__HEADER
#define PATH_MANAGER__HEADER

#include <stdafx.h>
#include <Singleton.h>

class MovementController;
class PathAlgorithm;
class MapCoordinates;
class KhazadGrid;
class AStar;
class Heuristic;
class Profile;
class MapPath;
class Timer;

class PathManager
{
    DECLARE_SINGLETON_CLASS(PathManager)

public:

    ~PathManager();
    bool Init();

    void CreateMapAbstraction();
    void EditMapAbstraction();
    void DeleteMapAbstraction();

    MovementController* getNewController(int AjentSize, int MovementType, MapCoordinates StarCoords); // Create a new controler

    MapPath* FindPath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);
    MapPath* ProfilePath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords, Profile* TargetProfile);

    bool isPathPossible(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);
    float EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);

    uint32_t getDirectionFlags(MapCoordinates Coordinates) const;
    bool contains(MapCoordinates Coordinates) const;

    uint32_t getZone(const MapCoordinates &TargetCoords) const;
    uint32_t getZoneEquivilency(const MapCoordinates &TargetCoords) const;

protected:

    Timer* PathingTimer;

    PathAlgorithm* AstarImplementation;
    PathAlgorithm* HeriarchialAstarImplementation;

    std::vector<MovementController*> MoveControllers;

    KhazadGrid* MapGrid;

    Heuristic* ManhattenHeuristic;
    Heuristic* EuclideanHeuristic;
    Heuristic* MaxDimensionHeuristic;
    Heuristic* DijkstraHeuristic;
    Heuristic* DiagonalHeuristic;
};

#define PATH (PathManager::GetInstance())

#endif // PATH__HEADER
