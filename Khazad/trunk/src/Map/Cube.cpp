#include <stdafx.h>

#include <Cube.h>
#include <Cell.h>
#include <Map.h>
#include <Random.h>
#include <TextureManager.h>
#include <DataManager.h>

Cube::Cube()
{
    //setType(CUBE_ACTOR);

    Visible = false;
    Hidden = true;
    Initalized = false;
    data.whole = 0;

    Facets[FACET_TOP] = false;
    Facets[FACET_BOTTOM] = false;
    Facets[FACET_NORTH] = false;
    Facets[FACET_SOUTH] = false;
    Facets[FACET_EAST] = false;
    Facets[FACET_WEST] = false;

    setPosition(0.0, 0.0, 0.0);
    MAP->ChangeCubeCount(1);
}

void Cube::SetOwner(Cell* NewOwner, Uint8 X, Uint8 Y)
{
    Owner = NewOwner;
    CellX = X % CELLEDGESIZE;
    CellY = Y % CELLEDGESIZE;

    Vector3 OwnerPosition = Owner->getPosition();

    float RealX = OwnerPosition.x - (float)(CELLEDGESIZE / 2) + (float)X + (float)HALFCUBE;
    float RealY = OwnerPosition.y - (float)(CELLEDGESIZE / 2) + (float)Y + (float)HALFCUBE;

	setPosition(RealX, RealY, OwnerPosition.z);
}

Cube::~Cube()
{
    MAP->ChangeCubeCount(-1);

    if(Initalized)
    {
        MAP->ChangeInitedCubeCount(-1);
    }
}

bool Cube::Init(Uint16 MaterialType)
{
    Initalized = true;

    Material = MaterialType;

    MAP->ChangeInitedCubeCount(1);

    return true;
}
/*
bool Cube::DrawLiquid(float xTranslate, float yTranslate)
{
    Uint16 texture;
    Uint16 Liquid= data.liquid;
    Uint16 LiquidType= data.liquidtype;
    if(Liquid > 0)
    {
        // bind the right texture
        if(LiquidType == 1) // magma
        {
            glColor4f(1, 1, 1, 0.3);
            texture = DATA->getLabelIndex("MATERIAL_LAVA");
        }
        else
        {
            glColor4f(0.5, 0.5, 0.5, 0.7);
            texture = DATA->getLabelIndex("MATERIAL_WATER");
        }
        TEXTURE->BindTexture(texture);
        float xx= xTranslate - 0.5;
        float yy = yTranslate - 0.5;
        float z = 0.141 * Liquid - 0.5;
        float zt = 0.141 * Liquid;
        // draw quad in the appropriate height
        {
            glNormal3f(0.0,0.0,1.0);
            glTexCoord2f(0,0);
            glVertex3f(xx     ,yy    ,z);
            glTexCoord2f(0,1);
            glVertex3f(xx     ,yy + 1,z);
            glTexCoord2f(1,1);
            glVertex3f(xx + 1 ,yy + 1,z);

            glTexCoord2f(1,1);
            glVertex3f(xx + 1 ,yy + 1 ,z);
            glTexCoord2f(1,0);
            glVertex3f(xx + 1 ,yy     ,z);
            glTexCoord2f(0,0);
            glVertex3f(xx     ,yy     ,z);
        }
        Cube * north = getAdjacentCube(FACET_NORTH);
        Cube * east = getAdjacentCube(FACET_EAST);
        Cube * west = getAdjacentCube(FACET_WEST);
        Cube * south = getAdjacentCube(FACET_SOUTH);
        float nl = 0;
        if(north)
        {
            nl = north->getLiquid();
        }
        if (nl < Liquid)
        {
            float z2 =  (0.141 * nl) - 0.5;
            float zt2 =  (0.141 * nl);
            // draw a quad to patch hole north
            glTexCoord2f(0,zt2);
            glVertex3f(xx +1     ,yy    ,z2);
            glTexCoord2f(1,zt2);
            glVertex3f(xx     ,yy     ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx    ,yy    ,z);

            glTexCoord2f(0,zt2);
            glVertex3f(xx +1     ,yy    ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx    ,yy    ,z);
            glTexCoord2f(0,zt);
            glVertex3f(xx +1 ,yy    ,z);

        }

        nl = 0;
        if(east)
        {
            nl = east->getLiquid();
        }
        if (nl < Liquid)
        {
            float z2 =  0.141 * nl - 0.5;
            float zt2 =  (0.141 * nl);
            // draw a quad to patch hole east
            glTexCoord2f(0,zt2);
            glVertex3f(xx +1     ,yy +1    ,z2);
            glTexCoord2f(1,zt2);
            glVertex3f(xx +1     ,yy    ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx +1     ,yy     ,z);

            glTexCoord2f(0,zt2);
            glVertex3f(xx +1     ,yy +1    ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx +1     ,yy     ,z);
            glTexCoord2f(0,zt);
            glVertex3f(xx +1     ,yy +1    ,z);
        }
        if(south)
        {
            nl = south->getLiquid();
        }
        if (nl < Liquid)
        {
            float z2 =  0.141 * nl - 0.5;
            float zt2 =  (0.141 * nl);
            // draw a quad to patch hole south
            glTexCoord2f(0,zt2);
            glVertex3f(xx      ,yy +1    ,z2);
            glTexCoord2f(1,zt2);
            glVertex3f(xx +1     ,yy +1    ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx +1   ,yy +1     ,z);

            glTexCoord2f(0,zt2);
            glVertex3f(xx      ,yy +1    ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx +1     ,yy +1     ,z);
            glTexCoord2f(0,zt);
            glVertex3f(xx      ,yy +1    ,z);
        }
        if(west)
        {
            nl = west->getLiquid();
        }
        if (nl < Liquid)
        {
            float z2 =  0.141 * nl - 0.5;
            float zt2 =  (0.141 * nl);
            // draw a quad to patch hole west
            glTexCoord2f(0,zt2);
            glVertex3f(xx      ,yy    ,z2);
            glTexCoord2f(1,zt2);
            glVertex3f(xx      ,yy +1    ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx   ,yy +1     ,z);

            glTexCoord2f(0,zt2);
            glVertex3f(xx      ,yy     ,z2);
            glTexCoord2f(1,zt);
            glVertex3f(xx      ,yy +1     ,z);
            glTexCoord2f(0,zt);
            glVertex3f(xx      ,yy     ,z);
        }
    }
}
*/
void Cube::setLiquid(Uint8 liquidtype,Uint8 NewValue)
{
    data.liquidtype=liquidtype;
    data.liquid = NewValue;
    if(NewValue > 0)
        Owner->setLiquidActive(true);
}

bool Cube::setMaterial(Uint16 MaterialType)
{
    Initalized = true;
    Material = MaterialType;
}

Cube* Cube::getAdjacentCube(Facet Type)
{
    Sint32 x = Position.x;
    Sint32 y = Position.y;
    Sint32 z = Position.z;

    switch(Type)
    {
        case FACET_TOP:
            z += 1;
            break;
        case FACET_BOTTOM:
            z -= 1;
            break;
        case FACET_NORTH:
            y -= 1;
            break;
        case FACET_EAST:
            x += 1;
            break;
        case FACET_SOUTH:
            y += 1;
            break;
        case FACET_WEST:
            x -= 1;
            break;
    }

    return MAP->getCube(x, y, z);
}

Cube* Cube::getNeighborCube(Direction Type)
{
    Sint32 x = Position.x;
    Sint32 y = Position.y;
    Sint32 z = Position.z;

    switch(Type)
    {
        case DIRECTION_NORTH:
            y -= 1;
            break;
        case DIRECTION_EAST:
            x += 1;
            break;
        case DIRECTION_SOUTH:
            y += 1;
            break;
        case DIRECTION_WEST:
            x -= 1;
            break;

        case DIRECTION_NORTHEAST:
            y -= 1;
            x += 1;
            break;
        case DIRECTION_SOUTHEAST:
            y += 1;
            x += 1;
            break;
        case DIRECTION_SOUTHWEST:
            y += 1;
            x -= 1;
            break;
        case DIRECTION_NORTHWEST:
            y -= 1;
            x -= 1;
            break;

        case DIRECTION_UP:
            z += 1;
            break;
        case DIRECTION_DOWN:
            z -= 1;
            break;
    }

    return MAP->getCube(x, y, z);
}

Cell* Cube::getAdjacentCell(Facet Type)
{
    Sint32 x = Position.x;
    Sint32 y = Position.y;
    Sint32 z = Position.z;

    switch(Type)
    {
        case FACET_TOP:
            z += 1;
            break;
        case FACET_BOTTOM:
            z -= 1;
            break;
        case FACET_NORTH:
            y -= 1;
            break;
        case FACET_EAST:
            x += 1;
            break;
        case FACET_SOUTH:
            y += 1;
            break;
        case FACET_WEST:
            x -= 1;
            break;
    }

    return MAP->getCubeOwner(x, y, z);
}

bool Cube::Update()
{
	return true;
}
/*
//fixme: !!total!! waste of CPU cycles
Vector3 Cube::ConvertSpacialPoint(SpacialPoint Point)
{
    switch(Point)
    {
        case SPACIAL_POINT_CENTER:
            return Vector3(0.0, 0.0, 0.0);

        case SPACIAL_POINT_NORTH_TOP:
            return Vector3(-0.5, -0.5, 0.5);

        case SPACIAL_POINT_EAST_TOP:
            return Vector3(0.5, -0.5, 0.5);

        case SPACIAL_POINT_SOUTH_TOP:
            return Vector3(0.5, 0.5 , 0.5);

        case SPACIAL_POINT_WEST_TOP:
            return Vector3(-0.5, 0.5, 0.5);

        case SPACIAL_POINT_NORTH_BOTTOM:
            return Vector3(-0.5, -0.5, -0.5);

        case SPACIAL_POINT_EAST_BOTTOM:
            return Vector3(0.5, -0.5, -0.5);

        case SPACIAL_POINT_SOUTH_BOTTOM:
            return Vector3(0.5, 0.5, -0.5);

        case SPACIAL_POINT_WEST_BOTTOM:
            return Vector3(-0.5, 0.5, -0.5);

        Default:
            return Vector3(0.5, 0.5  -0.5);
    }

    return Vector3(0.0, 0.0, 0.0);
}
*/
/*bool Cube::Draw(CameraOrientation Orientation, float xTranslate, float yTranslate)
{
    return false;
}*/

void Cube::Dig()
{
    setHidden(false);
    if(isSolid())
    {
        // set to floor
        setGeometry(GEOM_SLOPE);
        // reveal tiles around
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            if(DirectionType != DIRECTION_DOWN)
            {
                Cube* NeighborCube = getNeighborCube(DirectionType);
                if(NeighborCube != NULL && NeighborCube->isHidden())
                {
                    NeighborCube->setHidden(false);
                }
            }
        }
    }
    else if(isSlope())
    {
        // set to floor
        setGeometry(GEOM_FLOOR);
        // done
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            Cube* NeighborCube = getNeighborCube(DirectionType);
            if(NeighborCube != NULL && NeighborCube->isHidden())
            {
                NeighborCube->setHidden(false);
            }
        }
    }
    else if(isFloor())
    {
        // set empty (deleting cube would be ideal)
        setGeometry(GEOM_EMPTY);
        // reveal one tile under
        Cube * LowerCube = getNeighborCube(DIRECTION_DOWN);
        if(LowerCube)
        {
            LowerCube->setHidden(false);
        }
    }
    // update draw list of parent cell(s)
    getCellOwner()->setNeedsRedraw(true);
    for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
    {
        Cube* NeighborCube = getNeighborCube(DirectionType);

        if(NeighborCube != NULL)
        {
            NeighborCube->getCellOwner()->setNeedsRedraw(true);
        }
    }
}

void Cube::setGeometry(geometry_type NewValue)
{
    data.geometry = NewValue;
    //TODO: track and change tile type
    // update faces
    switch (data.geometry)
    {
        case GEOM_SLOPE:
            // block bottom so that we don't see the wall-tops below the slope
            Facets[FACET_BOTTOM] = true;
            Owner->setActive(true);

        case GEOM_EMPTY:
            Owner->setActive(true);
            break;

        case GEOM_FLOOR:
            Facets[FACET_BOTTOM] = true;
            Owner->setActive(true);
            break;

        case GEOM_WALL:
            Facets[FACET_TOP] = true;
            Facets[FACET_NORTH] = true;
            Facets[FACET_SOUTH] = true;
            Facets[FACET_EAST] = true;
            Facets[FACET_WEST] = true;

            Owner->setActive(true);
            Owner->setTopActive(true);
            break;
    }
}

bool Cube::hasFace(Facet FacetType)
{
    return Facets[FacetType];
}

bool Cube::Draw(float xTranslate, float yTranslate,
                     std::map< int16_t, vector< vertex >* >& normal,
                     std::map< int16_t, vector< vertex >* >& tops)
{
    if(data.geometry == GEOM_SLOPE)
    {
        return DrawSlope(xTranslate,yTranslate,normal,tops);
    }
    else if (data.geometry == GEOM_EMPTY)
    {
        return false;
    }
    else
    {
        return DrawFaces(xTranslate, yTranslate, normal, tops);
    }
}

//TODO: pre-generating all possible configs and using them as templates would be faster. deferred
bool Cube::DrawFaces(float xTranslate, float yTranslate,
                     std::map< int16_t, vector< vertex >* >& normal,
                     std::map< int16_t, vector< vertex >* >& tops)
{
    // cached quads
    static const vertex vertices[6][4] =
    {
        // position, uv texture coords, normal vector - see vertex in Renderer.h
        // FACET_TOP
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        // FACET_BOTTOM
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),

        // FACET_NORTH
        vertex( 0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f,-1.0f, 0.0f ),
        vertex(-0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f,-1.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  0.0f,-1.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  0.0f, 0.0f,  0.0f,-1.0f, 0.0f ),
        // FACET_SOUTH
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f ),

        // FACET_EAST
        vertex( 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f ),
        // FACET_WEST
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f )
    };

    // work vector ptr
    vector< vertex >* vec;
    vertex test = vertices[3][3];

    if(!Visible)
    {
        return false;
    }

    for(Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType)
    {
        if(!hasFace(FacetType))
        {
            continue; // skip faces we don't have here
        }
        Cube* c = getAdjacentCube(FacetType);

        // floors are always generated in the normal way, they face the opposite direction!
        if(FacetType != FACET_BOTTOM)
        {
            // blocked top facets are sent to the top vertex vector
            bool blocked =
            c &&
            (!c->isHidden() || c->isHidden() && Hidden) &&
            (c->hasFace(OppositeFacet(FacetType)) || c->getGeometry() == GEOM_WALL || c->getGeometry() == GEOM_SLOPE);

            if(blocked)
            {
                if(FacetType == FACET_TOP)
                {
                    if(!tops.count(Material))
                    {
                        vec = new vector< vertex >;
                        tops[Material] = vec;
                    }
                    else
                    {
                        vec = tops[Material];
                    }
                    vertex v3 = vertices[FacetType][3];
                    v3.translate(xTranslate, yTranslate);
                    vertex v2 = vertices[FacetType][2];
                    v2.translate(xTranslate, yTranslate);
                    vertex v1 = vertices[FacetType][1];
                    v1.translate(xTranslate, yTranslate);
                    vertex v0 = vertices[FacetType][0];
                    v0.translate(xTranslate, yTranslate);

                    vec->push_back(v3);
                    vec->push_back(v1);
                    vec->push_back(v0);

                    vec->push_back(v3);
                    vec->push_back(v2);
                    vec->push_back(v1);
                }
                continue; // skip blocked face
            }
        }
        // normal visible geometry
        vector< vertex >* vec;
        if(!normal.count(Material))
        {
            vec = new vector< vertex >;
            normal[Material] = vec;
        }
        else
        {
            vec = normal[Material];
        }
        vertex v3 = vertices[FacetType][3];
        v3.translate(xTranslate, yTranslate);
        vertex v2 = vertices[FacetType][2];
        v2.translate(xTranslate, yTranslate);
        vertex v1 = vertices[FacetType][1];
        v1.translate(xTranslate, yTranslate);
        vertex v0 = vertices[FacetType][0];
        v0.translate(xTranslate, yTranslate);

        vec->push_back(v3);
        vec->push_back(v1);
        vec->push_back(v0);

        vec->push_back(v3);
        vec->push_back(v2);
        vec->push_back(v1);
    }
}
// DRAW FACE
// tops_vis_invis =  draw visible or invisible tops? true = visible
//FIXME: waste of CPU cycles
/*
bool Cube::DrawFace(float xTranslate, float yTranslate, Facet FacetType, bool tops_vis_invis)
{
    //glColor4f(1,1,1,0.2);
    Vector3 Points[4];
    if(Visible && hasFace(FacetType))
    {
        if(FacetType != FACET_BOTTOM)
        {
            Cube * c = getAdjacentCube(FacetType);
            if(FacetType == FACET_TOP)
            {
                bool nodraw = c && (!c->isHidden() || c->isHidden() && Hidden) && (c->hasFace(OppositeFacet(FacetType)) || c->getGeometry() == GEOM_WALL);
                if(tops_vis_invis && nodraw || !tops_vis_invis && !nodraw)
                {
                    return false;
                }
            }
            else
            {
                if(c && (!c->isHidden() || c->isHidden() && Hidden) && c->hasFace(OppositeFacet(FacetType)))
                    return false;
            }
        }
        switch(FacetType)
        {
            case FACET_TOP:
            {
                glNormal3f( 0.0f, 0.0f, 1.0f);
                Points[0] = ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
                Points[1] = ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
                Points[2] = ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
                Points[3] = ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
                break;
            }
            case FACET_BOTTOM:
            {
                glNormal3f( 0.0f, 0.0f, 1.0f);
                Points[0] = ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
                Points[1] = ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
                Points[2] = ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
                Points[3] = ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
                break;
            }
            case FACET_NORTH:
            {
                glNormal3f( 0.0f, -1.0f, 0.0f);
                Points[0] = ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
                Points[1] = ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
                Points[2] = ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
                Points[3] = ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
                break;
            }
            case FACET_EAST:
            {
                glNormal3f( 1.0f, 0.0f, 0.0f);
                Points[0] = ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
                Points[1] = ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
                Points[2] = ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
                Points[3] = ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
                break;
            }
            case FACET_SOUTH:
            {
                glNormal3f( 0.0f, 1.0f, 0.0f);
                Points[0] = ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
                Points[1] = ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
                Points[2] = ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
                Points[3] = ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
                break;
            }
            case FACET_WEST:
            {
                glNormal3f( -1.0f, 0.0f, 0.0f);
                Points[0] = ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
                Points[1] = ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
                Points[2] = ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
                Points[3] = ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
                break;
            }
        }
        TEXTURE->BindTexture(Material);
        glTexCoord2f(0,0);
        glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);

        glTexCoord2f(1,1);
        glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);

        glTexCoord2f(0,1);
        glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);


        glTexCoord2f(0,0);
        glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);

        glTexCoord2f(1,0);
        glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

        glTexCoord2f(1,1);
        glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
        RENDERER->IncrementTriangles(2);
    }

    return true;
}

*/
// DRAW SLOPE

/// TODO: normal vectors. these are required for lighting
/// TODO: separate this from khazad, use for mesh generation
/// FIXME: waste of CPU cycles
bool Cube::DrawSlope(float xTranslate, float yTranslate,
                     std::map< int16_t, vector< vertex >* >& normal,
                     std::map< int16_t, vector< vertex >* >& tops)
{
    if(!Visible) return false;
    /**
    * heightmap. order is from nort-west to north-west, clockwise. hm[9] is the center
    * 0=8--1---2
    *  |   |   |
    *  7---9---3
    *  |   |   |
    *  6---5---4
    */
    uint8_t hm[10] = {0,0,0,0,0,0,0,0,0,1};
    float hmf[10];// same in float

    uint8_t covered[4] = {0,0,0,0}; // view of a side blocked by geometry?

    // coordinates of the directions... fixed for spillover at the end of the loop
    const float dc[9][2] =
    {
        {-0.5,-0.5},
        {0   ,-0.5},
        {0.5 ,-0.5},
        {0.5 ,0},
        {0.5 ,0.5},
        {0   ,0.5},
        {-0.5,0.5},
        {-0.5,0},
        {-0.5,-0.5}
    };
    // same for texture coords
    const float tc[9][2] =
    {
        {0,1},
        {0.5   ,1},
        {1 ,1},
        {1 ,0.5},
        {1 ,0},
        {0.5   ,0},
        {0,0},
        {0,0.5},
        {0,1}
    };
    const float norms[8][3] =
    {
        {0,-1,0},
        {0,0,0},
        {1,0,0},
        {0,0,0},
        {0,1,0},
        {0,0,0},
        {-1,0,0},
        {0,0,0}
    };
    uint8_t strong, weak, numsolids = 0;

    // copy surroundings
    for(Direction i = DIRECTION_NORTHWEST; i <= DIRECTION_WEST; ++i)
    {
        bool solid = getNeighborCube(i) != NULL && getNeighborCube(i)->isSolid();
        hm[i] = solid << 1;
        numsolids += solid;
    }

    // test for covered and uncovered sides
    covered[0] = hm[1] || getNeighborCube(DIRECTION_NORTH) != NULL && getNeighborCube(DIRECTION_NORTH)->isSlope();
    covered[1] = hm[3] || getNeighborCube(DIRECTION_EAST) != NULL && getNeighborCube(DIRECTION_EAST)->isSlope();
    covered[2] = hm[5] || getNeighborCube(DIRECTION_SOUTH) != NULL && getNeighborCube(DIRECTION_SOUTH)->isSlope();
    covered[3] = hm[7] || getNeighborCube(DIRECTION_WEST) != NULL && getNeighborCube(DIRECTION_WEST)->isSlope();

    // determine center
    strong = (hm[7] && hm[1] && !hm[3] && !hm[4] && !hm[5])
    + (hm[1] && hm[3] && !hm[5] && !hm[6] && !hm[7])
    + (hm[3] && hm[5] && !hm[7] && !hm[0] && !hm[1])
    + (hm[5] && hm[7] && !hm[1] && !hm[2] && !hm[3]);
    if(numsolids == 1)
    {
        if (hm[0] || hm[2] || hm[4] || hm[6] )
        {
            hm[9] = 0;
        }
    }
    else if(strong == 1) hm[9] = 2;
    else hm[9] = 1;

    // fix corners
    hm[0] = hm[7] | hm[0] | hm[1];
    hm[2] = hm[1] | hm[2] | hm[3];
    hm[4] = hm[3] | hm[4] | hm[5];
    hm[6] = hm[5] | hm[6] | hm[7];

    // fix sides so that slopes aren't jaggy.
    hm[1] = (hm[1] >> 1) + (hm[0] || hm[2]);
    hm[3] = (hm[3] >> 1) + (hm[2] || hm[4]);
    hm[5] = (hm[5] >> 1) + (hm[4] || hm[6]);
    hm[7] = (hm[7] >> 1) + (hm[6] || hm[0]);

    hm[8] = hm[0]; // copy first point so we can safely use algorithms that go to N+1

    // convert int heights to floats
    for(int i = 0; i < 10; ++i)
    {
        hmf[i] = ((float)hm[i]) / 2 - 0.5;
    }
    vector <vertex>* vec;
    if(!normal.count(Material))
    {
        vec = new vector< vertex >;
        normal[Material] = vec;
        vec->reserve(256);
    }
    else
    {
        vec = normal[Material];
    }

    // draw triangles
    for(Direction i = DIRECTION_NORTHWEST; i <= DIRECTION_WEST; ++i)
    {
        /**
        *  P--P+1--*
        *  | \ | / |
        *  *---C---*
        *  | / | \ |
        *  *---*---*
        */
        s3f center = {xTranslate, yTranslate,hmf[9]};
        s3f pt1 = {dc[i+1][0] + xTranslate,dc[i+1][1]+  yTranslate,hmf[i+1]};
        s3f pt2 = {dc[i][0] + xTranslate, dc[i][1]+  yTranslate,hmf[i]};
        s3f norm = CalculateNormal(center, pt2, pt1 );
        s2f tx0 = {0.5,0.5};
        s2f tx1 = {tc[i+1][0],tc[i+1][1]};
        s2f tx2 = {tc[i][0],tc[i][1]};
        /*
        // point C
        vertex v0 = vertex(xTranslate, yTranslate, hmf[9],0.5,0.5, 0,0,1);
        //point P+1
        vertex v1 = vertex(dc[i+1][0] + xTranslate, dc[i+1][1]+ yTranslate, hmf[i+1],tc[i+1][0],tc[i+1][1], 0,0,1);
        //point P
        vertex v2 = vertex(dc[i][0] + xTranslate, dc[i][1]+ yTranslate, hmf[i],tc[i][0],tc[i][1], 0,0,1);
        */
        vertex v0 = vertex(center,tx0, norm);
        //point P+1
        vertex v1 = vertex(pt1,tx1, norm);
        //point P
        vertex v2 = vertex(pt2,tx2, norm);
        if(v0.z == v1.z == v2.z == 0.5) // top
        {
            vector <vertex>* vect;
            Cube *c = getAdjacentCube(FACET_TOP);
            if(c && !c->getGeometry() == GEOM_EMPTY)
            {
                if(!tops.count(Material))
                {
                    vect = new vector< vertex >;
                    tops[Material] = vect;
                    vect->reserve(256);
                }
                else
                {
                    vect = tops[Material];
                }
                vect->push_back(v0);
                vect->push_back(v1);
                vect->push_back(v2);
                // next triangle
                continue;
            }
        }
        vec->push_back(v0);
        vec->push_back(v1);
        vec->push_back(v2);
    }
    //patch holes
    for(int i = 0; i< 8;i+=2)
    {
        // already covered by wall or nearby slope, don't draw side
        if (covered[i/2])
            continue;

        // zero center can be ignored
        if (hm[i+1] == 0)
            continue;

        // determine how many triangles are needed an in what configuration
        if(hm[i] == 0)// one tri, hm[i+2] is high
        {
            // second upper
            vec->push_back(vertex(dc[i+2][0] + xTranslate, dc[i+2][1] + yTranslate, hmf[i+2], 0.0,1.0, norms[i][0],norms[i][1],norms[i][2]));
            // second lower
            vec->push_back(vertex(dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, -0.5, 0.0,0.0, norms[i][0],norms[i][1],norms[i][2]));
            // first lower
            vec->push_back(vertex(dc[i][0] + xTranslate, dc[i][1]+ yTranslate, hmf[i], 1.0,0.0,norms[i][0],norms[i][1],norms[i][2]));
        }
        else if(hm[i+2] == 0)// one tri, hm[i] is high
        {
            // first lower
            vec->push_back(vertex(dc[i][0] + xTranslate,  dc[i][1] + yTranslate, -0.5,1.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // first upper
            vec->push_back(vertex(dc[i][0] + xTranslate,  dc[i][1] + yTranslate, hmf[i],   1.0,1.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // second
            vec->push_back(vertex(dc[i+2][0] + xTranslate, dc[i+2][1]+ yTranslate, hmf[i+2],   0.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
        }
        else // two tris, both corners high
        {
            // second upper
            vec->push_back(vertex(dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, 0.5,    0.0,1.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // second lower
            vec->push_back(vertex(dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, -0.5,    0.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // first lower
            vec->push_back(vertex(dc[i][0] + xTranslate, dc[i][1]+ yTranslate, -0.5,    1.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));

            // first lower
            vec->push_back(vertex(dc[i][0] + xTranslate, dc[i][1]+ yTranslate, -0.5,    1.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // first upper
            vec->push_back(vertex(dc[i][0] + xTranslate,  dc[i][1] + yTranslate, 0.5,    1.0,1.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // center
            vec->push_back(vertex(dc[i+1][0] + xTranslate, dc[i+1][1]+ yTranslate, 0.0,    0.5,0.5,   norms[i][0],norms[i][1],norms[i][2] ));
        }
    }
    return true;
}
