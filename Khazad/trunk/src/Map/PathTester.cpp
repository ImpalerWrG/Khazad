#include <stdafx.h>

#include <PathTester.h>
#include <Singleton.h>

#include <PathManager.h>

#include <Timer.h>

DECLARE_SINGLETON(PathTester)

PathTester::PathTester()
{

}

PathTester::~PathTester()
{

}

bool PathTester::Init()
{
    PathingTimer = new Timer();      // Master Timer

    // Generate and store many random passable points from the map

    return true;
}

void PathTester::RunTestSuite()
{
    uint64_t AStarGraphReads, AStarNodesConsidered, AStarPathLength, AStarTimeCost = 0;
    double AStarGraphReadEfficiency, AStarSearchEfficiency = 0;

    //GridGraph graph (g);
    //MaxHeuristic max;
    //EuclideanTiebreaker euc;

    //gridZoneManager zm(&graph, &max, MAXLEN, SCALE, MINLEN);
    //AStar astar (&graph, &max, &euc);

    uint64_t ZonedAStarGraphReads, ZonedAStarNodesConsidered, ZonedAStarPathLength, ZonedAStarTimeCost, ZonedAStarCacheMisses = 0;
    double ZonedAStarGraphReadEfficiency, ZonedAStarSearchEfficiency = 0;

    //ZonedAStar zastar (&graph, &zm, &max, &euc);

    MapCoordinates StartPoint, GoalPoint;
    Path ReturnedPath;

#define ITERATIONS 100000
    for (unsigned i = 0; i < ITERATIONS; ++i)
    {
        // Select two points at random
        //StartPoint = getRandomPassablePoint(g);
        //GoalPoint = getRandomPassablePoint(g);

        //g->zeroCount();

        PathingTimer->Start();

            // Change to a call into the PathManager
            // Path= astar.path(StartPoint, GoalPoint);

        AStarTimeCost += PathingTimer->Stop();

/*
        AStarGraphReads += g->getCount();
        AStarNodesConsidered += astar.getCount();
        AStarPathLength += Path.first;

        AStarGraphReadEfficiency += AStarGraphReads / AStarPathLength;
        AStarSearchEfficiency += AStarNodesConsidered / AStarPathLength;

        astar.zeroCount();
        g->zeroCount();
*/

        PathingTimer->Start();

            ReturnedPath = PATH->FindPath(StartPoint, GoalPoint);

        ZonedAStarTimeCost += PathingTimer->Stop();

        /*
        ZonedAStarGraphReads += g->getCount();
        ZonedAStarNodesConsidered += zastar.getCount();
        ZonedAStarPathLength += Path.length;
        ZonedAStarCacheMisses += zastar.cachemiss;

        ZonedAStarGraphReadEfficiency += ZonedAStarGraphReads / ZonedAStarPathLength;
        ZonedAStarSearchEfficiency += ZonedAStarNodesConsidered / ZonedAStarPathLength;
        */

//#ifdef ZONE_DEBUG
        // zm.checkValid();
//#endif

        //zastar.zeroCount();
        //g->zeroCount();
    }

    printf("A*  total path length: %llu \n", AStarPathLength);
    printf("A*  graph reads: %llu \n", AStarGraphReads);
    printf("A*  graph read eff: %lg \n", ITERATIONS / AStarGraphReadEfficiency);
    printf("A*  nodes considered: %llu \n", AStarNodesConsidered);
    printf("A*  search eff: %lg \n", ITERATIONS / AStarSearchEfficiency);
    printf("A*  total time cost: %llu \n\n", AStarTimeCost);

    printf("ZA* total path length: %llu \n", ZonedAStarTimeCost);
    printf("ZA* graph reads: %llu \n", ZonedAStarGraphReads);
    printf("ZA* graph read eff: %lg \n", ITERATIONS / ZonedAStarGraphReadEfficiency);
    printf("ZA* nodes considered: %llu \n", ZonedAStarNodesConsidered);
    printf("ZA* search eff: %lg \n", ITERATIONS / ZonedAStarSearchEfficiency);
    printf("ZA* cache miss: %llu \n", ZonedAStarCacheMisses);
    printf("ZA* total time cost: %llu \n\n", ZonedAStarTimeCost);

    //delete g;
}
