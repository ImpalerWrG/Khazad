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
    return NULL; //ManualProfile.ProfiledPath;
}

MapPath* PathTester::FindManualPath()
{
    //ManualProfileGroup;

    //std::vector<int> Systems;
    //Systems.push_back(0);  // 0th system for the manual path

    //RunPathTestSuite(Systems);

    //return ManualProfile.ProfiledPath;
    return NULL;
}

void PathTester::RunPathTestSuites(vector<int> TestSystems, int Iterations)
{
    std::vector<MapCoordinates> StartCoordsList, GoalCoordsList;
    RANDOM->Seed(0);

    // Prepare a set of Start Goal pairs for the test
    for (int i = 0; i < Iterations; i++)
    {
        StartCoordsList.push_back(TestCoords[RANDOM->Roll(0, (int32_t) TestCoords.size() - 1)]);
        GoalCoordsList.push_back(TestCoords[RANDOM->Roll(0, (int32_t) TestCoords.size() - 1)]);
        // Identical Start and Goal points are possible, the PathManager should handle them
    }

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

        TestSuite(Iterations, TestSystems[i], ProfileGroupList[TestSystems[i]], StartCoordsList, GoalCoordsList);
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
