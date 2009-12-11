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

    void RunPathTestSuite(int Interations, vector<int> PathSystems);
    void RunHuristicTestSuite(int Interations, vector<int> PathSystems);

    void SetStartCoords(MapCoordinates TestCoords)      { StartCoords = TestCoords; }
    MapCoordinates getStartCoords()                     { return StartCoords; }
    void SetGoalCoords(MapCoordinates TestCoords)       { GoalCoords = TestCoords; }
    MapCoordinates getGoalCoords()                      { return GoalCoords; }

    MapPath* FindManualPath();
    MapPath* getManualPath();

    int getManualPathSteps()                    { return ManualPathSteps; }
    int getManualPathCost()                     { return ManualPathCost; }
    float getManualPathLength()                 { return ManualPathLength; }
    int getManualPathGraphReads()               { return ManualPathGraphReads; }
    int getManualPathExpandedNodes()            { return ManualPathExpandedNodes; }

protected:

    Timer* PathingTimer;

    MapCoordinates* TestCoords;
    int NumTestPoints;

    MapCoordinates StartCoords, GoalCoords;  // Used for manual testing
    MapPath* ManualPath;

    int ManualPathSteps;
    int ManualPathCost;
    float ManualPathLength;
    int ManualPathGraphReads;
    int ManualPathExpandedNodes;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
