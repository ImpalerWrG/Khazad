#include "Building.h"
#include <TextureManager.h>

Building::Building(int x1, int y1, int x2, int y2, int z, int mat_type, int mat_idx, int type)
: x1( x1 ),y1( y1 ),x2( x2 ),y2( y2 ),z( z ),mat_type( mat_type ),mat_idx( mat_idx ),type( type ) { }

Building::~Building()
{
    // NOP
}
bool Building::Draw(CameraOrientation Orientation)
{
    //
    /*

    */
    float xa,xb,ya,yb, r, g, b;
    xa = x1 %16 - 0.5;
    xb = x2 - x1 + 1;
    ya = y1 %16 - 0.5;
    yb = y2 - y1 + 1;
    r = (type % 12345) % 256;
    r /= 256;
    g = (type % 6471) % 256;
    g /= 256;
    b = (type % 27163) % 256;
    b /= 256;
/*    Vector3 Point = Vector3(xa,ya,0);
    glEnd();
    glBegin(GL_LINES);
    SCREEN->DrawCage(Point, xb, yb, 1.0, 0, r,g,b);
    glBegin(GL_TRIANGLES);*/
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBegin(GL_QUADS);
        glColor3f(r, g, b);
        glVertex3f(xa     ,ya     ,-0.4);
        glVertex3f(xa     ,ya + yb,-0.4);
        glVertex3f(xa + xb,ya + yb,-0.4);
        glVertex3f(xa + xb,ya     ,-0.4);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, TEXTURE->getAggragateTexture()); // rebind
    glBegin(GL_TRIANGLES);
    return true;
}
