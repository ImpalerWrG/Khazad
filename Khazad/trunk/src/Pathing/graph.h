/* Copyright Benoit Hudson 2009 */
#ifndef graph_HEADER
#define graph_HEADER


//#include <boost/iterator/iterator_facade.hpp>
#include <unordered_map>
#include <list>


#include "grid.h"

struct zone;

struct PointIterator
//  : public boost::iterator_facade< PointIterator, point,
//             std::random_access_iterator_tag, point >
{
    static const unsigned maxoffset = 3 * 3 * 3; // 3 ** dim

    PointIterator (const gridInterface *g, const point &p) : grid_(g), p_(p), i_(0)
    {
        increment();
    }
    PointIterator () : i_(maxoffset) { }

    point operator*() const
    {
        return dereference();
    }
    point operator++()
    {
        increment();
        return dereference();
    }

    point dereference() const
    {
        unsigned offset = i_;
        point p;
        assert (offset < maxoffset);
        /* this could be a 26-case switch statement instead of a loop with
         * division in it (case 0 is never activated). */
        for (unsigned j = 0; j < 3; ++j)
        {
            switch (offset % 3)
            {
            case 0:
                p.set(j, p_[j]);
                break;
            case 1:
                p.set(j, p_[j] + 1);
                break;
            case 2:
                p.set(j, p_[j] - 1);
                break;
            }
            offset /= 3;
        }
        return p;
    }

    void increment()
    {
        do
        {
            ++i_; // skip the current, go to the next
        }
        while ( (i_ < maxoffset) && ((!grid_->contains(dereference())) // next that is past-end or passable
                                     || (grid_->edgeCost(p_,dereference()) < 0)));
    }

    void decrement()
    {
        do
        {
            --i_;
        }
        while ( (i_ > 0) && ((!grid_->contains(dereference())) // 0 is self, skip it.
                             || (grid_->edgeCost(p_,dereference()) < 0)));
    }

    bool equal (const PointIterator & other) const
    {
        return i_ == other.i_;
    }
    bool operator!=(const PointIterator &other) const
    {
        return !equal(other);
    }

private:
    const gridInterface *grid_;
    const point p_;
    unsigned i_;
};


struct GridGraph : gridInterface
{
    GridGraph (const gridInterface *grid) : grid_(grid) { }

    typedef point vertex;
    static const unsigned dim = 3;

    typedef PointIterator iterator;

    iterator begin(const point &v) const
    {
        return iterator(this, v);
    }
    iterator end(const point) const
    {
        return iterator();
    }


    cost_t edgeCost(const point &a,const point &b) const
    {
        return grid_->edgeCost(a,b);
    }

    virtual zone* getZone() const
    {
        return NULL;
    }

    unsigned max(unsigned dim) const
    {
        return grid_->max(dim);
    }
    bool contains(const point &p) const
    {
        return grid_->contains(p);
    }
private:
    const gridInterface * grid_;
};

#endif
