#ifndef entry_HEADER
#define entry_HEADER

#include <vector>
#include <memory.h>

class Heuristic;

#include <boost/shared_ptr.hpp>


struct entry
{
    virtual ~entry() {};
    virtual float value(const MapCoordinates &t, const Heuristic &h) const = 0;
    virtual float getTiebreaker() const = 0;
    virtual MapCoordinates& getCoordinates() = 0;

    virtual bool costLessThan(const entry &e, const MapCoordinates &t, const Heuristic &h) const
    {
        float va,vb;
        va = value(t,h);
        vb = e.value(t,h);
        if (va < vb)
            return true;
        else if (va == vb)
            return getTiebreaker() < e.getTiebreaker();
        else
            return false;
    }
};

struct AStarEntry : entry
{
    MapCoordinates LocationCoordinates;

    float PathLengthFromStart;
    float TieBreakerValue;

    AStarEntry* Parent;

    AStarEntry(MapCoordinates TargetCoordinates, float MinimumCost)
    {
        Parent = NULL;
        LocationCoordinates = TargetCoordinates;
        PathLengthFromStart = TieBreakerValue = 0;
        PathLengthFromStart = MinimumCost;
    }

    AStarEntry(MapCoordinates TargetCoordinates, AStarEntry* ParentEntry, float MinimumCost, float TieBreaker)
    {
        Parent = ParentEntry;
        LocationCoordinates = TargetCoordinates;
        PathLengthFromStart = MinimumCost;
        TieBreakerValue = TieBreaker;
    }

    float value(const MapCoordinates &GoalCoordinates, const Heuristic &TargetHeuristic) const
    {
        return PathLengthFromStart + TargetHeuristic(LocationCoordinates, GoalCoordinates);
    }

    float getTiebreaker() const
    {
        return TieBreakerValue;
    }

    bool operator == (const AStarEntry& other) const
    {
        return LocationCoordinates == other.LocationCoordinates;
    }

    virtual MapCoordinates& getCoordinates()
    {
        return LocationCoordinates;
    }
};

class entryGreaterThan
{
private:
    const MapCoordinates t_;
    const Heuristic *h_;

public:
    entryGreaterThan(const MapCoordinates t, const Heuristic *h) : t_(t), h_(h) {}

    bool operator()(const boost::shared_ptr<entry> a, const boost::shared_ptr<entry> b) const
    {
        return !a->costLessThan(*b, t_, *h_);
    }

    bool operator()(const entry* a, const entry* b) const
    {
        return !a->costLessThan(*b, t_, *h_);
    }

    bool operator()(const entry &a, const entry &b) const
    {
        return !a.costLessThan(b, t_, *h_);
    }
};

#endif
