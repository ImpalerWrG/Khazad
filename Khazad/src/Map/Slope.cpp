#include <stdafx.h>

#include <Slope.h>
#include <Cube.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>

Slope::Slope()
{
	setType(SLOPE_ACTOR);

	Owner = NULL;

	GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Slope::~Slope()
{
    //GAME->RemoveActor(ID);
}

bool Slope::Init(Cube* NewOwner, Slopping Type)
{
    Owner = NewOwner;
    SetSlopeType(Type);
}

void Slope::SetSlopeType(Slopping Type)
{
    SlopeType = Type;
    Visible = true;

    switch(SlopeType)
    {
        case SLOPE_NORTH_EAST:
        {
            Points[0].x = + HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = - HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
        case SLOPE_SOUTH_EAST:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = - HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
        case SLOPE_SOUTH_WEST:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
        case SLOPE_NORTH_WEST:
        {
			Points[0].x = + HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
        case SLOPE_LARGE_NORTH:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = + HALFCUBE;
            break;
        }
        case SLOPE_LARGE_EAST:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = + HALFCUBE;
            break;
        }
        case SLOPE_LARGE_SOUTH:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = + HALFCUBE;
            break;
        }
        case SLOPE_LARGE_WEST:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = + HALFCUBE;
            break;
        }
        case SLOPE_SMALL_NORTH:
        {
			Points[0].x = + HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
        case SLOPE_SMALL_EAST:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = - HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
        case SLOPE_SMALL_SOUTH:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = - HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
       case SLOPE_SMALL_WEST:
        {
			Points[0].x = + HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = - HALFCUBE;
            break;
        }
       case SLOPE_CRESS_NORTH_SOUTH:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = + HALFCUBE;
            break;
        }
       case SLOPE_CRESS_EAST_WEST:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = + HALFCUBE;
            break;
        }

       case SLOPE_FLAT:
        {
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = 0;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = 0;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = 0;

			Points[3].x = + HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = 0;
            break;
        }



        default:
        {
            break;
        }
    }
}

bool Slope::Update()
{
    return true;
}

bool Slope::Draw(float xTranslate, float yTranslate)
{
    if(Visible)
    {
        Uint16 Material = Owner->getMaterial();

        if (SlopeType == SLOPE_NORTH_EAST || SlopeType == SLOPE_SOUTH_EAST || SlopeType == SLOPE_SOUTH_WEST || SlopeType == SLOPE_NORTH_WEST )
        {
                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);   glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);
                TEXTURE->BindTexturePoint(Material, 3);   glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
        }

        if (SlopeType == SLOPE_SMALL_NORTH || SlopeType == SLOPE_SMALL_EAST || SlopeType == SLOPE_SMALL_SOUTH || SlopeType == SLOPE_SMALL_WEST )
        {
                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);   glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

                TEXTURE->BindTexturePoint(Material, 1);   glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);
                TEXTURE->BindTexturePoint(Material, 3);   glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
        }

        if (SlopeType == SLOPE_LARGE_NORTH || SlopeType == SLOPE_LARGE_EAST || SlopeType == SLOPE_LARGE_SOUTH || SlopeType == SLOPE_LARGE_WEST )
        {
                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);   glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);   glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 3);   glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
        }

        if (SlopeType == SLOPE_CRESS_NORTH_SOUTH || SlopeType == SLOPE_CRESS_EAST_WEST )
        {
                TEXTURE->BindTexturePoint(Material, 0);    glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);    glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 2);    glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

                TEXTURE->BindTexturePoint(Material, 0);    glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);    glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 3);    glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
        }

        if(SlopeType == SLOPE_FLAT)
        {
                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
                TEXTURE->BindTexturePoint(Material, 1);   glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

                TEXTURE->BindTexturePoint(Material, 2);   glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);
                TEXTURE->BindTexturePoint(Material, 3);   glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
                TEXTURE->BindTexturePoint(Material, 0);   glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
        }

        SCREEN->IncrementTriangles(2);
    }

    return true;
}
