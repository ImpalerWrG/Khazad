/* Copyright Benoit Hudson 2009 */
#ifndef graph_HEADER
#define graph_HEADER

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/unordered_map.hpp>
#include <list>


#include "grid.h"

struct zone;


struct PointIterator
//  : public boost::iterator_facade< PointIterator, point,
//             std::random_access_iterator_tag, point >
{
    static const unsigned maxoffset = 3 * 3 * 3; // 3 ** dim

    PointIterator (const gridInterface *g, const MapCoordinates &p) : grid_(g), p_(p), i_(0)
    {
        increment();
    }
    PointIterator () : i_(maxoffset) { }

    MapCoordinates operator*() const
    {
        return dereference();
    }
    MapCoordinates operator++()
    {
        increment();
        if (i_ < maxoffset)
            return dereference();
        else
            return MapCoordinates(0,0,0);
    }

    MapCoordinates dereference() const
    {
        unsigned offset = i_;
        MapCoordinates p;
        assert (offset < maxoffset);
        // this could be a 26-case switch statement instead of a loop with
         // division in it (case 0 is never activated). */
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
        /*
        do
        {
            ++i_; // skip the current, go to the next
        }
        while ( (i_ < maxoffset) && ((!grid_->contains(dereference())) ))// next that is past-end or passable

     */                             //   || (grid_->edgeCost(p_,dereference()) < 0)));
    }

    /*void decrement() {
      do {
        --i_;
      } while ( (i_ > 0) && ((!grid_->contains(dereference())) // 0 is self, skip it.
          || (grid_->edgeCost(p_,dereference()) < 0)));
    }*/

    bool equal (const PointIterator & other) const
    {
        return i_ == other.i_;
    }
    bool operator!=(const PointIterator &other) const
    {
        return !equal(other);
    }

private:

    const gridInterface* grid_;
    const MapCoordinates p_;
    unsigned i_;
};

struct GridGraph : gridInterface
{
    GridGraph (const gridInterface *grid) : grid_(grid) { }

    typedef MapCoordinates vertex;
    static const unsigned dim = 3;

    typedef PointIterator iterator;

    iterator begin(const MapCoordinates &v) const
    {
        return iterator(this, v);
    }
    iterator end(const MapCoordinates) const
    {
        return iterator();
    }

    cost_t getEdgeCost(const MapCoordinates &a,const MapCoordinates &b) const
    {
        return 0;  //grid_->edgeCost(a,b);
    }

    virtual zone* getZone() const
    {
        return NULL;
    }

    unsigned max(unsigned dim) const
    {
        return 0; //grid_->max(dim);
    }
    bool contains(const MapCoordinates &p) const
    {
        return grid_->contains(p);
    }
private:
    const gridInterface * grid_;
};

#endif
