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
    ProfileGroupList[0] = NULL;
    ProfileGroupList[1] = NULL;

    CurrentProfileGroup = NULL;
    ManualProfileGroup = NULL;
    ManualPath = NULL;

    return true;
}

void PathTester::CreateTestSuite(int Seed, int Iterations)
{
    std::vector<MapCoordinates> TestCoords;
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

    RANDOM->Seed(Seed);
    TestingIterations = Iterations;

    // Prepare a set of Start Goal pairs for the test
    for (int i = 0; i < Iterations; i++)
    {
        StartCoordsList.push_back(TestCoords[RANDOM->Roll(0, (int32_t) TestCoords.size() - 1)]);
        GoalCoordsList.push_back(TestCoords[RANDOM->Roll(0, (int32_t) TestCoords.size() - 1)]);
        // Identical Start and Goal points are possible, the PathManager should handle them
    }
}

void PathTester::ProfileManualPath()
{
    if (ManualProfileGroup == NULL)
    {
        ManualProfileGroup = new GroupProfile();
    }
    else
    {
        delete ManualProfileGroup;
        ManualProfileGroup = new GroupProfile();
    }

    Profile* NewProfile = new Profile();
    PATH->ProfilePath(0, ManualStartCoords, ManualGoalCoords, NewProfile);

    ManualProfileGroup->Profiles.push_back(NewProfile);
    ManualPath = NewProfile->ProfiledPath;

    ManualProfileGroup->Analyze();
    CurrentProfileGroup = ManualProfileGroup;
}

void PathTester::RunPathTestSuites(vector<int> TestSystems)
{
    for (int i = 0; i < TestSystems.size(); i++)
    {
        if (ProfileGroupList[TestSystems[i]] == NULL)
        {
            ProfileGroupList[TestSystems[i]] = new GroupProfile();
        }
        else
        {
            delete ProfileGroupList[TestSystems[i]];
            ProfileGroupList[TestSystems[i]] = new GroupProfile();
        }

        TestSuite(TestingIterations, TestSystems[i], ProfileGroupList[TestSystems[i]], StartCoordsList, GoalCoordsList);
        CurrentProfileGroup = ProfileGroupList[TestSystems[i]];
    }
}

void PathTester::TestSuite(int Iterations, int SystemIndex, GroupProfile* SystemProfileGroup, vector<MapCoordinates> StartCoordsList, vector<MapCoordinates> GoalCoordsList)
{
    for (int i = 0; i < Iterations; ++i)
    {
        Profile* NewProfile = new Profile();
        PATH->ProfilePath(SystemIndex, StartCoordsList[i], GoalCoordsList[i], NewProfile);
        SystemProfileGroup->Profiles.push_back(NewProfile);
    }

    SystemProfileGroup->Analyze();
}

void PathTester::RunHuristicTestSuite(int Interations, vector<int> PathSystems)
{

}
