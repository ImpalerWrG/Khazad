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

struct GridGraph : gridInterface
{
    GridGraph (const gridInterface *grid) : grid_(grid) { }

    typedef MapCoordinates vertex;
    static const unsigned dim = 3;

    cost_t getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const
    {
        return grid_->getEdgeCost(TestCoords,DirectionType);
    }

    virtual zone* getZone() const
    {
        return NULL;
    }
    
    virtual unsigned max(int dim) const { return grid_->max(dim); }

    bool contains(const MapCoordinates &TestCoords) const
    {
        return grid_->contains(TestCoords);
    }
private:
    const gridInterface * grid_;
};

#endif
