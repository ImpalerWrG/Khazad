/* Copyright Benoit Hudson 2009 */
#ifndef HEURISTIC_HEADER
#define HEURISTIC_HEADER

#include <math.h>
#include <stdlib.h>

#include <Coordinates.h>

typedef float cost_t;

typedef MapCoordinates point;

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
    virtual cost_t Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const = 0;
    cost_t operator()(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const { return Estimate(StartCoord, GoalCoord); }
};

struct Manhatten : Heuristic
{
    cost_t Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        return (cost_t) abs(StartCoord.X - GoalCoord.X) + abs(StartCoord.Y - GoalCoord.Y) +  abs(StartCoord.Z - GoalCoord.Z);
    }
};

struct MaxHeuristic : Heuristic
{
    cost_t Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        cost_t max = 0;
        cost_t cost = 0;

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
    cost_t Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        cost_t cost = StartCoord.X - GoalCoord.X;
        cost_t sum = cost * cost;

        cost = StartCoord.Y - GoalCoord.Y;
        sum += cost * cost;

        // Use Z as well?  cuberoot?

        return sqrt(sum);
    }
};

struct Dijkstra : Heuristic
{
    cost_t Estimate(const MapCoordinates StartCoord, const MapCoordinates GoalCoord) const
    {
        return 0;
    }
};

#endif // HEURISTIC_HEADER
