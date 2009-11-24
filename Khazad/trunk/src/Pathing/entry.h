#ifndef entry_HEADER
#define entry_HEADER

#include <vector>
#include <memory>

#include "heuristics.h"
#include "point.h"

struct entry
{
    virtual ~entry() {};
    virtual cost_t value(const point &t, const Heuristic &h) const = 0;
    virtual cost_t tiebreaker() const = 0;
    virtual point& getPoint() = 0;
    virtual bool costLessThan(const entry &e, const point &t, const Heuristic &h) const
    {
        cost_t va,vb;
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
    point v_;
    cost_t cost_;
    cost_t tiebreaker_;
    std::vector<point> path_;

    AStarEntry(point v, cost_t cost) : v_(v), cost_(cost), tiebreaker_(0) { }
    AStarEntry(point v, AStarEntry prev, cost_t cost, cost_t tiebreaker) : v_(v), cost_(cost), tiebreaker_(tiebreaker)
    {
        path_.reserve(prev.path_.size()+1);
        path_.insert(path_.begin(), prev.path_.begin(),prev.path_.end());
        path_.push_back(prev.v_);
    }

    cost_t value(const point &t, const Heuristic &h) const
    {
        return cost_ + h (v_, t);
    }

    cost_t tiebreaker() const
    {
        return tiebreaker_;
    }

    bool operator == (const AStarEntry& other) const
    {
        return v_ == other.v_;
    }

    virtual point& getPoint()
    {
        return v_;
    }
};

class entryGreaterThan
{
private:
    const point t_;
    const Heuristic *h_;
public:
    entryGreaterThan(const point t, const Heuristic *h) : t_(t), h_(h) {}
    bool operator()(const std::shared_ptr<entry> a, const std::shared_ptr<entry> b) const
    {
        return !a->costLessThan(*b,t_,*h_);
    }
    bool operator()(const entry &a, const entry &b) const
    {
        return !a.costLessThan(b,t_,*h_);
    }
};

#endif
