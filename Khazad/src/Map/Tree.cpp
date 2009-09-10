#include <TextureManager.h>
/// FIXME: dfhack paths
#include <stdint.h>
#include <string>
using namespace std;
#include <DFTypes.h>
#include "Tree.h"

Tree::Tree(t_matglossPair material, int x, int y, int z)
: x( x ),y( y ),z( z ),material( material ) { }

Tree::~Tree()
{
    // NOP
}

bool Tree::Draw(CameraOrientation Orientation)
{
    float xa,ya;
    xa = x %16 - 0.5;
    ya = y %16 - 0.5;

    glNormal3f(0.0,0.0,1.0);
    TEXTURE->BindTexturePoint(1, 0,0);
    glVertex3f(xa     ,ya    ,-0.3);
    TEXTURE->BindTexturePoint(1, 0,1);
    glVertex3f(xa     ,ya + 1,-0.3);
    TEXTURE->BindTexturePoint(1, 1,1);
    glVertex3f(xa + 1 ,ya + 1,-0.3);

    TEXTURE->BindTexturePoint(1, 1,1);
    glVertex3f(xa + 1 ,ya + 1 ,-0.3);
    TEXTURE->BindTexturePoint(1, 1,0);
    glVertex3f(xa + 1 ,ya     ,-0.3);
    TEXTURE->BindTexturePoint(1, 0,0);
    glVertex3f(xa     ,ya     ,-0.3);
    return true;
}
