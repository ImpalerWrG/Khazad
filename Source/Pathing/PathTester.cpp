#include <stdafx.h>

#include <PathTester.h>

#include <PathManager.h>
#include <Game.h>
#include <Map.h>
#include <Cell.h>
#include <Random.h>
#include <map>

#include <boost/unordered_map.hpp>

PathTester::PathTester()
{
}

PathTester::~PathTester()
{
}

bool PathTester::Init(PathManager* Parent)
{
    ProfileGroupList[0] = NULL;
    ProfileGroupList[1] = NULL;

    CurrentProfileGroup = NULL;
    ManualProfileGroup = NULL;
    ManualPath = NULL;

    ParentManager = Parent;

    PathDice = new RandomNumberGenerator();

    CollectTestCoords();
    CreateTestSuite(42, 1000);

    return true;
}

void PathTester::CollectTestCoords()
{
    boost::unordered_map<uint64_t, Cell*>* TargetCells = GAME->getMap()->getCellMap();

    for (boost::unordered_map<uint64_t, Cell*>::iterator it = TargetCells->begin(); it != TargetCells->end(); ++it)
    {
        if (it->second != NULL)
        {
            Cell* TargetCell = it->second;
            CellCoordinates CellCoords = TargetCell->getCellCoordinates();

            CubeCoordinates TargetCube = 0;
            do
            {
                MapCoordinates TestCoordinates = MapCoordinates(CellCoords, TargetCube);

                if (ParentManager->contains(TestCoordinates) && ParentManager->getDirectionFlags(TestCoordinates) != 0)
                {
                    TestCoords.push_back(TestCoordinates);
                }
                TargetCube++;
            }
            while (TargetCube != 0);  // End Loop when Byte rolls over
        }
    }
}

void PathTester::CreateTestSuite(int Seed, int Iterations)
{
    TestingIterations = Iterations;

    // Prepare a set of Start Goal pairs for the test
    for (int i = 0; i < Iterations; i++)
    {
        StartCoordsList.push_back(TestCoords[PathDice->Roll(0, (int32_t) TestCoords.size() - 1)]);
        GoalCoordsList.push_back(TestCoords[PathDice->Roll(0, (int32_t) TestCoords.size() - 1)]);
        // Identical Start and Goal points are possible, the PathManager should handle them
    }
}

MapCoordinates PathTester::getRandomPassableCoordinate()
{
    return TestCoords[PathDice->Roll(0, (int32_t) TestCoords.size() - 1)];
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
    ParentManager->ProfilePath(0, ManualStartCoords, ManualGoalCoords, NewProfile);

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
        ParentManager->ProfilePath(SystemIndex, StartCoordsList[i], GoalCoordsList[i], NewProfile);
        SystemProfileGroup->Profiles.push_back(NewProfile);
    }

    SystemProfileGroup->Analyze();
}

void PathTester::RunHuristicTestSuite(int Interations, vector<int> PathSystems)
{

}
