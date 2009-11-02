#include <TextureManager.h>
#include <DataManager.h>
#include <TreeManager.h>
#include <Map.h>
#include <Cell.h>


/// FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFTileTypes.h"

#include <Renderer.h>
#include <ModelManager.h>
#include <Model.h>
#include <Tree.h>


Tree::Tree(t_matglossPair material, MapCoordinates NewCoordinates, VegetationType NewType)
{
    Vegetation = NewType;

    MapPosition = NewCoordinates;

    CubePosition.X = MapPosition.X % CELLEDGESIZE;
    CubePosition.Y = MapPosition.Y % CELLEDGESIZE;

    switch(Vegetation)
    {
        case TREE_DEAD:
        case TREE_OK:

        case SAPLING_DEAD:
        case SAPLING_OK:
            material.type = Mat_Wood;
            break;
        case SHRUB_DEAD:
        case SHRUB_OK:
            material.type = Mat_Plant;
            break;
    }

    model = NULL;
    if( material.type == Mat_Wood)
    {
        TreeData* Data = MAP->TreeMan->getTreeDesc(material.index);
        if(Data)
        {
            model = MODEL->getModel(Data->getModelID());
            if(model)
            {
                TreeVariant var = Data->getVariant(0);

                trunkid = model->getSubmodelIndex("trunk");
                leavesid  = model->getSubmodelIndex("leaves");
                snowid = model->getSubmodelIndex("snow");

                trunkmat = var.TrunkMaterialID;
                leavesmat_normal = leavesmat_autumn = var.LeavesMaterialID;
                snowmat = var.SnowMaterialID;
            }
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
        //glDisable(GL_CULL_FACE);
        glPushMatrix();

            glTranslatef(CubePosition.X, CubePosition.Y, -0.5);

            //float scale = 1 + ((float)(rand % 100 - 50)) * 0.002;
//            glScalef(scale,scale,scale);
            //glRotatef( rand , 0,0,1);

            if(trunkid != -1)
            {
                TEXTURE->BindTexture(TEXTURE->MapTexture(trunkmat, TEXTURE->PickImageTexture(trunkmat, SurfaceTypeID)));
                model->Render(trunkid);
            }

            if(leavesid != -1 && Vegetation != TREE_DEAD && Vegetation != SAPLING_DEAD)
            {
                TEXTURE->BindTexture(TEXTURE->MapTexture(leavesmat_normal,  TEXTURE->PickImageTexture(leavesmat_normal, SurfaceTypeID)));
                model->Render(leavesid);
            }
/*
            if(snowid != -1)
            {
                TEXTURE->BindTexture(snowmat);
                model->Render(snowid);
            }
*/
        glPopMatrix();
        //glEnable(GL_CULL_FACE);

        return true;
    }
}
