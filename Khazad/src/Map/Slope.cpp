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
	ID = (Uint32) GAME->ActorList.size();
}

Slope::~Slope()
{
    //GAME->RemoveActor(ID);
}

bool Slope::Init(Cube* NewOwner, Slopping Type)
{
    Owner = NewOwner;
    SetSlopeType(Type);

    Texture = 2;
}

void Slope::SetSlopeType(Slopping Type)
{
    SlopeType = Type;
    Visible = true;

    switch(SlopeType)
    {
        case SLOPE_NORTH_EAST:
        {
            Points[0].x = Owner->Position.x + 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x - 0.5;
			Points[1].y = Owner->Position.y + 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;

			Points[3].x = Owner->Position.x + 0.5;
			Points[3].y = Owner->Position.y - 0.5;
			Points[3].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_SOUTH_EAST:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x - 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;

			Points[3].x = Owner->Position.x + 0.5;
			Points[3].y = Owner->Position.y + 0.5;
			Points[3].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_SOUTH_WEST:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y - 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y + 0.5;
			Points[2].z = Owner->Position.z - 0.5;

			Points[3].x = Owner->Position.x - 0.5;
			Points[3].y = Owner->Position.y + 0.5;
			Points[3].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_NORTH_WEST:
        {
			Points[0].x = Owner->Position.x + 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;

			Points[3].x = Owner->Position.x - 0.5;
			Points[3].y = Owner->Position.y + 0.5;
			Points[3].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_LARGE_NORTH:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_LARGE_EAST:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y - 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y + 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_LARGE_SOUTH:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y + 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_LARGE_WEST:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y - 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y + 0.5;
			Points[1].z = Owner->Position.z + 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y + 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_SMALL_NORTH:
        {
			Points[0].x = Owner->Position.x + 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z - 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y + 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_SMALL_EAST:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x - 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z - 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y + 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
        case SLOPE_SMALL_SOUTH:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y - 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x - 0.5;
			Points[1].y = Owner->Position.y + 0.5;
			Points[1].z = Owner->Position.z - 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
       case SLOPE_SMALL_WEST:
        {
			Points[0].x = Owner->Position.x + 0.5;
			Points[0].y = Owner->Position.y - 0.5;
			Points[0].z = Owner->Position.z + 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y + 0.5;
			Points[1].z = Owner->Position.z - 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z - 0.5;
            break;
        }
       case SLOPE_CRESS_NORTH_SOUTH:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y - 0.5;
			Points[0].z = Owner->Position.z - 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y + 0.5;
			Points[1].z = Owner->Position.z - 0.5;

			Points[2].x = Owner->Position.x + 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z + 0.5;

			Points[3].x = Owner->Position.x - 0.5;
			Points[3].y = Owner->Position.y + 0.5;
			Points[3].z = Owner->Position.z + 0.5;
            break;
        }
       case SLOPE_CRESS_EAST_WEST:
        {
			Points[0].x = Owner->Position.x - 0.5;
			Points[0].y = Owner->Position.y + 0.5;
			Points[0].z = Owner->Position.z - 0.5;

			Points[1].x = Owner->Position.x + 0.5;
			Points[1].y = Owner->Position.y - 0.5;
			Points[1].z = Owner->Position.z - 0.5;

			Points[2].x = Owner->Position.x - 0.5;
			Points[2].y = Owner->Position.y - 0.5;
			Points[2].z = Owner->Position.z + 0.5;

			Points[3].x = Owner->Position.x + 0.5;
			Points[3].y = Owner->Position.y + 0.5;
			Points[3].z = Owner->Position.z + 0.5;
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

bool Slope::Draw()
{
    if(Visible)
    {
        float Shading = SCREEN->getShading(Owner->Position.z);

        glColor3f(Shading, Shading, Shading);

        //glBindTexture(GL_TEXTURE_2D, TEXTURE->SingularTextureLibrary[Texture]);

        if (SlopeType == SLOPE_NORTH_EAST || SlopeType == SLOPE_SOUTH_EAST || SlopeType == SLOPE_SOUTH_WEST || SlopeType == SLOPE_NORTH_WEST )
        {
/*
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(Points[3].x, Points[3].y, Points[3].z);
            glEnd();
*/

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

                SCREEN->IncrementTriangles(2);
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

        if (SlopeType == SLOPE_SMALL_NORTH || SlopeType == SLOPE_SMALL_EAST || SlopeType == SLOPE_SMALL_SOUTH || SlopeType == SLOPE_SMALL_WEST )
        {
            //glBegin(GL_TRIANGLES);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);

                SCREEN->IncrementTriangles(1);
            //glEnd();

/*
            glColor3f (0.0, 0.0, 0.0);

            glBegin(GL_LINE_LOOP);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);
            glEnd();
*/
        }

        if (SlopeType == SLOPE_LARGE_NORTH || SlopeType == SLOPE_LARGE_EAST || SlopeType == SLOPE_LARGE_SOUTH || SlopeType == SLOPE_LARGE_WEST )
        {
            //glBegin(GL_TRIANGLES);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);

                SCREEN->IncrementTriangles(1);
            //glEnd();
/*
            glColor3f (0.0, 0.0, 0.0);

            glBegin(GL_LINE_LOOP);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);
            glEnd();
*/
        }

        if (SlopeType == SLOPE_CRESS_NORTH_SOUTH || SlopeType == SLOPE_CRESS_EAST_WEST )
        {
            //glBegin(GL_TRIANGLES);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);

                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(Points[3].x, Points[3].y, Points[3].z);

                SCREEN->IncrementTriangles(2);
            //glEnd();
/*
            glColor3f (0.0, 0.0, 0.0);

            glBegin(GL_LINE_LOOP);
                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glVertex3f(Points[2].x, Points[2].y, Points[2].z);

                glVertex3f(Points[0].x, Points[0].y, Points[0].z);
                glVertex3f(Points[1].x, Points[1].y, Points[1].z);
                glVertex3f(Points[3].x, Points[3].y, Points[3].z);
            glEnd();
*/
        }
    }

    return true;
}
