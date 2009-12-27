#ifndef TREE_HEADER
#define TREE_HEADER

#include <Actor.h>
#include <Map.h>

class Model;

class Tree : public Actor
{

public:

    Tree(Sint16 TreeType, MapCoordinates Coordinates, bool isAlive);
    virtual ~Tree();

    bool Init();
    bool Update();
    bool Draw(CameraOrientation Orientaion);

protected:

    MapCoordinates MapPosition;
    CubeCoordinates CubePosition;

    Model* model;

    bool Alive;

    // submodels
    int32_t trunkid;
    int32_t leavesid;

    // submodel materials
    Sint16 trunkmat;
    Sint16 leavesmat;
};

#endif // TREE_HEADER
