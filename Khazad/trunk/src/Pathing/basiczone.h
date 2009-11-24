//basic zone covering a single tile (for debuging)
class basicZone : public zone
{
public:
    zoneBorderNode p_;

    basicZone(const point &p) : p_(p,this) {}

    bool equals(const zone *z) const
    {
        return ((basicZone*)z)->p_ == p_;
    }

    bool contains(const point &p) const
    {
        return p == p_;
    }

    zoneBorderNode* get(const point &p)
    {
        if (p == p_)
            return &p_;
        else
            return NULL;
    }

    void removeBorderNode(const zoneBorderNode* deleteme)
    {
    }

    zoneBorderNode* addBorderNode(const point &p, const Heuristic* h)
    {
        return NULL;
    }

    void connect(zone *other, const point &pthis, const point &pother, cost_t cost)
    {
        assert(p_ == pthis);
        connect(other->connect(&p_,pother,cost),pthis,cost);
    }

    zoneBorderNode* connect(zoneBorderNode *zbnother, const point &pthis, cost_t cost)
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

    zone* findContainingZone(const point &p)
    {
        for (iterator it = zl.begin(); it != zl.end(); it++)
            if ((*it)->contains(p))
                return *it;
        return NULL;
    }

    virtual void registerChange(const point &p)
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
                    point p(x,y,z);
                    if ((G->edgeCost(p,p) > 0) && (findContainingZone(p) == NULL))
                    {
                        //create a zone
                        basicZone *pbz = new basicZone(p);
                        zl.push_back(pbz);

                        //connect zone to others...
                        GridGraph::iterator end = G->end(p);
                        for (GridGraph::iterator nit = G->begin(p); nit != end; ++nit)
                        {
                            point neigh = *nit;
                            basicZone *nz = (basicZone*) findContainingZone(neigh);
                            if (nz != NULL)
                            {
                                pbz->connect(nz,p,neigh,(*h)(p,neigh));
#ifdef ZONE_DEBUG
                                printf("(%2d,%2d,%2d)->(%2d,%2d,%2d)\n",p[0],p[1],p[2],neigh[0],neigh[1],neigh[2]);
                                pbz->checkValid();
                                nz->checkValid();
#endif
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
