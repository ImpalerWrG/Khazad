#ifndef TREE_H
#define TREE_H

#include <Actor.h>


class Tree : public Actor
{
    public:
        /** Default constructor */
        Tree(int mat_type, int mat_idx, int x, int y, int z);
        /** Default destructor */
        virtual ~Tree();
        bool Init();
        bool Draw(CameraOrientation Orientation);

    protected:
    private:
    int x, y, z, mat_type, mat_idx;
};

#endif // TREE_H
