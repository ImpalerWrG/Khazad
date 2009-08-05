#include "Tree.h"
#include <TextureManager.h>

Tree::Tree(int mat_type, int mat_idx, int x, int y, int z)
: x( x ),y( y ),z( z ),mat_type( mat_type ),mat_idx( mat_idx ) { }

Tree::~Tree()
{
    // NOP
}

bool Tree::Draw(CameraOrientation Orientation)
{
    float xa,ya;
    xa = x %16 - 0.5;
    ya = y %16 - 0.5;
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBegin(GL_QUADS);
        glColor3f(0, 0.5, 0);
        glVertex3f(xa    , ya, -0.3);
        glVertex3f(xa    , ya + 1, -0.3);
        glVertex3f(xa + 1, ya + 1, -0.3);
        glVertex3f(xa + 1, ya    , -0.3);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, TEXTURE->getAggragateTexture()); // rebind
    glBegin(GL_TRIANGLES);
    return true;
}
