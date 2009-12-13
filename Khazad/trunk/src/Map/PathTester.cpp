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

}

PathTester::~PathTester()
{
   delete PathingTimer;
}

bool PathTester::Init()
{
    PathingTimer = new Timer();      // Master Timer

    return true;
}

void PathTester::CreateTestSuite()
{
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
                    MapCoordinates TestCoordinates = MapCoordinates(CellCoords, TargetCubeCoords);

                    if (PATH->contains(TestCoordinates) && PATH->getDirectionFlags(TestCoordinates) != 0)
                    {
                        TestCoords.push_back(TestCoordinates);
                    }
                }
            }
        }
    }

    ManualProfile.Valid = false;
}

MapPath* PathTester::getManualPath()
{
    return ManualProfile.ProfiledPath;
}

MapPath* PathTester::FindManualPath()
{
    PATH->ResetProfileData(0);

    PathingTimer->Start();
        MapPath* ManualPath = PATH->FindPath(0, getStartCoords(), getGoalCoords());
    ManualProfile.PathTimeCost = PathingTimer->Stop();

    ManualProfile.ProfiledPath = ManualPath;

    ManualProfile.PathGraphReads = PATH->getGraphReads(0);
    ManualProfile.PathExpandedNodes = PATH->getExpandedNodeCount(0);

    ManualProfile.Valid = true;
    return ManualPath;
}

void PathTester::RunPathTestSuite(int Seed, int Iterations, vector<int> PathSystems)
{
    uint64_t TotalGraphReads, NodesConsidered, TotalPathSteps, TotalTimeCost, CacheHits;

    MapPath* ReturnedPath;
    std::vector<MapCoordinates> StartCoordsList, GoalCoordsList;
    RANDOM->Seed(Seed);

    // Prepare a set of Start Goal pairs for the test
    for (int i = 0; i < Iterations; i++)
    {
        MapCoordinates StartCoords = TestCoords[RANDOM->Roll(0, (int32_t) TestCoords.size() - 1)];
        StartCoordsList.push_back(StartCoords);

        MapCoordinates GoalCoords = TestCoords[RANDOM->Roll(0, (int32_t) TestCoords.size() - 1)];
        GoalCoordsList.push_back(GoalCoords);
        // Identical Start and Goal points are possible, the PathManager should handle them
    }

    for (int SystemIndex = 0; SystemIndex < PathSystems.size(); SystemIndex++)
    {
        // Reset all the counting values for each System
        TotalGraphReads = NodesConsidered = TotalPathSteps = TotalTimeCost = CacheHits = 0;

        for (int i = 0; i < Iterations; ++i)
        {
            PATH->ResetProfileData(SystemIndex);

            PathingTimer->Start();
                ReturnedPath = PATH->FindPath(PathSystems[SystemIndex], StartCoordsList[i], GoalCoordsList[i]);
            TotalTimeCost += PathingTimer->Stop();

            TotalGraphReads += PATH->getGraphReads(SystemIndex);
            NodesConsidered += PATH->getExpandedNodeCount(SystemIndex);
            TotalPathSteps += ReturnedPath->StepCount;
            CacheHits += PATH->isCacheHit(SystemIndex);

            delete ReturnedPath;
        }

        printf("System %i TestResults\n\n", PathSystems[SystemIndex]);
        printf("Total path length: %i\n", TotalPathSteps);
        printf("Graph reads: %i\n", TotalGraphReads);
        printf("Graph read efficiency (Reads per Step): %f \n", (float) TotalGraphReads / TotalPathSteps);
        printf("Nodes considered: %i \n", NodesConsidered);
        printf("Search efficiency (Nodes per Step): %f \n", (float) NodesConsidered / TotalPathSteps);
        printf("Cache efficiency (Percent Hits): %f \n", CacheHits / Iterations);
        printf("Total time cost: %i \n", TotalTimeCost);
        printf("Time efficiency (Time per Step): %f \n", (float) TotalTimeCost / TotalPathSteps);
    }
}

void PathTester::RunHuristicTestSuite(int Interations, vector<int> PathSystems)
{

}
