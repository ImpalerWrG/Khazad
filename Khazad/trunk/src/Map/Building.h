#ifndef BUILDING_H
#define BUILDING_H

#include <stdafx.h>
#include <Actor.h>
#include <DFTypes.h>


class Building : public Actor
{
    public:
        /** Default constructor */
        Building(int x1, int y1, int x2, int y2, int z, t_matglossPair material, int type);
        /** Default destructor */
        virtual ~Building();
        bool Init();
        bool Draw();
    protected:
    private:
        int x1, y1, x2, y2, z, type;
        t_matglossPair material;
};

#endif // BUILDING_H
