#ifndef TEST__HEADER
#define TEST__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Timer.h>
#include <Map.h>
#include <Path.h>


struct Profile  // stores data from profiling
{
    Profile()
    {
        Valid = false;
        PathGraphReads = PathExpandedNodes = CacheHit = PathTimeCost = 0;
        ProfiledPath = NULL;
    }

    bool Valid;

    int PathTimeCost;
    int PathGraphReads;
    int PathExpandedNodes;
    bool CacheHit;

    MapPath* ProfiledPath;
};

struct GroupProfile
{
    bool Valid;

    std::vector<Profile> Profiles;

    float GraphReadEfficency;
    float NodeSearchEfficency;
    float CacheEfficency;
};

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

    Profile getManualProfile()                          { return ManualProfile; }

protected:

    Timer* PathingTimer;

    std::vector<MapCoordinates> TestCoords;

    MapCoordinates ManualStartCoords, ManualGoalCoords;  // Used for manual testing

    Profile ManualProfile;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
