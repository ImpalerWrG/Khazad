//basic zone covering a single tile (for debuging)
class basicZone : public zone
{
public:
    zoneBorderNode p_;

    basicZone(const MapCoordinates &p) : p_(p,this) {}

    bool equals(const zone *z) const
    {
        return ((basicZone*)z)->p_ == p_;
    }

    bool contains(const MapCoordinates &p) const
    {
        return p == p_;
    }

    zoneBorderNode* get(const MapCoordinates &p)
    {
        if (p == p_)
            return &p_;
        else
            return NULL;
    }

    void removeBorderNode(const zoneBorderNode* deleteme)
    {
    }

    zoneBorderNode* addBorderNode(const MapCoordinates &p, const Heuristic* h)
    {
        return NULL;
    }

    void connect(zone *other, const MapCoordinates &pthis, const MapCoordinates &pother, cost_t cost)
    {
        assert(p_ == pthis);
        connect(other->connect(&p_,pother,cost),pthis,cost);
    }

    zoneBorderNode* connect(zoneBorderNode *zbnother, const MapCoordinates &pthis, cost_t cost)
    {
        assert(p_ == pthis);
        std::vector<point> cache;
        cache.push_back(pthis);
        cache.push_back((point)*zbnother);
        p_.addAdjacentNode(zbnother,cost,cache);
        return &p_;
    }

    void clearCache(const Heuristic *h)
    {
        p_.invalidateCache(this,h);
    }

    void checkValid()
    {
        p_.checkValid();
    }

    //typedef std::vector<zoneBorderNode>::const_iterator iterator;

    //iterator begin() { return borderNode_.begin(); }
    //iterator end() { return borderNode_.end(); }
};


class basicZoneManager : public zoneManager
{
    std::vector<basicZone*> zl;
    typedef std::vector<basicZone*>::iterator iterator;
    const GridGraph* G_;
    const Heuristic* h_;

public:
    basicZoneManager(const GridGraph* G, Heuristic* h) : G_(G), h_(h)
    {
        createZones(G,h);
    }

    ~basicZoneManager()
    {
        iterator it = zl.begin();
        while (it != zl.end())
        {
            basicZone *bz = *it;
            it = zl.erase(it);
            delete bz;
        }
    }

    void checkValid()
    {
        for (iterator it = zl.begin(); it != zl.end(); it++)
        {
            basicZone* z = *it;
            z->checkValid();
        }
    }

    zone* findContainingZone(const MapCoordinates &p)
    {
        for (iterator it = zl.begin(); it != zl.end(); it++)
            if ((*it)->contains(p))
                return *it;
        return NULL;
    }

    virtual void registerChange(const MapCoordinates &p)
    {
        zone *z = findContainingZone(p);
        zoneBorderNode *zbn = z->get(p);
        z->clearCache(h_);
    }

    void createZones(const GridGraph *G, const Heuristic *h)
    {
        zl.clear();
        zl.reserve(G->max(0)*G->max(1)*G->max(2));
        for (int z = 0; z < G->max(2); z++)
        {
            for (int y = 0;  y < G->max(1); y++)
            {
                for (int x = 0; x < G->max(0); x++)
                {
                    MapCoordinates p(x,y,z);
                    if (/*G->edgeCost(p,p) > 0) && */(findContainingZone(p) == NULL))
                    {
                        //create a zone
                        basicZone *pbz = new basicZone(p);
                        zl.push_back(pbz);

                        //connect zone to others...
                        for (Direction dir = ANGULAR_DIRECTIONS_START; dir < NUM_ANGULAR_DIRECTIONS; ++dir)
                        {
                          if (G_->getEdgeCost(p,dir) >= 0)
                          {
                            MapCoordinates NeiboringCoordinates = p;
                            NeiboringCoordinates.TranslateMapCoordinates(dir);
                            basicZone *nz = (basicZone*) findContainingZone(NeiboringCoordinates);
                            if (nz != NULL)
                            {
                                pbz->connect(nz,p,NeiboringCoordinates,(*h)(p, NeiboringCoordinates));
#ifdef ZONE_DEBUG
                                printf("(%2d,%2d,%2d)->(%2d,%2d,%2d)\n",p[0],p[1],p[2],NeiboringCoordinates[0],NeiboringCoordinates[1],NeiboringCoordinates[2]);
                                pbz->checkValid();
                                nz->checkValid();
#endif
                            }
                          }
                        }


                    }
                }
            }
        }

#ifdef ZONE_DEBUG
        for (iterator it = zl.begin(); it != zl.end(); it++)
        {
            (*it)->checkValid();
        }
#endif
    }
};
