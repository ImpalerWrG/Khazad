#ifndef TEST__HEADER
#define TEST__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Map.h>
#include <Path.h>
#include <Random.h>

class PathManager;

enum ProfileResultCode
{
    PATH_CODE_NO_DATA,
    PATH_CODE_SUCCESS,
    PATH_CODE_FAILUTE_UNITIALIZED,
    PATH_CODE_FAILURE_INVALID_LOCATION,
    PATH_CODE_FAILURE_NO_CONNECTION,
    PATH_CODE_FAILURE_UNKNOWN,

    NUM_PATH_CODES = 6
};

struct Profile  // stores data from profiling
{
    Profile()
    {
        ResultCode = PATH_CODE_NO_DATA;
        PathGraphReads = PathExpandedNodes = CacheHit = PathTimeCost = 0;
        ProfiledPath = NULL;
    }

    ProfileResultCode ResultCode;

    int PathTimeCost;
    int PathGraphReads;
    int PathExpandedNodes;
    bool CacheHit;

    MapPath* ProfiledPath;
};

struct GroupProfile
{
    GroupProfile()
    {
        TotalGraphReads = TotalNodesExpanded = TotalCacheHits = TotalPathLength = TotalPathSteps =  TotalTimeCost = 0;
    }

    std::vector<Profile*> Profiles;

    int TotalPathSteps;
    float TotalPathLength;
    int TotalGraphReads;
    int TotalNodesExpanded;
    int TotalCacheHits;
    int TotalTimeCost;

    int Results[NUM_PATH_CODES];

    float GraphReadEfficency;
    float NodeSearchEfficency;
    float CacheEfficency;
    float TimeCostEfficency;

    void Analyze()
    {
        TotalGraphReads = TotalNodesExpanded = TotalCacheHits = TotalPathLength = TotalPathSteps =  TotalTimeCost = 0;

        for (int i = 0; i < NUM_PATH_CODES; i++)
        {
            Results[i] = 0;
        }

        for (int i = 0; i < Profiles.size(); i++)
        {
            Results[(ProfileResultCode) Profiles[i]->ResultCode] += 1;

            if (Profiles[i]->ProfiledPath != NULL && Profiles[i]->ResultCode == PATH_CODE_SUCCESS)
            {
                TotalPathSteps += Profiles[i]->ProfiledPath->StepCount;
                TotalPathLength += Profiles[i]->ProfiledPath->Length;
            }

            TotalTimeCost += Profiles[i]->PathTimeCost;
            TotalGraphReads += Profiles[i]->PathGraphReads;
            TotalNodesExpanded += Profiles[i]->PathExpandedNodes;
            TotalCacheHits += (int) Profiles[i]->CacheHit;
        }

        GraphReadEfficency = TotalGraphReads / (float) TotalPathSteps;
        NodeSearchEfficency = TotalNodesExpanded / (float) TotalPathSteps;
        CacheEfficency = TotalCacheHits / (float) Profiles.size();
        TimeCostEfficency = TotalTimeCost / (float) TotalPathSteps;
    }
};

class PathTester
{
	DECLARE_SINGLETON_CLASS(PathTester)

public:

    ~PathTester();
    bool Init(PathManager* Parent);

    void CreateTestSuite(int Seed, int Iterations);
    MapCoordinates getRandomPassableCoordinate();

    void RunPathTestSuites(vector<int> TestSystems);
    void TestSuite(int Iterations, int SystemIndex, GroupProfile* SystemProfileGroup, vector<MapCoordinates> StartCoordsList, vector<MapCoordinates> GoalCoordsList);

    void RunHuristicTestSuite(int Interations, vector<int> PathSystems);

    void SetStartCoords(MapCoordinates TestCoords)      { ManualStartCoords = TestCoords; }
    MapCoordinates getStartCoords()                     { return ManualStartCoords; }
    void SetGoalCoords(MapCoordinates TestCoords)       { ManualGoalCoords = TestCoords; }
    MapCoordinates getGoalCoords()                      { return ManualGoalCoords; }

    void ProfileManualPath();
    MapPath* getManualPath()                            { return ManualPath; }

    GroupProfile* getCurrentGroupProfile()              { return CurrentProfileGroup; }

protected:

    PathManager* ParentManager;
    RandomNumberGenerator* PathDice;

    MapCoordinates ManualStartCoords, ManualGoalCoords;  // Used for manual testing
    MapPath* ManualPath;

    GroupProfile* ManualProfileGroup;
    GroupProfile* CurrentProfileGroup;
    GroupProfile* ProfileGroupList[2];

    std::vector<MapCoordinates> TestCoords;
    std::vector<MapCoordinates> StartCoordsList, GoalCoordsList;
    int TestingIterations;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
