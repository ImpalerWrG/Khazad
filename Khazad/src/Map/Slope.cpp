#include <stdafx.h>

#include <Slope.h>
#include <Cube.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>
#include <DataManager.h>
#include <Map.h>

Slope::Slope()
{
//	setType(SLOPE_ACTOR);

	Owner = NULL;

    Material = 6;
	Texture = 0;

//	GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
    MAP->ChangeSlopeCount(1);
}

Slope::~Slope()
{
    MAP->ChangeSlopeCount(-1);
    //GAME->RemoveActor(ID);
    if(Initalized)
    {
        MAP->ChangeInitedSlopeCount(-1);
    }
}

bool Slope::Init(Cube* NewOwner, Slopping Type)
{
    Owner = NewOwner;
    SetSlopeType(Type);

    Material = Owner->getMaterial();
	Texture = Material; //DATA->getMaterialData(Material)->getTexture();

    MAP->ChangeInitedSlopeCount(1);
    return true;
}

void Slope::SetSlopeType(Slopping Type)
{
    SlopeType = Type;
    Visible = true;
/*
    switch(SlopeType)
    {
        case SLOPE_NORTH_EAST:
        {
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            break;
        }
        case SLOPE_SOUTH_EAST:
        {
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            break;
        }
        case SLOPE_SOUTH_WEST:
        {
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            break;
        }
        case SLOPE_NORTH_WEST:
        {
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            break;
        }
        case SLOPE_LARGE_NORTH:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
            break;
        }
        case SLOPE_LARGE_EAST:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            break;
        }
        case SLOPE_LARGE_SOUTH:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
            break;
        }
        case SLOPE_LARGE_WEST:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            break;
        }
        case SLOPE_SMALL_NORTH:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            break;
        }
        case SLOPE_SMALL_EAST:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            break;
        }
        case SLOPE_SMALL_SOUTH:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            break;
        }
       case SLOPE_SMALL_WEST:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            break;
        }
       case SLOPE_CRESS_NORTH_SOUTH:
        {

            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            break;
        }
       case SLOPE_CRESS_EAST_WEST:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_TOP);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_TOP);
            break;
        }

       case SLOPE_FLAT:
        {
            Points[0] = Owner->ConvertSpacialPoint(SPACIAL_POINT_SOUTH_BOTTOM);
            Points[1] = Owner->ConvertSpacialPoint(SPACIAL_POINT_EAST_BOTTOM);
            Points[2] = Owner->ConvertSpacialPoint(SPACIAL_POINT_WEST_BOTTOM);
            Points[3] = Owner->ConvertSpacialPoint(SPACIAL_POINT_NORTH_BOTTOM);
            break;
        }


        default:
        {
            break;
        }
    }*/
}

bool Slope::Update()
{
    return true;
}

/// TODO: terrible... just terrible. needs better algo and refactor of the surrounding classes. but it works. best to move it out of the way somewhere and make it create an instantiated VBO
/// TODO: normal vectors. these are required for lighting
/// TODO: document, scan and clean up notes
bool Slope::Draw(float xTranslate, float yTranslate)
{
    if(Visible)
    {
        // how many strong corners?
        int numStrong = 0;
        bool NorthEastSolid = Owner->getNeiborCube(DIRECTION_NORTHEAST) != NULL && Owner->getNeiborCube(DIRECTION_NORTHEAST)->isSolid();
        bool SouthEastSolid = Owner->getNeiborCube(DIRECTION_SOUTHEAST) != NULL && Owner->getNeiborCube(DIRECTION_SOUTHEAST)->isSolid();
        bool NorthWestSolid = Owner->getNeiborCube(DIRECTION_NORTHWEST) != NULL && Owner->getNeiborCube(DIRECTION_NORTHWEST)->isSolid();
        bool SouthWestSolid = Owner->getNeiborCube(DIRECTION_SOUTHWEST) != NULL && Owner->getNeiborCube(DIRECTION_SOUTHWEST)->isSolid();
        bool NorthSolid = Owner->getNeiborCube(DIRECTION_NORTH) != NULL && Owner->getNeiborCube(DIRECTION_NORTH)->isSolid();
        bool SouthSolid = Owner->getNeiborCube(DIRECTION_SOUTH) != NULL && Owner->getNeiborCube(DIRECTION_SOUTH)->isSolid();
        bool WestSolid = Owner->getNeiborCube(DIRECTION_WEST) != NULL && Owner->getNeiborCube(DIRECTION_WEST)->isSolid();
        bool EastSolid = Owner->getNeiborCube(DIRECTION_EAST) != NULL && Owner->getNeiborCube(DIRECTION_EAST)->isSolid();
        // number of solids - only if slope has one adjacent solid tile it has a weak center point
        int numSolids = NorthEastSolid + SouthEastSolid + NorthWestSolid + SouthWestSolid + NorthSolid + SouthSolid + WestSolid + EastSolid;

        bool NorthEastSlope = Owner->getNeiborCube(DIRECTION_NORTHEAST) != NULL && Owner->getNeiborCube(DIRECTION_NORTHEAST)->getSlope();
        bool SouthEastSlope = Owner->getNeiborCube(DIRECTION_SOUTHEAST) != NULL && Owner->getNeiborCube(DIRECTION_SOUTHEAST)->getSlope();
        bool NorthWestSlope = Owner->getNeiborCube(DIRECTION_NORTHWEST) != NULL && Owner->getNeiborCube(DIRECTION_NORTHWEST)->getSlope();
        bool SouthWestSlope = Owner->getNeiborCube(DIRECTION_SOUTHWEST) != NULL && Owner->getNeiborCube(DIRECTION_SOUTHWEST)->getSlope();
        bool NorthSlope = Owner->getNeiborCube(DIRECTION_NORTH) != NULL && Owner->getNeiborCube(DIRECTION_NORTH)->getSlope();
        bool SouthSlope = Owner->getNeiborCube(DIRECTION_SOUTH) != NULL && Owner->getNeiborCube(DIRECTION_SOUTH)->getSlope();
        bool WestSlope = Owner->getNeiborCube(DIRECTION_WEST) != NULL && Owner->getNeiborCube(DIRECTION_WEST)->getSlope();
        bool EastSlope = Owner->getNeiborCube(DIRECTION_EAST) != NULL && Owner->getNeiborCube(DIRECTION_EAST)->getSlope();

        float heights[9];
        heights[0] = -0.5; // top left
        heights[1] = -0.5; // top
        heights[2] = -0.5; // top right
        heights[3] = -0.5; // right
        heights[4] = -0.5; // bottom right
        heights[5] = -0.5; // bottom
        heights[6] = -0.5; // bottom left
        heights[7] = -0.5; // left

        // corner points
        if(NorthWestSolid || NorthSolid || WestSolid) heights[0] = 0.5;
        else if(NorthWestSlope && NorthSlope && WestSlope) heights[0] = 0;
        if(NorthEastSolid || NorthSolid || EastSolid) heights[2] = 0.5;
        else if(NorthEastSlope && NorthSlope && EastSlope) heights[2] = 0;
        if(SouthEastSolid || SouthSolid || EastSolid) heights[4] = 0.5;
        else if(SouthEastSlope && SouthSlope && EastSlope) heights[4] = 0;
        if(SouthWestSolid || SouthSolid || WestSolid) heights[6] = 0.5;
        else if(SouthWestSlope && SouthSlope && WestSlope) heights[6] = 0;

        // side points
        if(NorthSolid) heights[1] = 0.5;
        else if(NorthSlope) heights[1] = 0;
        if(EastSolid) heights[3] = 0.5;
        else if(EastSlope) heights[3] = 0;
        if(SouthSolid) heights[5] = 0.5;
        else if(SouthSlope) heights[5] = 0;
        if(WestSolid) heights[7] = 0.5;
        else if(WestSlope) heights[7] = 0;

        // strong point check
        if(NorthSolid && WestSolid && !SouthEastSlope && !SouthEastSolid) numStrong ++;
        if(NorthSolid && EastSolid && !SouthWestSlope && !SouthWestSolid) numStrong ++;
        if(SouthSolid && WestSolid && !NorthEastSlope && !NorthEastSolid) numStrong ++;
        if(SouthSolid && EastSolid && !NorthWestSlope && !NorthWestSolid) numStrong ++;
/*
        if(NorthSolid && WestSolid && !SouthEastSolid) numStrong ++;
        if(NorthSolid && EastSolid && !SouthWestSolid) numStrong ++;
        if(SouthSolid && WestSolid && !NorthEastSolid) numStrong ++;
        if(SouthSolid && EastSolid && !NorthWestSolid) numStrong ++;
*/

        // weak/strong point processing
        if(numSolids == 1)// maybe weak point
        {
            // valid weak point?
           if(  (NorthWestSolid && !SouthSlope && !SouthEastSlope && !EastSlope)
              ||(NorthEastSolid && !SouthSlope && !SouthWestSlope && !WestSlope)
              ||(SouthEastSolid && !NorthSlope && !NorthWestSlope && !WestSlope)
              ||(SouthWestSolid && !NorthSlope && !NorthEastSlope && !EastSlope)
             )
           {
               heights[8] =  -0.5;
           }
           else
           {
               heights[8] =  0.0;// just slope
           }
        }
        else if(numStrong == 1)// valid strong point?
        {
            heights[8] =  0.5;
        }
        else heights[8] =  0.0;// just slope

        // assign high center only to slopes with one strong corner



        glEnd(); // end GL_TRIANGLES
        glBegin(GL_TRIANGLE_FAN);     	// draw a fan of triangles
        TEXTURE->BindTexturePoint(Texture, 0.5,0.5);
        glVertex3f( xTranslate,  yTranslate, heights[8]);

        TEXTURE->BindTexturePoint(Texture, 0,0);
        glVertex3f(-0.5f + xTranslate, -0.5f+ yTranslate, heights[0]);
        TEXTURE->BindTexturePoint(Texture, 0,0.5);
        glVertex3f(-0.5f + xTranslate,  0.0f+ yTranslate, heights[7]);
        TEXTURE->BindTexturePoint(Texture, 0,1);
        glVertex3f(-0.5f + xTranslate,  0.5f+ yTranslate, heights[6]);
        TEXTURE->BindTexturePoint(Texture, 0.5,1);
        glVertex3f( 0.0f + xTranslate,  0.5f+ yTranslate, heights[5]);
        TEXTURE->BindTexturePoint(Texture, 1,1);
        glVertex3f( 0.5f + xTranslate,  0.5f+ yTranslate, heights[4]);
        TEXTURE->BindTexturePoint(Texture, 1,0.5);
        glVertex3f( 0.5f + xTranslate,  0.0f+ yTranslate, heights[3]);
        TEXTURE->BindTexturePoint(Texture, 1,0);
        glVertex3f( 0.5f + xTranslate, -0.5f+ yTranslate, heights[2]);
        TEXTURE->BindTexturePoint(Texture, 0.5,0);
        glVertex3f( 0.0f + xTranslate, -0.5f+ yTranslate, heights[1]);
        TEXTURE->BindTexturePoint(Texture, 0,0);
        glVertex3f(-0.5f + xTranslate, -0.5f+ yTranslate, heights[0]);

        glEnd();
        glBegin(GL_TRIANGLES);

        // fill in empty spaces if any. another ugly hack :D
        ///TODO: use matrix manipulation to reduce code multiplication
        if(!NorthSolid && !NorthSlope)
        {
            if(heights[0] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 0,0);
                glVertex3f(-0.5f + xTranslate, -0.5f+ yTranslate, -0.5);
                TEXTURE->BindTexturePoint(Texture, 0,heights[0] + 0.5);
                glVertex3f(-0.5f + xTranslate, -0.5f+ yTranslate, heights[0]);
                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(xTranslate, -0.5f+ yTranslate, -0.5);
            }
            if(heights[2] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(xTranslate, -0.5f+ yTranslate, -0.5);
                TEXTURE->BindTexturePoint(Texture, 1,heights[2] + 0.5);
                glVertex3f(0.5f + xTranslate, -0.5f+ yTranslate, heights[2]);
                TEXTURE->BindTexturePoint(Texture, 1,0);
                glVertex3f(0.5f + xTranslate, -0.5f+ yTranslate, -0.5);

            }
        }
        if(!SouthSolid && !SouthSlope)
        {
            if(heights[4] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 1,0);
                glVertex3f(0.5f + xTranslate, 0.5f+ yTranslate, -0.5);
                TEXTURE->BindTexturePoint(Texture, 1,heights[4] + 0.5);
                glVertex3f(0.5f + xTranslate, 0.5f+ yTranslate, heights[4]);
                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(xTranslate, 0.5f+ yTranslate, -0.5);


            }
            if(heights[6] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(xTranslate, 0.5f+ yTranslate, -0.5);

                TEXTURE->BindTexturePoint(Texture, 0,heights[6] + 0.5);
                glVertex3f(-0.5f + xTranslate, 0.5f+ yTranslate, heights[6]);

                TEXTURE->BindTexturePoint(Texture, 0,0);
                glVertex3f(-0.5f + xTranslate, 0.5f+ yTranslate, -0.5);

            }
        }
        if(!EastSolid && !EastSlope)
        {
            if(heights[4] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 0,0);
                glVertex3f(0.5f + xTranslate, 0.5f+ yTranslate, -0.5);

                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(0.5f + xTranslate, yTranslate, -0.5);

                TEXTURE->BindTexturePoint(Texture, 0,heights[4] + 0.5);
                glVertex3f(0.5f + xTranslate, 0.5f+ yTranslate, heights[4]);
            }
            if(heights[2] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 1,0);
                glVertex3f(0.5f + xTranslate, -0.5f+ yTranslate, -0.5);

                TEXTURE->BindTexturePoint(Texture, 1,heights[2] + 0.5);
                glVertex3f(0.5f + xTranslate, -0.5f+ yTranslate, heights[2]);

                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(0.5f + xTranslate, yTranslate, -0.5);
            }
        }
        if(!WestSolid && !WestSlope)
        {
            if(heights[0] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 0,0);
                glVertex3f(-0.5f + xTranslate, -0.5f+ yTranslate, -0.5);
                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(-0.5f + xTranslate, yTranslate, -0.5);
                TEXTURE->BindTexturePoint(Texture, 0,heights[0] + 0.5);
                glVertex3f(-0.5f + xTranslate, -0.5f+ yTranslate, heights[0]);
            }
            if(heights[6] > 0.0)
            {
                TEXTURE->BindTexturePoint(Texture, 1,0);
                glVertex3f(-0.5f + xTranslate, 0.5f+ yTranslate, -0.5);

                TEXTURE->BindTexturePoint(Texture, 1,heights[6] + 0.5);
                glVertex3f(-0.5f + xTranslate, 0.5f+ yTranslate, heights[6]);

                TEXTURE->BindTexturePoint(Texture, 0.5,0);
                glVertex3f(-0.5f + xTranslate, yTranslate, -0.5);
            }
        }
    }
    SCREEN->IncrementTriangles(8);
    return true;
}
