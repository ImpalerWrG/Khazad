#ifndef TREE_HEADER
#define TREE_HEADER

#include <Actor.h>
#include <Map.h>

class Model;

class Tree : public Actor
{

public:

    Tree(int16_t TreeType, MapCoordinates Coordinates, bool isAlive);
    virtual ~Tree();

    bool Init();
    CoolDown Update();

    //bool Draw(CameraOrientation Orientaion);

protected:

    Model* model;

    bool Alive;

    // submodels
    int32_t trunkid;
    int32_t leavesid;

    // submodel materials
    int16_t trunkmat;
    int16_t leavesmat;
};

#endif // TREE_HEADER
