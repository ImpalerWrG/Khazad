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
        MapCoordinates q(p[AXIS_X]/length,p[AXIS_Y]/length,p[AXIS_Z]/length);
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
          MapCoordinates q(p[AXIS_X]/length,p[AXIS_Y]/length,p[AXIS_Z]);
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
        for (int z = G_->min(AXIS_Z); z < G_->max(AXIS_Z); z++)
        {
            for (int y = G_->min(AXIS_Y);  y < G_->max(AXIS_Y); y+=length)
            {
                for (int x = G_->min(AXIS_X); x < G_->max(AXIS_X); x+=length)
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
        MapCoordinates tul(length*floor(p[AXIS_X]/length),length*floor(p[AXIS_Y]/length),length*floor(p[AXIS_Z]/length));
        MapCoordinates blr(tul[AXIS_X]+length-1,tul[AXIS_Y]+length-1,tul[AXIS_Z]+length-1);
        z = pz = new gridZone(tul,blr);
        //create a zone
        zl[MapCoordinates(p[AXIS_X]/length,p[AXIS_Y]/length,p[AXIS_Z]/length)] = pz;
        //for each element in the zone
        for (unsigned v = 0; v < length; v++)
        {
            for (unsigned u = 0; u < length; u++)
            {
                for (unsigned w = 0; w < length; w++)
                {
                    {
                        MapCoordinates p(tul[AXIS_X]+u,tul[AXIS_Y]+v,tul[AXIS_Z]);
                        AddLeavingEdges(pz, p);
                    }
                }
            }
        }
      }
      return z;
    }

    void AddLeavingEdges(zone *pz, const MapCoordinates &p)
    {
        for (uint8_t i = 0, DirectionType = Direction::ANGULAR_DIRECTIONS[i]; i < NUM_ANGULAR_DIRECTIONS; ++i, DirectionType = Direction::ANGULAR_DIRECTIONS[i])
        {
          if (G_->getEdgeCost(p, DirectionType) >= 0)
          {
            MapCoordinates NeiboringCoordinates = p;
            NeiboringCoordinates.TranslateMapCoordinates(DirectionType);
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
                    pz->connect(nz,p,NeiboringCoordinates,G_->getEdgeCost(p, DirectionType));
#ifdef ZONE_DEBUG
                    printf("(%2d,%2d,%2d)->(%2d,%2d,%2d)\n", p[AXIS_X], p[AXIS_Y], p[AXIS_Z], NeiboringCoordinates[AXIS_X], NeiboringCoordinates[AXIS_Y], NeiboringCoordinates[AXIS_Z]);
                    pz->checkValid();
                    nz->checkValid();
#endif
                }
            }
          }
        }
    }
};
