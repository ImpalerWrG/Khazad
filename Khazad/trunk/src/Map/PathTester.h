#ifndef TEST__HEADER
#define TEST__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Timer.h>
#include <Map.h>
#include <Path.h>

class PathTester
{
	DECLARE_SINGLETON_CLASS(PathTester)

public:

    ~PathTester();
    bool Init();

    void CreateTestSuite();

    void RunPathTestSuite(int Seed, int Iterations, vector<int> PathSystems);
    void RunHuristicTestSuite(int Interations, vector<int> PathSystems);

    void SetStartCoords(MapCoordinates TestCoords)      { ManualStartCoords = TestCoords; }
    MapCoordinates getStartCoords()                     { return ManualStartCoords; }
    void SetGoalCoords(MapCoordinates TestCoords)       { ManualGoalCoords = TestCoords; }
    MapCoordinates getGoalCoords()                      { return ManualGoalCoords; }

    MapPath* FindManualPath();
    MapPath* getManualPath();

    int getManualPathSteps()                    { return ManualPathSteps; }
    int getManualPathCost()                     { return ManualPathCost; }
    float getManualPathLength()                 { return ManualPathLength; }
    int getManualPathGraphReads()               { return ManualPathGraphReads; }
    int getManualPathExpandedNodes()            { return ManualPathExpandedNodes; }

protected:

    Timer* PathingTimer;

    std::vector<MapCoordinates> TestCoords;

    MapCoordinates ManualStartCoords, ManualGoalCoords;  // Used for manual testing
    MapPath* ManualPath;

    int ManualPathSteps;
    int ManualPathCost;
    float ManualPathLength;
    int ManualPathGraphReads;
    int ManualPathExpandedNodes;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
