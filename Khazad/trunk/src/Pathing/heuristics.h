/* Copyright Benoit Hudson 2009 */
#ifndef HEURISTIC_HEADER
#define HEURISTIC_HEADER

#include <math.h>
#include <stdlib.h>

#include <point.h>

typedef int cost_t;

//Interface (pure virtual base class)
struct Heuristic
{
    virtual ~Heuristic() {};
    virtual cost_t operator() (const point p, const point t) const = 0;
};

struct L1Heuristic : Heuristic
{
    cost_t operator () (const point p, const point t) const
    {
        cost_t sum = 0;
        for (unsigned i = 0; i < point::dim; ++i)
        {
            sum += abs(p[i] - t[i]);
        }
        return sum;
    }
};

struct MaxHeuristic : Heuristic
{
    cost_t operator () (const point p, const point t) const
    {
        cost_t max = 0;
        for (unsigned i = 0; i < point::dim; ++i)
        {
            cost_t cost = abs(p[i] - t[i]);
            if (cost > max)
                max = cost;
        }
        return max;
    }
};

struct EuclideanTiebreaker : Heuristic
{
    cost_t operator () (const point p, const point t) const
    {
        cost_t sum = 0;
        for (unsigned i = 0; i < point::dim; ++i)
        {
            cost_t cost = p[i] - t[i];
            sum += cost*cost;
        }
        return sum;
    }
};

#endif // HEURISTIC_HEADER
