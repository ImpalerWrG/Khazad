#include <boost/unordered_map.hpp>

class cubeZoneManager : public zoneManager
{
    boost::unordered_map<MapCoordinates, gridZone*, MapCoordinates::hash> zl;

    const unsigned length;
    zoneManager *child;
    const gridInterface* G_;
    const Heuristic* h_;

    typedef boost::unordered_map<MapCoordinates, gridZone*, MapCoordinates::hash>::iterator iterator;

public:
    cubeZoneManager(const gridInterface *G, const Heuristic *h, unsigned len, unsigned scale, unsigned minlen) : length(len), G_(G), h_(h)
    {
        if (len/scale >= minlen)
            child = new gridZoneManager(G,h,len/scale,scale,minlen);
        else
            child = NULL;
        createZones();
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

    zone* findContainingZone(const MapCoordinates &p)
    {
        MapCoordinates q(p[0]/length,p[1]/length,p[2]/length);
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
                    MapCoordinates tul(x,y,z);
                    createZone(tul);

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
        MapCoordinates tul(length*floor(p[0]/length),length*floor(p[1]/length),length*floor(p[2]/length));
        MapCoordinates blr(tul[0]+length-1,tul[1]+length-1,tul[2]+length-1);
        z = pz = new gridZone(tul,blr);
        //create a zone
        zl[MapCoordinates(p[0]/length,p[1]/length,p[2]/length)] = pz;
        //for each element in the zone
        for (int v = 0; v < length; v++)
            for (int u = 0; u < length; u++)
                for (int w = 0; w < length; w++)
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
