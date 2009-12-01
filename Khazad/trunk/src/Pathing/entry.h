#ifndef entry_HEADER
#define entry_HEADER

#include <vector>
#include <memory.h>

#include <heuristics.h>
#include <boost/shared_ptr.hpp>


struct entry
{
    virtual ~entry() {};
    virtual float value(const MapCoordinates &t, const Heuristic &h) const = 0;
    virtual float tiebreaker() const = 0;
    virtual MapCoordinates& getPoint() = 0;
    virtual bool costLessThan(const entry &e, const MapCoordinates &t, const Heuristic &h) const
    {
        float va,vb;
        va = value(t,h);
        vb = e.value(t,h);
        if (va < vb)
            return true;
        else if (va == vb)
            return tiebreaker() < e.tiebreaker();
        else
            return false;
    }
};

struct AStarEntry : entry
{
    MapCoordinates v_;
    float cost_;
    float tiebreaker_;
    std::vector<MapCoordinates> path_;

    AStarEntry(MapCoordinates v, float cost) : v_(v), cost_(cost), tiebreaker_(0) { }
    AStarEntry(MapCoordinates v, AStarEntry prev, float cost, float tiebreaker) : v_(v), cost_(cost), tiebreaker_(tiebreaker)
    {
        path_.reserve(prev.path_.size()+1);
        path_.insert(path_.begin(), prev.path_.begin(),prev.path_.end());
        path_.push_back(prev.v_);
    }

    float value(const MapCoordinates &t, const Heuristic &h) const
    {
        return cost_ + h (v_, t);
    }

    float tiebreaker() const
    {
        return tiebreaker_;
    }

    bool operator == (const AStarEntry& other) const
    {
        return v_ == other.v_;
    }

    virtual MapCoordinates& getPoint()
    {
        return v_;
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
    bool operator()(const entry &a, const entry &b) const
    {
        return !a.costLessThan(b, t_, *h_);
    }
};

#endif
