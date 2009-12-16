#ifndef TEST__HEADER
#define TEST__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <Map.h>
#include <Path.h>

enum ProfileResultCode
{
    PATH_CODE_NO_DATA,
    PATH_CODE_SUCCESS,
    PATH_CODE_FAILUTE_UNITIALIZED,
    PATH_CODE_FAILURE_INVALID_LOCATION,
    PATH_CODE_FAILURE_NO_CONNECTION,
    PATH_CODE_FAILURE_UNKNOWN
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

    float GraphReadEfficency;
    float NodeSearchEfficency;
    float CacheEfficency;
    float TimeCostEfficency;

    void Analyze()
    {
        TotalGraphReads = TotalNodesExpanded = TotalCacheHits = TotalPathLength = TotalPathSteps =  TotalTimeCost = 0;

        for (int i = 0; i < Profiles.size(); i++)
        {
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
    GroupProfile* getSystemGroupProfile()                { return SystemProfileGroup; }

protected:

    std::vector<MapCoordinates> TestCoords;

    MapCoordinates ManualStartCoords, ManualGoalCoords;  // Used for manual testing

    Profile ManualProfile;
    GroupProfile* SystemProfileGroup;
};


#define TESTER (PathTester::GetInstance())

#endif // TEST__HEADER
