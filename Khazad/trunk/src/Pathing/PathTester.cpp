#include <stdafx.h>

#include <PathTester.h>
#include <Singleton.h>

#include <PathManager.h>
#include <Map.h>
#include <Cell.h>
#include <Random.h>
#include <map>

DECLARE_SINGLETON(PathTester)

PathTester::PathTester()
{

}

PathTester::~PathTester()
{
}

bool PathTester::Init()
{
    SystemProfileGroup = new GroupProfile();

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
}

MapPath* PathTester::getManualPath()
{
    return ManualProfile.ProfiledPath;
}

MapPath* PathTester::FindManualPath()
{
    PATH->ProfilePath(0, getStartCoords(), getGoalCoords(), &ManualProfile);

    return ManualProfile.ProfiledPath;
}

void PathTester::RunPathTestSuite(int Seed, int Iterations, vector<int> PathSystems)
{
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

    if (SystemProfileGroup == NULL)
    {
        SystemProfileGroup = new GroupProfile();
    }
    else
    {
        delete SystemProfileGroup;
        SystemProfileGroup = new GroupProfile();
    }
    //for (int SystemIndex = 0; SystemIndex < PathSystems.size(); SystemIndex++)
    //{
        // Reset all the profiles for each System
        for (int i = 0; i < Iterations; ++i)
        {
            Profile* NewProfile = new Profile();
            ReturnedPath = PATH->ProfilePath(/*PathSystems[SystemIndex]*/ 0, StartCoordsList[i], GoalCoordsList[i], NewProfile);
            SystemProfileGroup->Profiles.push_back(NewProfile);
        }

        SystemProfileGroup->Analyze();
    //}
}

void PathTester::RunHuristicTestSuite(int Interations, vector<int> PathSystems)
{

}
