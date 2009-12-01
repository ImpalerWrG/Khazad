/* Copyright Benoit Hudson 2009 */
#ifndef HEURISTIC_HEADER
#define HEURISTIC_HEADER

#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <Coordinates.h>

//typedef MapCoordinates point;

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

struct MaxHeuristic : Heuristic
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

        float Straits = abs((StartCoord.X - GoalCoord.X) + abs(StartCoord.Y - GoalCoord.Y));

        if(DiagonalsX < DiagonalsY)
        {
            return (M_SQRT2 * DiagonalsX) + (Straits - (2 * DiagonalsX));
        }
        else
        {
            return (M_SQRT2 * DiagonalsY) + (Straits - (2 * DiagonalsY));
        }
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
