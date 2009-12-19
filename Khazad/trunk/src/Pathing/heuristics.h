/* Copyright Benoit Hudson 2009 */
#ifndef HEURISTIC_HEADER
#define HEURISTIC_HEADER

#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <Coordinates.h>

template <typename T>
struct pointerEqualityPredicate
{
    const T* to_find;

    pointerEqualityPredicate(const T* t) : to_find(t) {}

    bool operator()(const T* other) const
    {
        return *to_find == *other;
    }
};

struct Heuristic // Interface (pure virtual base class)
{
    virtual ~Heuristic() {};
    virtual float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const = 0;
    float operator()(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        return Estimate(StartCoord, GoalCoord);
    }
};

struct Manhatten : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        return (float) abs(StartCoord.X - GoalCoord.X) + abs(StartCoord.Y - GoalCoord.Y) +  abs(StartCoord.Z - GoalCoord.Z);
    }
};

struct MaxDimension : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
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
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        float cost = StartCoord.X - GoalCoord.X;
        float sum = cost * cost;

        cost = StartCoord.Y - GoalCoord.Y;
        sum += cost * cost;

        // Use Z as well?  cuberoot?

        return sqrt(sum);
    }
};

struct Diagonal : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        float DiagonalsX = abs(StartCoord.X - GoalCoord.X);
        float DiagonalsY = abs(StartCoord.Y - GoalCoord.Y);
        float ZDifference = abs(StartCoord.Z - GoalCoord.Z);

        if(DiagonalsX < DiagonalsY)
        {
            return (M_SQRT2 * DiagonalsX) + (DiagonalsY - DiagonalsX) + (ZDifference * 2);
        }
        else
        {
            return (M_SQRT2 * DiagonalsY) + (DiagonalsX - DiagonalsY) + (ZDifference * 2);
        }
    }
};

struct StraitLine : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        /*
        dx1 = current.x - goal.x
        dy1 = current.y - goal.y

        dx2 = start.x - goal.x
        dy2 = start.y - goal.y

        cross = abs(dx1*dy2 - dx2*dy1)
        heuristic += cross*0.001
        */
        return 0;
    }
};

struct Dijkstra : Heuristic
{
    float Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        return 0;
    }
};

#endif // HEURISTIC_HEADER
