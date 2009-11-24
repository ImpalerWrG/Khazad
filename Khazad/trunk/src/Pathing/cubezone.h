class cubeZoneManager : public zoneManager
{
    std::unordered_map<point,gridZone*,point::hash> zl;
    const unsigned length;
    zoneManager *child;
    const GridGraph* G_;
    const Heuristic* h_;

    typedef std::unordered_map<point,gridZone*,point::hash>::iterator iterator;

public:
    cubeZoneManager(const GridGraph *G, const Heuristic *h, unsigned len, unsigned scale, unsigned minlen) : length(len), G_(G), h_(h)
    {
        if (len/scale >= minlen)
            child = new gridZoneManager(G,h,len/scale,scale,minlen);
        else
            child = NULL;
        createZones(G,h);
    }

    ~cubeZoneManager()
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

    zone* findContainingZone(const point &p)
    {
        point q(p[0]/length,p[1]/length,p[2]/length);
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

    virtual void registerChange(const point &p)
    {
        if (child != NULL)
            child->registerChange(p);
        zone *z = findContainingZone(p);
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
    }

    void createZones(const GridGraph *G, const Heuristic *h)
    {
        zl.clear();
        //zl.reserve(G_->max(0)*G_->max(1)*G_->max(2)/length/length+1);
        for (int z = 0; z < G_->max(2); z++)
        {
            for (int y = 0;  y < G_->max(1); y+=length)
            {
                for (int x = 0; x < G_->max(0); x+=length)
                {
                    point tul(x,y,z), blr(x+length-1,y+length-1,z);
                    gridZone *pz = new gridZone(tul,blr);
                    //create a zone
                    zl[point(x/length,y/length,z/length)] = pz;

                    //for each element in the zone
                    for (int v = 0; v < length; v++)
                        for (int u = 0; u < length; u++)
                            for (int w = 0; w < length; w++)
                            {
                                point p(x+u,y+v,z+w);
                                AddLeavingEdges(pz, p);
                            }

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

    void AddLeavingEdges(zone *pz, const point &p)
    {
        GridGraph::iterator end = G_->end(p);
        for (GridGraph::iterator nit = G_->begin(p); nit != end; ++nit)
        {
            point neigh = *nit;
            assert(G_->edgeCost(p,neigh)>0);
            assert(G_->edgeCost(p,p)>=0);
            assert(G_->edgeCost(neigh,neigh)>0);
            if (!pz->contains(neigh)) //here's an edge leaving the zone
            {
                if (pz->get(p) == NULL)
                    pz->addBorderNode(p,h_);
                gridZone *nz = (gridZone*) findContainingZone(neigh);
                if (nz != NULL)
                {
                    pz->connect(nz,p,neigh,G_->edgeCost(p,neigh));
#ifdef ZONE_DEBUG
                    printf("(%2d,%2d,%2d)->(%2d,%2d,%2d)\n",p[0],p[1],p[2],neigh[0],neigh[1],neigh[2]);
                    pz->checkValid();
                    nz->checkValid();
#endif
                }
            }
        }
    }
};
