#include <stdafx.h>

#include <Cube.h>
#include <Face.h>
#include <Game.h>
#include <TextureManager.h>
#include <ScreenManager.h>


Face::Face()
{
	setType(FACE_ACTOR);

	FirstOwner = NULL;
	SecondeOwner = NULL;

	GAME->ActorList.push_back(this);
	ID = (Uint32) GAME->ActorList.size();
}

Face::~Face()
{
    //GAME->RemoveActor(ID);
    //delete Points;
}

bool Face::Init(Cube* First, Cube* Second, Facet Type)
{
	FirstOwner = First;
    SecondeOwner = Second;
	FacetType = Type;
	Visible = true;

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

			Texture = 0;
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

			Texture = 0;
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

			Texture = 1;
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

			Texture = 1;
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

			Texture = 1;
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

			Texture = 1;
			break;
		}
	}

	return true;
}

void Face::CheckRemoval()
{
	if (FirstOwner != NULL && FirstOwner->Initalized)
	{
        if (SecondeOwner != NULL && SecondeOwner->Initalized)
        {
            Visible = false;
        }
	}
}

bool Face::Update()
{
	return true;
}

bool Face::Draw()
{
    if(Visible)
    {
        float Shading = SCREEN->getShading(FirstOwner->Position.z);

        glColor3f(Shading, Shading, Shading);

        //glBindTexture(GL_TEXTURE_2D, TEXTURE->SingularTextureLibrary[Texture]);

        //glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(Points[0].x, Points[0].y, Points[0].z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(Points[1].x, Points[1].y, Points[1].z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(Points[2].x, Points[2].y, Points[2].z);
        glTexCoord2f(0.0f, 0.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(Points[2].x, Points[2].y, Points[2].z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(Points[3].x, Points[3].y, Points[3].z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(Points[0].x, Points[0].y, Points[0].z);
        //glEnd();

/*
        glColor3f (0.0, 0.0, 0.0);

        glBegin(GL_LINE_LOOP);
            glVertex3f(Points[0].x, Points[0].y, Points[0].z);
            glVertex3f(Points[1].x, Points[1].y, Points[1].z);
            glVertex3f(Points[2].x, Points[2].y, Points[2].z);
            glVertex3f(Points[3].x, Points[3].y, Points[3].z);
        glEnd();
*/
    }

	return true;
}
