#ifndef BUILDING_H
#define BUILDING_H

#include <stdafx.h>
#include <Actor.h>


class Building : public Actor
{
    public:
        /** Default constructor */
        Building(int x1, int y1, int x2, int y2, int z, int mat_type, int mat_idx, int type);
        /** Default destructor */
        virtual ~Building();
        bool Init();
        bool Draw(CameraOrientation Orientation);
    protected:
    private:
        int x1, y1, x2, y2, z, mat_type, mat_idx, type;
};

#endif // BUILDING_H
