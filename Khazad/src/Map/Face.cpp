#include <stdafx.h>

#include <Cube.h>
#include <Face.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>
#include <DataManager.h>


Face::Face()
{
	setType(FACE_ACTOR);

	Initalized = false;
	Visible = false;

	FirstOwner = NULL;
	SecondOwner = NULL;

    Constructed = false;
    Decorated = false;

    Material = 6;
	//GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Face::~Face()
{
    //GAME->RemoveActor(ID);
}

bool Face::Init(Cube* First, Cube* Second, Facet Type, Uint16 MaterialType)
{
    Initalized = true;

	FirstOwner = First;
    SecondOwner = Second;
	FacetType = Type;
	Visible = true;
	Material = MaterialType;

    if(SecondOwner != NULL)
    {
        SecondOwner->setFacet(Cube::OpositeFace(Type), this);
    }

	switch(FacetType)
	{
		case FACET_TOP:
		{
			Points[0].x = FirstOwner->Position.x - 0.5;
			Points[0].y = FirstOwner->Position.y + 0.5;
			Points[0].z = FirstOwner->Position.z + 0.5;

			Points[1].x = FirstOwner->Position.x + 0.5;
			Points[1].y = FirstOwner->Position.y + 0.5;
			Points[1].z = FirstOwner->Position.z + 0.5;

			Points[2].x = FirstOwner->Position.x + 0.5;
			Points[2].y = FirstOwner->Position.y - 0.5;
			Points[2].z = FirstOwner->Position.z + 0.5;

			Points[3].x = FirstOwner->Position.x - 0.5;
			Points[3].y = FirstOwner->Position.y - 0.5;
			Points[3].z = FirstOwner->Position.z + 0.5;

			break;
		}
		case FACET_BOTTOM:
		{
			Points[0].x = FirstOwner->Position.x - 0.5;
			Points[0].y = FirstOwner->Position.y + 0.5;
			Points[0].z = FirstOwner->Position.z - 0.5;

			Points[1].x = FirstOwner->Position.x + 0.5;
			Points[1].y = FirstOwner->Position.y + 0.5;
			Points[1].z = FirstOwner->Position.z - 0.5;

			Points[2].x = FirstOwner->Position.x + 0.5;
			Points[2].y = FirstOwner->Position.y - 0.5;
			Points[2].z = FirstOwner->Position.z - 0.5;

			Points[3].x = FirstOwner->Position.x - 0.5;
			Points[3].y = FirstOwner->Position.y - 0.5;
			Points[3].z = FirstOwner->Position.z - 0.5;

			break;
		}
		case FACET_NORTH_EAST:
		{
			Points[0].x = FirstOwner->Position.x - 0.5;
			Points[0].y = FirstOwner->Position.y - 0.5;
			Points[0].z = FirstOwner->Position.z - 0.5;

			Points[1].x = FirstOwner->Position.x + 0.5;
			Points[1].y = FirstOwner->Position.y - 0.5;
			Points[1].z = FirstOwner->Position.z - 0.5;

			Points[2].x = FirstOwner->Position.x + 0.5;
			Points[2].y = FirstOwner->Position.y - 0.5;
			Points[2].z = FirstOwner->Position.z + 0.5;

			Points[3].x = FirstOwner->Position.x - 0.5;
			Points[3].y = FirstOwner->Position.y - 0.5;
			Points[3].z = FirstOwner->Position.z + 0.5;

			break;
		}
		case FACET_SOUTH_EAST:
		{
			Points[0].x = FirstOwner->Position.x + 0.5;
			Points[0].y = FirstOwner->Position.y - 0.5;
			Points[0].z = FirstOwner->Position.z - 0.5;

			Points[1].x = FirstOwner->Position.x + 0.5;
			Points[1].y = FirstOwner->Position.y + 0.5;
			Points[1].z = FirstOwner->Position.z - 0.5;

			Points[2].x = FirstOwner->Position.x + 0.5;
			Points[2].y = FirstOwner->Position.y + 0.5;
			Points[2].z = FirstOwner->Position.z + 0.5;

			Points[3].x = FirstOwner->Position.x + 0.5;
			Points[3].y = FirstOwner->Position.y - 0.5;
			Points[3].z = FirstOwner->Position.z + 0.5;

			break;
		}
		case FACET_SOUTH_WEST:
		{
			Points[0].x = FirstOwner->Position.x + 0.5;
			Points[0].y = FirstOwner->Position.y + 0.5;
			Points[0].z = FirstOwner->Position.z - 0.5;

			Points[1].x = FirstOwner->Position.x - 0.5;
			Points[1].y = FirstOwner->Position.y + 0.5;
			Points[1].z = FirstOwner->Position.z - 0.5;

			Points[2].x = FirstOwner->Position.x - 0.5;
			Points[2].y = FirstOwner->Position.y + 0.5;
			Points[2].z = FirstOwner->Position.z + 0.5;

			Points[3].x = FirstOwner->Position.x + 0.5;
			Points[3].y = FirstOwner->Position.y + 0.5;
			Points[3].z = FirstOwner->Position.z + 0.5;

			break;
		}
		case FACET_NORTH_WEST:
		{
			Points[0].x = FirstOwner->Position.x - 0.5;
			Points[0].y = FirstOwner->Position.y + 0.5;
			Points[0].z = FirstOwner->Position.z - 0.5;

			Points[1].x = FirstOwner->Position.x - 0.5;
			Points[1].y = FirstOwner->Position.y - 0.5;
			Points[1].z = FirstOwner->Position.z - 0.5;

			Points[2].x = FirstOwner->Position.x - 0.5;
			Points[2].y = FirstOwner->Position.y - 0.5;
			Points[2].z = FirstOwner->Position.z + 0.5;

			Points[3].x = FirstOwner->Position.x - 0.5;
			Points[3].y = FirstOwner->Position.y + 0.5;
			Points[3].z = FirstOwner->Position.z + 0.5;

			break;
		}
	}

	return true;
}

bool Face::CheckRemoval()
{
	if (FirstOwner != NULL && FirstOwner->Initalized)
	{
        if (SecondOwner != NULL && SecondOwner->Initalized)
        {
            return true;
        }
	}
	return false;
}

bool Face::Update()
{
	return true;
}

bool Face::Draw()
{
    if(Visible)
    {
        SDL_Rect TexRect = TEXTURE->TextureCordinates[/*DATA->Materials[*/Material/*]->getTexture()*/];

        glTexCoord2i(TexRect.x, TexRect.h);         glVertex3f(Points[0].x, Points[0].y, Points[0].z);
        glTexCoord2i(TexRect.w, TexRect.h);         glVertex3f(Points[1].x, Points[1].y, Points[1].z);
        glTexCoord2i(TexRect.w, TexRect.y);         glVertex3f(Points[2].x, Points[2].y, Points[2].z);

        glTexCoord2i(TexRect.w, TexRect.y);         glVertex3f(Points[2].x, Points[2].y, Points[2].z);
        glTexCoord2i(TexRect.x, TexRect.y);         glVertex3f(Points[3].x, Points[3].y, Points[3].z);
        glTexCoord2i(TexRect.x, TexRect.h);         glVertex3f(Points[0].x, Points[0].y, Points[0].z);
/*
        glColor3f (0.0, 0.0, 0.0);

        glBegin(GL_LINE_LOOP);
            glVertex3f(Points[0].x, Points[0].y, Points[0].z);
            glVertex3f(Points[1].x, Points[1].y, Points[1].z);
            glVertex3f(Points[2].x, Points[2].y, Points[2].z);
            glVertex3f(Points[3].x, Points[3].y, Points[3].z);
        glEnd();
*/
        SCREEN->IncrementTriangles(2);
    }

	return true;
}
