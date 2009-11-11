#include <stdafx.h>

#include <Tree.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <Map.h>

#include <ModelManager.h>
#include <Model.h>


Tree::Tree(Sint16 TreeType, MapCoordinates NewCoordinates, bool isAlive)
{
    Alive = isAlive;

    MapPosition = NewCoordinates;

    CubePosition.X = MapPosition.X % CELLEDGESIZE;
    CubePosition.Y = MapPosition.Y % CELLEDGESIZE;

    model = NULL;

    TreeData* Data = DATA->getTreeData(TreeType);
    if(Data != NULL)
    {
        model = MODEL->getModel(Data->getModelID());
        if(model)
        {
            trunkid = model->getSubmodelIndex("trunk");
            leavesid  = model->getSubmodelIndex("leaves");

            trunkmat = Data->getTrunkMaterialID();
            leavesmat = Data->getLeavesMaterialID();
        }
    }
}

Tree::~Tree()
{
    // NOP
}

bool Tree::Draw()
{
    static Sint16 SurfaceTypeID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");

    if(!RENDERER->isCubeDrawn(MapPosition))
    {
        return false;
    }
    if(model != NULL)
    {
        glPushMatrix();

            glTranslatef(CubePosition.X, CubePosition.Y, -HALFCUBE);

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
