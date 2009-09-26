#include <TextureManager.h>
#include <DataManager.h>
/// FIXME: dfhack paths
#include <stdint.h>
#include <string>
using namespace std;
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFTileTypes.h"
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
    /** DRAW **/
    {
        glNormal3f(0.0,0.0,1.0);
        glTexCoord2i(0,1);
        glVertex3f(xa     ,ya    ,-0.3);
        glTexCoord2i(0,0);
        glVertex3f(xa     ,ya + 1,-0.3);
        glTexCoord2i(1,0);
        glVertex3f(xa + 1 ,ya + 1,-0.3);

        glTexCoord2i(1,0);
        glVertex3f(xa + 1 ,ya + 1 ,-0.3);
        glTexCoord2i(1,1);
        glVertex3f(xa + 1 ,ya     ,-0.3);
        glTexCoord2i(0,1);
        glVertex3f(xa     ,ya     ,-0.3);
    }
    return true;
}
