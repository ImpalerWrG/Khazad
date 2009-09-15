#ifndef TREE_H
#define TREE_H

#include <Actor.h>


class Tree : public Actor
{
    public:
        /** Default constructor */
        Tree(t_matglossPair material, int x, int y, int z);
        /** Default destructor */
        virtual ~Tree();
        bool Init();
        bool Draw(CameraOrientation Orientation);

    protected:
    private:
    int x, y, z;
    t_matglossPair material;
};

#endif // TREE_H
