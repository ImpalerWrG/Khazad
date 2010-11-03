#include <Tree.h>
//#include <TextureManager.h>
#include <DataManager.h>
#include <Map.h>

//#include <ModelManager.h>
//#include <Model.h>


Tree::Tree(int16_t TreeType, MapCoordinates SpawnLocation, bool isAlive)
{
    Alive = isAlive;
    setLocation(SpawnLocation);

    model = NULL;

    TreeData* Data = DATA->getTreeData(TreeType);
    if(Data != NULL)
    {
        //model = MODEL->getModel(Data->getModelID());
        if(model)
        {
            //trunkid = model->getSubmodelIndex("trunk");
            //leavesid  = model->getSubmodelIndex("leaves");

            //trunkmat = Data->getTrunkMaterialID();
            //leavesmat = Data->getLeavesMaterialID();
        }
    }
}

Tree::~Tree()
{
    // NOP
}

bool Tree::Init()
{
    return true;
}

CoolDown Tree::Update()
{
    return 1000;
}
