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

    void RunPathTestSuite(int Interations, vector<int> PathSystems);
    void RunHuristicTestSuite(int Interations, vector<int> PathSystems);

    void SetStartCoords(MapCoordinates TestCoords)      { StartCoords = TestCoords; }
    MapCoordinates getStartCoords()                     { return StartCoords; }
    void SetGoalCoords(MapCoordinates TestCoords)       { GoalCoords = TestCoords; }
    MapCoordinates getGoalCoords()                      { return GoalCoords; }

    MapPath* FindManualPath();
    MapPath* getManualPath();

protected:

    Timer* PathingTimer;

    MapCoordinates* TestCoords;
    int NumTestPoints;

    MapCoordinates StartCoords, GoalCoords;  // Used for manual testing
    MapPath* ManualPath;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
