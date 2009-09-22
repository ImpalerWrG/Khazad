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
//    if(Initalized)
//    {
        MAP->ChangeInitedSlopeCount(-1);
//    }
}

bool Slope::Init(Cube* NewOwner, Slopping Type)
{
    Owner = NewOwner;
    SetSlopeType(Type);

    Material = Owner->getMaterial();
    Texture = Material;

    MAP->ChangeInitedSlopeCount(1);
    return true;
}

void Slope::SetSlopeType(Slopping Type)
{
    SlopeType = Type;
    Visible = true;
}

bool Slope::Update()
{
    return true;
}



/// TODO: normal vectors. these are required for lighting
/// TODO: separate this from khazad, use for mesh generation
/// FIXME: waste of CPU cycles
bool Slope::Draw(float xTranslate, float yTranslate)
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
    uint8_t strong, weak, numsolids = 0;

    // copy surroundings
    for(Direction i = DIRECTION_NORTHWEST; i <= DIRECTION_WEST; ++i)
    {
        bool solid = Owner->getNeiborCube(i) != NULL && Owner->getNeiborCube(i)->isSolid();
        hm[i] = solid << 1;
        numsolids += solid;
    }

    // test for covered and uncovered sides
    covered[0] = hm[1] || Owner->getNeiborCube(DIRECTION_NORTH) != NULL && Owner->getNeiborCube(DIRECTION_NORTH)->getSlope();
    covered[1] = hm[3] || Owner->getNeiborCube(DIRECTION_EAST) != NULL && Owner->getNeiborCube(DIRECTION_EAST)->getSlope();
    covered[2] = hm[5] || Owner->getNeiborCube(DIRECTION_SOUTH) != NULL && Owner->getNeiborCube(DIRECTION_SOUTH)->getSlope();
    covered[3] = hm[7] || Owner->getNeiborCube(DIRECTION_WEST) != NULL && Owner->getNeiborCube(DIRECTION_WEST)->getSlope();

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
        // point C
        TEXTURE->BindTexturePoint(Texture, 0.5,0.5);
        glVertex3f( xTranslate,  yTranslate, hmf[9]);
        //point P+1
        TEXTURE->BindTexturePoint(Texture, tc[i+1][0],tc[i+1][1]);
        glVertex3f(dc[i+1][0] + xTranslate, dc[i+1][1]+ yTranslate, hmf[i+1]);
        //point P
        TEXTURE->BindTexturePoint(Texture, tc[i][0],tc[i][1]);
        glVertex3f(dc[i][0] + xTranslate, dc[i][1]+ yTranslate, hmf[i]);
    }
    // just drawn 8 tris
    SCREEN->IncrementTriangles(8);

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
            TEXTURE->BindTexturePoint(Texture, 0.0,1.0);
            glVertex3f( dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, hmf[i+2]);
            // second lower
            TEXTURE->BindTexturePoint(Texture, 0.0,0.0);
            glVertex3f( dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, -0.5);
            // first lower
            TEXTURE->BindTexturePoint(Texture, 1.0,0.0);
            glVertex3f( dc[i][0] + xTranslate, dc[i][1]+ yTranslate, hmf[i]);
            SCREEN->IncrementTriangles(1);
        }
        else if(hm[i+2] == 0)// one tri, hm[i] is high
        {
            // first lower
            TEXTURE->BindTexturePoint(Texture, 1.0,0.0);
            glVertex3f( dc[i][0] + xTranslate,  dc[i][1] + yTranslate, -0.5);
            // first upper
            TEXTURE->BindTexturePoint(Texture, 1.0,1.0);
            glVertex3f( dc[i][0] + xTranslate,  dc[i][1] + yTranslate, hmf[i]);
            // second
            TEXTURE->BindTexturePoint(Texture, 0.0,0.0);
            glVertex3f( dc[i+2][0] + xTranslate, dc[i+2][1]+ yTranslate, hmf[i+2]);
            SCREEN->IncrementTriangles(1);
        }
        else // two tris, both corners high
        {
            // second upper
            TEXTURE->BindTexturePoint(Texture, 0.0,1.0);
            glVertex3f( dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, 0.5);
            // second lower
            TEXTURE->BindTexturePoint(Texture, 0.0,0.0);
            glVertex3f( dc[i+2][0] + xTranslate,  dc[i+2][1] + yTranslate, -0.5);
            // first lower
            TEXTURE->BindTexturePoint(Texture, 1.0,0.0);
            glVertex3f( dc[i][0] + xTranslate, dc[i][1]+ yTranslate, -0.5);
            
            // first lower
            TEXTURE->BindTexturePoint(Texture, 1.0,0.0);
            glVertex3f( dc[i][0] + xTranslate, dc[i][1]+ yTranslate, -0.5);
            // first upper
            TEXTURE->BindTexturePoint(Texture, 1.0,1.0);
            glVertex3f( dc[i][0] + xTranslate,  dc[i][1] + yTranslate, 0.5);
            // center
            TEXTURE->BindTexturePoint(Texture, 0.5,0.5);
            glVertex3f( dc[i+1][0] + xTranslate, dc[i+1][1]+ yTranslate, 0.0);
            SCREEN->IncrementTriangles(2);
        }
    }
    return true;
}
