//basic zone covering a single tile (for debuging)

#include <boost/unordered_map.hpp>


class gridZone : public zone
{
public:
    boost::unordered_map< MapCoordinates, zoneBorderNode*, MapCoordinates::hash> zbn_;
    MapCoordinates tul_, blr_; //top upper left & bottom lower right

    typedef boost::unordered_map<MapCoordinates, zoneBorderNode*, MapCoordinates::hash>::iterator iterator;

    gridZone(const MapCoordinates &tul, const MapCoordinates &blr) : tul_(tul), blr_(blr) { }

    ~gridZone()
    {
        iterator it = zbn_.begin();
        while (it != zbn_.end())
        {
            zoneBorderNode *z = it->second;
            it = zbn_.erase(it);
            delete z;
        }
    }

    bool equals(const zone *z) const
    {
        return (((gridZone*)z)->tul_ == tul_) && (((gridZone*)z)->blr_ == blr_);
    }

    bool contains(const MapCoordinates &p) const
    {
        for (unsigned i = 0; i < 3; ++i)
        {
            if (p[i] > blr_[i])
            {
                return false;
            }
            if (p[i] < tul_[i])
            {
                return false;
            }
        }
        return true;
    }

    zoneBorderNode* get(const MapCoordinates &p)
    {
        if (zbn_.find(p) != zbn_.end())
            return zbn_[p];
        else
            return NULL;
    }

    void removeBorderNode(const zoneBorderNode* deleteme)
    {
        zoneBorderNode* todelete = zbn_[(const MapCoordinates)*deleteme];
        zbn_.erase((const MapCoordinates)*deleteme);
        assert(todelete == deleteme);
        //fprintf(stderr,"Deleting (%2d,%2d,%2d)\n",(*todelete)[0],(*todelete)[1],(*todelete)[2]);
        delete todelete;

#ifdef ZONE_DEBUG
        checkValid();
#endif
    }

    zoneBorderNode* addBorderNode(const MapCoordinates &p, const Heuristic* h)
    {
        assert(zbn_.find(p)==zbn_.end());
        std::vector<point> cache;

        zoneBorderNode *r = new zoneBorderNode(p,this); //not freed?
        for (iterator it = zbn_.begin(); it != zbn_.end(); it++)
        {
            zoneBorderNode *b = it->second;
            b->addAdjacentNode(r,(*h)(*b,*r),cache);
            r->addAdjacentNode(b,(*h)(*r,*b),cache);
        }
        zbn_[p] = r;
#ifdef ZONE_DEBUG
        checkValid();
#endif
        return r;
    }

    void connect(zone *other, const MapCoordinates &pthis, const MapCoordinates &pother, cost_t cost)
    {
        zoneBorderNode *z = get(pthis);
        assert(z != NULL);
        connect(other->connect(z,pother,cost),pthis,cost);
    }

    zoneBorderNode* connect(zoneBorderNode *zbnother, const MapCoordinates &pthis, cost_t cost)
    {
        zoneBorderNode *z = get(pthis);
        assert(z != NULL);
        std::vector<point> cache;
        cache.push_back(pthis);
        cache.push_back((point)*zbnother);
        z->addAdjacentNode(zbnother,cost,cache);
        return z;
    }

    void clearCache(const Heuristic *h)
    {
        for (iterator it = zbn_.begin(); it != zbn_.end(); it++)
        {
            zoneBorderNode *b = it->second;
            b->invalidateCache(this,h);
        }
    }

    void checkValid()
    {
        for (iterator it = zbn_.begin(); it != zbn_.end(); it++)
        {
            zoneBorderNode *b = it->second;
            b->checkValid();
        }
    }

};

class gridZoneManager : public zoneManager
{
    boost::unordered_map<point, gridZone*, point::hash> zl;
    const unsigned length;
    zoneManager *child;
    const gridInterface* G_;
    const Heuristic* h_;

    typedef boost::unordered_map<MapCoordinates, gridZone*, MapCoordinates::hash>::iterator iterator;

public:
    gridZoneManager(const gridInterface *G, const Heuristic *h, unsigned len, unsigned scale, unsigned minlen) : length(len), G_(G), h_(h)
    {
        if (len/scale >= minlen)
            child = new gridZoneManager(G,h,len/scale,scale,minlen);
        else
            child = NULL;
        createZones();
    }

    ~gridZoneManager()
    {
        iterator it = zl.begin();
        while (it != zl.end())
        {
            gridZone *bz = it->second;
            it = zl.erase(it);
            delete bz;
        }
        if (child != NULL)
            delete child;
    }

    zone* findContainingZone(const MapCoordinates &p)
    {
        MapCoordinates q(p[0]/length,p[1]/length,p[2]);
        if (zl.find(q)!= zl.end())
            return zl[q];
        else
            return NULL;
    }

    void checkValid()
    {
        for (iterator it = zl.begin(); it != zl.end(); it++)
        {
            gridZone* z = it->second;
            z->checkValid();
        }
        if (child != NULL)
            child->checkValid();
    }

    virtual zoneManager* down()
    {
        return child;
    }

    virtual void registerChange(const MapCoordinates &p)
    {
        if (child != NULL)
            child->registerChange(p);
        zone *z = findContainingZone(p);
        if (z == NULL)
          z = createZone(p);
        zoneBorderNode *zbn = z->get(p);
        if (zbn != NULL)
            z->removeBorderNode(zbn);
        z->clearCache(h_);
        AddLeavingEdges(z,p);

#ifdef ZONE_DEBUG
        for (iterator it = zl.begin(); it != zl.end(); it++)
        {
            gridZone* z = *it;
            z->checkValid();
        }
#endif
        if (((gridZone*)z)->zbn_.size() == 0)
        {
          //this is an empty zone, so we can probably delete it
          MapCoordinates q(p[0]/length,p[1]/length,p[2]);
          iterator it = zl.find(q);
          assert(it != zl.end());
          zl.erase(it);
          delete z;
        }
    }

    void createZones()
    {
        zl.clear();
        //zl.reserve(G_->max(0)*G_->max(1)*G_->max(2)/length/length+1);
        for (int z = G_->min(2); z < G_->max(2); z++)
        {
            for (int y = G_->min(1);  y < G_->max(1); y+=length)
            {
                for (int x = G_->min(0); x < G_->max(0); x+=length)
                {
                    MapCoordinates p(x,y,z);
                    createZone(p);
                }
            }
        }
#ifdef ZONE_DEBUG
        for (iterator it = zl.begin(); it != zl.end(); it++)
        {
            gridZone* z = *it;
            z->checkValid();
        }
#endif
    }
    
    zone *createZone(MapCoordinates p)
    {
      gridZone *pz;
      zone *z = findContainingZone(p);
      if ((G_->contains(p)) && (z == NULL))
      {
        MapCoordinates tul(length*floor(p[0]/length),length*floor(p[1]/length),p[2]);
        MapCoordinates blr(tul[0]+length-1,tul[1]+length-1,tul[2]);
        z = pz = new gridZone(tul,blr);
        //create a zone
        zl[point(p[0]/length,p[1]/length,p[2])] = pz;
        //for each element in the zone
        for (int v = 0; v < length; v++)
            for (int u = 0; u < length; u++)
                {
                    MapCoordinates p(tul[0]+u,tul[1]+v,tul[2]);
                    AddLeavingEdges(pz, p);
                }
      }
      return z;
    }



    void AddLeavingEdges(zone *pz, const MapCoordinates &p)
    {
        for (Direction dir = ANGULAR_DIRECTIONS_START; dir < NUM_ANGULAR_DIRECTIONS; ++dir)
        {
          if (G_->getEdgeCost(p,dir) >= 0)
          {
            MapCoordinates NeiboringCoordinates = p;
            NeiboringCoordinates.TranslateMapCoordinates(dir);
            //assert(G_->edgeCost(p,neigh)>0);
            //assert(G_->edgeCost(p,p)>=0);
            //assert(G_->edgeCost(neigh,neigh)>0);
            if (!pz->contains(NeiboringCoordinates)) //here's an edge leaving the zone
            {
                if (pz->get(p) == NULL)
                    pz->addBorderNode(p,h_);
                gridZone *nz = (gridZone*) findContainingZone(NeiboringCoordinates);
                if (nz != NULL)
                {
                    pz->connect(nz,p,NeiboringCoordinates,G_->getEdgeCost(p,dir));
#ifdef ZONE_DEBUG
                    printf("(%2d,%2d,%2d)->(%2d,%2d,%2d)\n",p[0],p[1],p[2],NeiboringCoordinates[0],NeiboringCoordinates[1],NeiboringCoordinates[2]);
                    pz->checkValid();
                    nz->checkValid();
#endif
                }
            }
          }
        }
    }
};
