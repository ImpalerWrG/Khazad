#ifndef TREE_HEADER
#define TREE_HEADER

#include <Actor.h>
#include <Map.h>
#include <DFTypes.h>
#include "../../dfhack/library/DFTileTypes.h"

class Model;

class Tree : public Actor
{

public:
        Tree(t_matglossPair material, int x, int y, int z, VegetationType Type);
        virtual ~Tree();

        bool Init();
        bool Draw();

protected:

    MapCoordinates MapPosition;
    CubeCoordinates CubePosition;

    t_matglossPair material;
    Model* model;
    // submodels
    int32_t trunkid;
    int32_t leavesid;
    int32_t snowid;

    // submodel materials
    int32_t trunkmat;
    int32_t leavesmat_normal;
    int32_t leavesmat_autumn;
    int32_t snowmat;

    VegetationType Vegetation;
};

#endif // TREE_HEADER
