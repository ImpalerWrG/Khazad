#ifndef TREE_H
#define TREE_H

#include <Actor.h>

class Model;

class Tree : public Actor
{
    public:
        /** Default constructor */
        Tree(t_matglossPair material, int x, int y, int z, int tiletype);
        /** Default destructor */
        virtual ~Tree();
        bool Init();
        bool Draw();

    protected:
    private:
    int x, y, z;
    t_matglossPair material;
    Model * model;
    // submodels
    int32_t trunkid;
    int32_t leavesid;
    int32_t snowid;
    
    // submodel materials
    int32_t trunkmat;
    int32_t leavesmat_normal;
    int32_t leavesmat_autumn;
    int32_t snowmat;
    
    int type;
};

#endif // TREE_H
