#include <TextureManager.h>
#include <DataManager.h>
#include <TreeManager.h>
#include <Map.h>


/// FIXME: dfhack paths
#include <stdint.h>
#include <string>
using namespace std;
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFTileTypes.h"


#include "Renderer.h"
#include <ModelManager.h>
#include <Model.h>
#include "Tree.h"


Tree::Tree(t_matglossPair material, int x, int y, int z, int tiletype)
: x( x ),y( y ),z( z ),material( material )
{
    type = getVegetationType(tiletype);
    switch(type)
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
        TreeData * tr = MAP->TreeMan->getTreeDesc( material.index );
        if(tr)
        {
            model = RENDERER->ModelMan->LoadOBJModel(Path(tr->getModel()));
            if(model)
            {
                TreeVariant var = tr->getVariant(0);
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
    float xa,ya;
    xa = x %16 - 0.5;
    ya = y %16 - 0.5;
    int32_t texture;
    // some fake stuff
    Cube * testCube = MAP->getCube(x,y,z);
    if(!testCube || !RENDERER->isCubeDrawn(testCube))
    {
        return false;
    }
    if(!model)
    {
/*
        switch(type)
        {
            case TREE_DEAD:
            case SAPLING_DEAD:
                texture = DATA->getLabelIndex("MATERIAL_DEAD_TREE");
                break;
            case TREE_OK:
            case SAPLING_OK:
            default:
                texture = DATA->getLabelIndex("MATERIAL_TREE");
                break;
            case SHRUB_DEAD:
                texture = DATA->getLabelIndex("MATERIAL_DEAD_SHRUB");
                break;
            case SHRUB_OK:
                texture = DATA->getLabelIndex("MATERIAL_SHRUB");
                break;
        }
        TEXTURE->BindTexture(texture);
        glEnable(GL_BLEND);
        glBegin(GL_TRIANGLES);
        {
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2i(0,1);
            glVertex3f(xa     ,ya    ,-0.45);
            glTexCoord2i(0,0);
            glVertex3f(xa     ,ya + 1,-0.45);
            glTexCoord2i(1,0);
            glVertex3f(xa + 1 ,ya + 1,-0.45);

            glTexCoord2i(1,0);
            glVertex3f(xa + 1 ,ya + 1 ,-0.45);
            glTexCoord2i(1,1);
            glVertex3f(xa + 1 ,ya     ,-0.45);
            glTexCoord2i(0,1);
            glVertex3f(xa     ,ya     ,-0.45);
        }
        glEnd();
        glDisable(GL_BLEND);
*/
    }
    // some real stuff
    else
    {
        glDisable(GL_CULL_FACE);
        glPushMatrix();

          //  int64_t rand = x * 1000 + y * 413 + z* 11;
            glTranslatef(xa + 0.5, ya + 0.5, -0.5);

            static Sint16 SurfaceTypeID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
            //float scale = 1 + ((float)(rand % 100 - 50)) * 0.002;
//            glScalef(scale,scale,scale);
            //glRotatef( rand , 0,0,1);

            if(trunkid != -1)
            {
                TEXTURE->BindTexture(TEXTURE->MapTexture(trunkmat, SurfaceTypeID));
                model->Render(trunkid);
            }

            if(leavesid != -1 && type !=TREE_DEAD && type != SAPLING_DEAD)
            {
                TEXTURE->BindTexture(TEXTURE->MapTexture(leavesmat_normal, SurfaceTypeID));
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
        glEnable(GL_CULL_FACE);
    }
    return true;
}
