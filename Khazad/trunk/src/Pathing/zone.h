#ifndef ZONE_HEADER
#define ZONE_HEADER

#include <vector>
//#include <unordered_map>
#include <algorithm>
#include <memory>
#include <stdio.h>

#include "heuristics.h"
#include "entry.h"
#include "grid.h"
#include "Path.h"

class zoneBorderNode;

//interface
class zone
{
public:
    virtual ~zone() {};

    virtual bool equals(const zone *z) const = 0;
    virtual bool contains(const point &p) const = 0;
    virtual zoneBorderNode* get(const point &p) = 0;

    virtual zoneBorderNode* addBorderNode(const point &p, const Heuristic* h)  = 0;
    virtual void removeBorderNode(const zoneBorderNode *deleteme) = 0;
    virtual void connect(zone *other, const point &pthis, const point &pother, cost_t estimate) = 0;
    virtual zoneBorderNode* connect(zoneBorderNode *zbnother, const point &pthis, cost_t estimate) = 0;
    virtual void clearCache(const Heuristic *h) = 0;

    virtual void checkValid() = 0;
//  typedef zoneBorderNode* iterator;

//  virtual iterator begin() = 0;
//  virtual iterator end() = 0;
};

class zoneManager
{
public:
    virtual ~zoneManager() {};

    virtual zone* findContainingZone(const point &p) = 0;
    virtual zoneManager* down()
    {
        return NULL;
    }
    virtual void registerChange(const point &p) = 0;
    virtual void checkValid() = 0;
};

class adjacentNode : public MapCoordinates
{
public:
    zoneBorderNode *node_;
    cost_t cost_;
    std::vector<MapCoordinates> cache_;

    adjacentNode(zoneBorderNode *n, cost_t c);
    void setCache(FullPath *cpath);
};

class zoneBorderNode : public MapCoordinates
{
public:
    zone *owner_;
    std::vector<adjacentNode*> at_; //adjacency table


    typedef std::vector<adjacentNode*>::iterator iterator;

    iterator begin()
    {
        return at_.begin();
    }
    iterator end()
    {
        return at_.end();
    }

    zoneBorderNode(const point &p, zone *owner) : point(p), owner_(owner) {}
    ~zoneBorderNode()
    {
        iterator it = at_.begin();
        while (it != at_.end())
        {
            adjacentNode *an = *it;
            an->node_->removeAdjacentNode(this);
            it = at_.erase(it);
            delete an;
        }
    }

    void invalidateCache(const zone *affected, const Heuristic *h);
    void addAdjacentNode(zoneBorderNode *zbn, cost_t c, const std::vector<point> &cache)
    {
        adjacentNode *an = new adjacentNode(zbn,c);
        an->cache_.insert(an->cache_.begin(),cache.begin(), cache.end());
        at_.push_back(an);
        assert(*an == *zbn);
        //checkValid();
    }

    void removeAdjacentNode(const zoneBorderNode *zbn)
    {
        iterator it = std::find_if(at_.begin(),at_.end(),pointerEqualityPredicate<point>((const point*)zbn));
        assert(it != at_.end());
        adjacentNode* an = *it;
        at_.erase(it);
        delete an;
        //checkValid();
    }

    void checkValid()
    {
        for (iterator it = begin(); it != end(); it++)
        {
            adjacentNode *an = *it;
            if (!(*an == *an->node_))
                fprintf(stderr,"Assertion failed: (%2d,%2d,%2d)->(%2d,%2d,%2d) at (%2d,%2d,%2d)\n",(*an)[0],(*an)[1],(*an)[2],(*an->node_)[0],(*an->node_)[1],(*an->node_)[2], (*this)[0],(*this)[1],(*this)[2]);
            assert(*an == *an->node_);
        }
    }
};

struct ZonedGridGraph : public gridInterface
{
    ZonedGridGraph (const gridInterface *grid, const zone* zone) : grid_(grid), z_(zone) { }

    bool contains(const point &p) const
    {
        return z_->contains(p);
    }
    
    cost_t getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const
    {
        return grid_->getEdgeCost(TestCoords,DirectionType);
    }
    
    virtual int max(unsigned dim) const { return grid_->max(dim); }
    virtual int min(unsigned dim) const { return grid_->min(dim); }

private:
    const zone *z_;
    const gridInterface * grid_;
};

#include "gridzone.h"
#include "cubezone.h"

class AStarZoneEntry : public entry
{
public:
    adjacentNode *node_;
    cost_t cost_;
    cost_t tiebreaker_;
    std::vector<point> path_;

    AStarZoneEntry(adjacentNode *node, cost_t cost) : node_(node), cost_(cost), tiebreaker_(0) { }
    AStarZoneEntry(adjacentNode *node, const AStarZoneEntry &prev, cost_t tiebreaker)
    {
        node_ = node;
        cost_ = prev.cost_+prev.node_->cost_;
        tiebreaker_ = tiebreaker;
        path_.reserve(prev.path_.size()+prev.node_->cache_.size());
        path_.insert(path_.begin(), prev.path_.begin(),prev.path_.end());
        path_.insert(path_.end(), ++(prev.node_->cache_.begin()),prev.node_->cache_.end()); //no duplicating end node
    }

    cost_t value(const point &t, const Heuristic &h) const
    {
        return cost_ + node_->cost_ + h(*node_,t); //previous cost + most recent edge cost + heuristic to dest
    }

    cost_t tiebreaker() const
    {
        return tiebreaker_;
    }

    bool operator == (const AStarZoneEntry& other) const
    {
        return *node_ == *other.node_;
    }
    bool cached()
    {
        return node_->cache_.size() != 0;
    }

    virtual point& getPoint()
    {
        return *node_;
    }
    
};

#endif // ZONE_HEADER
