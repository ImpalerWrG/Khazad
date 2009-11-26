/* Copyright Benoit Hudson 2009 */
#ifndef HEURISTIC_HEADER
#define HEURISTIC_HEADER

#include <math.h>
#include <stdlib.h>

#include <Coordinates.h>


struct Heuristic // Interface (pure virtual base class)
{
    virtual ~Heuristic() {};
    virtual float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const = 0;
};

struct Manhatten : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord)
    {
        return (float) abs(StartCoord.X - GoalCoord.X) + abs(StartCoord.Y - GoalCoord.Y) +  abs(StartCoord.Z - GoalCoord.Z);
    }
};

struct MaxHeuristic : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord)
    {
        float max = 0;
        float cost = 0;

        cost = abs(StartCoord.X - GoalCoord.X);
        if (cost > max)
            max = cost;

        cost = abs(StartCoord.Y - GoalCoord.Y);
        if (cost > max)
            max = cost;

        cost = abs(StartCoord.Z - GoalCoord.Z);
        if (cost > max)
            max = cost;

        return max;
    }
};

struct Euclidean : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord)
    {
        float cost = StartCoord.X - GoalCoord.X;
        float sum = cost * cost;

        cost = StartCoord.Y - GoalCoord.Y;
        sum += cost * cost;

        // Use Z as well?  cuberoot?

        return sqrt(sum);
    }
};

struct Dijkstra : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord)
    {
        return 0;
    }
};

#endif // HEURISTIC_HEADER
