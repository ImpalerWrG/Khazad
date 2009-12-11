#include <stdafx.h>

#include <PathTester.h>
#include <Singleton.h>

#include <PathManager.h>
#include <Map.h>
#include <Cell.h>
#include <Timer.h>
#include <Random.h>
#include <map>

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

}

void PathTester::CreateTestSuite()
{
    PathingTimer = new Timer();      // Master Timer
    NumTestPoints = 1000;

    TestCoords = new MapCoordinates[NumTestPoints];
    std::vector<MapCoordinates> PotentialTestCoords;

    std::map<uint64_t, Cell*>* TargetCells = MAP->getCellMap();
    for (std::map<uint64_t, Cell*>::iterator it = TargetCells->begin(); it != TargetCells->end(); ++it)
    {
        if (it->second != NULL)
        {
            Cell* TargetCell = it->second;
            CellCoordinates CellCoords = TargetCell->getCellCoordinates();

            CubeCoordinates TargetCubeCoords;
            for (TargetCubeCoords.X = 0; TargetCubeCoords.X < CELLEDGESIZE; TargetCubeCoords.X++)
            {
                for (TargetCubeCoords.Y = 0; TargetCubeCoords.Y < CELLEDGESIZE; TargetCubeCoords.Y++)
                {
                   MapCoordinates TestCoords = MapCoordinates(CellCoords, TargetCubeCoords);

                    if(false)
                    {
                        PotentialTestCoords.push_back(TestCoords);
                    }
                }
            }
        }
    }

    for(int i = 0; i < NumTestPoints; i++)
    {
        TestCoords[i] = MapCoordinates(0, 0, 0);
        // Generate real random passable points from the map
    }

    ManualPath = new FullPath();
}

MapPath* PathTester::getManualPath()
{
    return ManualPath;
}

MapPath* PathTester::FindManualPath()
{
    PATH->ResetProfileData(0);

    PathingTimer->Start();
        ManualPath = PATH->FindPath(0, getStartCoords(), getGoalCoords());
    ManualPathCost = PathingTimer->Stop();

    ManualPathSteps = ManualPath->StepCount;
    ManualPathLength = ManualPath->Length;
    ManualPathGraphReads = PATH->getGraphReads(0);
    ManualPathExpandedNodes = PATH->getExpandedNodeCount(0);

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
        // Identical Start and Goal points are possible, the PathManager should handle them
    }

    for (int SystemIndex = 0; SystemIndex < PathSystems.size(); SystemIndex++)
    {
        // Reset all the counting values for each System
        TotalGraphReads = NodesConsidered = TotalPathLength = TotalTimeCost = CacheHits = 0;

        for (int i = 0; i < Interations; ++i)
        {
            PATH->ResetProfileData(SystemIndex);

            PathingTimer->Start();
                ReturnedPath = PATH->FindPath(PathSystems[SystemIndex], TestCoords[StartCoords[i]], TestCoords[GoalCoords[i]]);
            TotalTimeCost += PathingTimer->Stop();

            TotalGraphReads += PATH->getGraphReads(SystemIndex);
            NodesConsidered += PATH->getExpandedNodeCount(SystemIndex);
            TotalPathLength += ReturnedPath->Length;
            CacheHits += PATH->isCacheHit(SystemIndex);

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
