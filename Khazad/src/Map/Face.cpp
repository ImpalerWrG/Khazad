#include <stdafx.h>

#include <Cell.h>
#include <Cube.h>
#include <Face.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>
#include <DataManager.h>
#include <Random.h>


Face::Face()
{
	setType(FACE_ACTOR);

	Initalized = false;
	Visible = false;

    Constructed = false;
    Decorated = false;

    Material = 6;

    Rotation = RANDOM->Roll(0, 255);

	//GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Face::~Face()
{
    //GAME->RemoveActor(ID);
}

bool Face::Init(Facet Type, Uint16 MaterialType)
{
    Initalized = true;

	FacetType = Type;
	Visible = true;
	Material = MaterialType;
	Texture = DATA->getMaterialData(Material)->getTexture();

/*
	switch(FacetType)
	{
		case FACET_TOP:
		{
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = + HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = + HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = + HALFCUBE;

			break;
		}
		case FACET_BOTTOM:
		{
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = - HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = - HALFCUBE;

			break;
		}
		case FACET_NORTH_EAST:
		{
			Points[0].x = - HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = + HALFCUBE;

			break;
		}
		case FACET_SOUTH_EAST:
		{
			Points[0].x = + HALFCUBE;
			Points[0].y = - HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = + HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = + HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = - HALFCUBE;
			Points[3].z = + HALFCUBE;

			break;
		}
		case FACET_SOUTH_WEST:
		{
			Points[0].x = + HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = - HALFCUBE;
			Points[1].y = + HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = + HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = + HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = + HALFCUBE;

			break;
		}
		case FACET_NORTH_WEST:
		{
			Points[0].x = - HALFCUBE;
			Points[0].y = + HALFCUBE;
			Points[0].z = - HALFCUBE;

			Points[1].x = - HALFCUBE;
			Points[1].y = - HALFCUBE;
			Points[1].z = - HALFCUBE;

			Points[2].x = - HALFCUBE;
			Points[2].y = - HALFCUBE;
			Points[2].z = + HALFCUBE;

			Points[3].x = - HALFCUBE;
			Points[3].y = + HALFCUBE;
			Points[3].z = + HALFCUBE;

			break;
		}
	}
*/

	return true;
}

bool Face::Update()
{
	return true;
}

bool Face::Draw(float xTranslate, float yTranslate)
{
    Vector3 Points[4];

    if(Visible)
    {
        switch(FacetType)
        {
            case FACET_TOP:
            {
                Points[0].x = - HALFCUBE;
                Points[0].y = + HALFCUBE;
                Points[0].z = + HALFCUBE;

                Points[1].x = + HALFCUBE;
                Points[1].y = + HALFCUBE;
                Points[1].z = + HALFCUBE;

                Points[2].x = + HALFCUBE;
                Points[2].y = - HALFCUBE;
                Points[2].z = + HALFCUBE;

                Points[3].x = - HALFCUBE;
                Points[3].y = - HALFCUBE;
                Points[3].z = + HALFCUBE;

                break;
            }
            case FACET_BOTTOM:
            {
                Points[0].x = - HALFCUBE;
                Points[0].y = + HALFCUBE;
                Points[0].z = - HALFCUBE;

                Points[1].x = + HALFCUBE;
                Points[1].y = + HALFCUBE;
                Points[1].z = - HALFCUBE;

                Points[2].x = + HALFCUBE;
                Points[2].y = - HALFCUBE;
                Points[2].z = - HALFCUBE;

                Points[3].x = - HALFCUBE;
                Points[3].y = - HALFCUBE;
                Points[3].z = - HALFCUBE;

                break;
            }
            case FACET_NORTH_EAST:
            {
                Points[0].x = - HALFCUBE;
                Points[0].y = - HALFCUBE;
                Points[0].z = - HALFCUBE;

                Points[1].x = + HALFCUBE;
                Points[1].y = - HALFCUBE;
                Points[1].z = - HALFCUBE;

                Points[2].x = + HALFCUBE;
                Points[2].y = - HALFCUBE;
                Points[2].z = + HALFCUBE;

                Points[3].x = - HALFCUBE;
                Points[3].y = - HALFCUBE;
                Points[3].z = + HALFCUBE;

                break;
            }
            case FACET_SOUTH_EAST:
            {
                Points[0].x = + HALFCUBE;
                Points[0].y = - HALFCUBE;
                Points[0].z = - HALFCUBE;

                Points[1].x = + HALFCUBE;
                Points[1].y = + HALFCUBE;
                Points[1].z = - HALFCUBE;

                Points[2].x = + HALFCUBE;
                Points[2].y = + HALFCUBE;
                Points[2].z = + HALFCUBE;

                Points[3].x = + HALFCUBE;
                Points[3].y = - HALFCUBE;
                Points[3].z = + HALFCUBE;

                break;
            }
            case FACET_SOUTH_WEST:
            {
                Points[0].x = + HALFCUBE;
                Points[0].y = + HALFCUBE;
                Points[0].z = - HALFCUBE;

                Points[1].x = - HALFCUBE;
                Points[1].y = + HALFCUBE;
                Points[1].z = - HALFCUBE;

                Points[2].x = - HALFCUBE;
                Points[2].y = + HALFCUBE;
                Points[2].z = + HALFCUBE;

                Points[3].x = + HALFCUBE;
                Points[3].y = + HALFCUBE;
                Points[3].z = + HALFCUBE;

                break;
            }
            case FACET_NORTH_WEST:
            {
                Points[0].x = - HALFCUBE;
                Points[0].y = + HALFCUBE;
                Points[0].z = - HALFCUBE;

                Points[1].x = - HALFCUBE;
                Points[1].y = - HALFCUBE;
                Points[1].z = - HALFCUBE;

                Points[2].x = - HALFCUBE;
                Points[2].y = - HALFCUBE;
                Points[2].z = + HALFCUBE;

                Points[3].x = - HALFCUBE;
                Points[3].y = + HALFCUBE;
                Points[3].z = + HALFCUBE;

                break;
            }
        }

        TEXTURE->BindTexturePoint(Texture, 0 + Rotation);         glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);
        TEXTURE->BindTexturePoint(Texture, 1 + Rotation);         glVertex3f(Points[1].x + xTranslate, Points[1].y + yTranslate, Points[1].z);
        TEXTURE->BindTexturePoint(Texture, 2 + Rotation);         glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);

        TEXTURE->BindTexturePoint(Texture, 2 + Rotation);         glVertex3f(Points[2].x + xTranslate, Points[2].y + yTranslate, Points[2].z);
        TEXTURE->BindTexturePoint(Texture, 3 + Rotation);         glVertex3f(Points[3].x + xTranslate, Points[3].y + yTranslate, Points[3].z);
        TEXTURE->BindTexturePoint(Texture, 0 + Rotation);         glVertex3f(Points[0].x + xTranslate, Points[0].y + yTranslate, Points[0].z);

        SCREEN->IncrementTriangles(2);
    }

	return true;
}
