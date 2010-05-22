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

int Tree::Update()
{
    return 1000;
}

/*
bool Tree::Draw(CameraOrientation Orientaion)
{
    static int16_t SurfaceTypeID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");

    if(!RENDERER->isCubeDrawn(LocationCoordinates))
    {
        return false;
    }

    if(model != NULL)
    {
        glPushMatrix();

            glTranslatef(CurrentCubeCoordinates.X, CurrentCubeCoordinates.Y, -HALFCUBE);

            //float scale = 1 + ((float)(rand % 100 - 50)) * 0.002;
            //glScalef(scale,scale,scale);
            //glRotatef( rand , 0,0,1);

            if(trunkid != -1)
            {
                TEXTURE->BindTexture(TEXTURE->MapTexture(trunkmat, TEXTURE->PickImageTexture(trunkmat, SurfaceTypeID)));
                model->Render(trunkid);
            }

            if(leavesid != -1 && Alive)
            {
                TEXTURE->BindTexture(TEXTURE->MapTexture(leavesmat,  TEXTURE->PickImageTexture(leavesmat, SurfaceTypeID)));
                model->Render(leavesid);
            }

        glPopMatrix();

        return true;
    }
}
*/
