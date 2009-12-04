#include <stdafx.h>

#include <PathTester.h>
#include <Singleton.h>

#include <PathManager.h>
#include <Map.h>
#include <Timer.h>
#include <Random.h>

DECLARE_SINGLETON(PathTester)

PathTester::PathTester()
{
   Init();
}

PathTester::~PathTester()
{
   delete ManualPath;
   delete TestCoords;
}

bool PathTester::Init()
{
    PathingTimer = new Timer();      // Master Timer
    NumTestPoints = 1000;

    TestCoords = new MapCoordinates[NumTestPoints];

    for(int i = 0; i < NumTestPoints; i++)
    {
        TestCoords[i] = MapCoordinates(0, 0, 0);
        // Generate real random passable points from the map
    }

    ManualPath = new FullPath();

    return true;
}

MapPath* PathTester::getManualPath()
{
    return ManualPath;
}

MapPath* PathTester::FindManualPath()
{
    PATH->ResetProfileData();

    PathingTimer->Start();
        ManualPath = PATH->FindPath(0, getStartCoords(), getGoalCoords());
    int TimeCost = PathingTimer->Stop();

    //printf("System %i TestResults\n\n", PathSystems[SystemIndex]);
    printf("Ttotal path length: %g \n", ManualPath->Length);
    printf("Graph reads: %llu \n", PATH->getGraphReads());
    printf("Graph read efficiency: %lg \n", PATH->getGraphReads() / ManualPath->Length);
    printf("Nodes considered: %llu \n", PATH->getExpandedNodeCount());
    printf("Search efficiency: %lg \n", PATH->getExpandedNodeCount() / ManualPath->Length);
    printf("Total time cost: %llu \n\n", TimeCost);

    return ManualPath;
}

void PathTester::RunPathTestSuite(int Interations, vector<int> PathSystems)
{
    uint64_t TotalGraphReads, NodesConsidered, TotalPathLength, TotalTimeCost, CacheHits;

    MapPath *ReturnedPath;
    vector <int> StartCoords, GoalCoords;

    // Prepare a set of Start Goal pairs for the test
    for (int i = 0; i < Interations; i++)
    {
        StartCoords.push_back(RANDOM->Roll(0, NumTestPoints));
        GoalCoords.push_back(RANDOM->Roll(0, NumTestPoints));
        // Identic Start and Goal points are possible, the PathManager should handle them
    }

    for (int SystemIndex = 0; SystemIndex < PathSystems.size(); SystemIndex++)
    {
        // Reset all the counting values for each System
        TotalGraphReads = NodesConsidered = TotalPathLength = TotalTimeCost = CacheHits = 0;

        for (int i = 0; i < Interations; ++i)
        {
            PATH->ResetProfileData();

            PathingTimer->Start();
            ReturnedPath = PATH->FindPath(PathSystems[SystemIndex], TestCoords[StartCoords[i]], TestCoords[GoalCoords[i]]);
            TotalTimeCost += PathingTimer->Stop();

            TotalGraphReads += PATH->getGraphReads();
            NodesConsidered += PATH->getExpandedNodeCount();
            TotalPathLength += ReturnedPath->Length;
            CacheHits += PATH->isCacheHit();
           delete ReturnedPath;
        }

        printf("System %i TestResults\n\n", PathSystems[SystemIndex]);
        printf("Ttotal path length: %g \n", TotalPathLength);
        printf("Graph reads: %llu \n", TotalGraphReads);
        printf("Graph read efficiency: %lg \n", (TotalGraphReads / TotalPathLength) / Interations);
        printf("Nodes considered: %llu \n", NodesConsidered);
        printf("Search efficiency: %lg \n", (NodesConsidered / TotalPathLength) / Interations);
        printf("Cache efficiency: %llu \n", CacheHits / Interations);
        printf("Total time cost: %llu \n\n", TotalTimeCost);
    }
}

void PathTester::RunHuristicTestSuite(int Interations, vector<int> PathSystems)
{

}
