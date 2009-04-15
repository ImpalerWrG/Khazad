#include <stdafx.h>

#include <Game.h>
#include <Camera.h>
#include <ScreenManager.h>
#include <Cube.h>
#include <Map.h>
#include <Face.h>
#include <Slope.h>

Cube::Cube()
{
	setType(CUBE_ACTOR);

	Visible = false;
	Initalized = false;
	Slopage = NULL;

	for(Uint8 i = 0; i < NUM_FACETS; i++)
	{
		Facets[i] = NULL;
	}

	//GAME->ActorList.push_back(this);
	//ID = (Uint32) GAME->ActorList.size();
}

Cube::~Cube()
{
	for(Uint8 i = 0; i < NUM_FACETS; i++)
	{
		if (Facets[i] != NULL)
		{
			Facets[i]->CheckRemoval();
		}
	}
}

bool Cube::Init(Uint16 MaterialType)
{
	Initalized = true;
    Cube* Neibor = NULL;
    Material = MaterialType;

    Neibor = MAP->getCube((Sint32) Position.x, (Sint32) Position.y, (Sint32) Position.z - 1);
    if(Neibor != NULL)
    {
        if (Neibor->Initalized == false)
        {
            Facets[FACET_BOTTOM] = new Face;
            Facets[FACET_BOTTOM]->Init(this, Neibor, FACET_BOTTOM, MaterialType);
        }
        else
        {
            Neibor->Facets[FACET_TOP]->CheckRemoval();
        }
    }
    else
    {
        Facets[FACET_BOTTOM] = new Face;
        Facets[FACET_BOTTOM]->Init(this, NULL, FACET_BOTTOM, MaterialType);
    }

    Neibor = MAP->getCube((Sint32) Position.x, (Sint32) Position.y, (Sint32) Position.z + 1);
    if(Neibor != NULL)
    {
        if(Neibor->Initalized == false)
        {
            Facets[FACET_TOP] = new Face;
            Facets[FACET_TOP]->Init(this, Neibor, FACET_TOP, MaterialType);
        }
        else
        {
            Neibor->Facets[FACET_BOTTOM]->CheckRemoval();
        }
    }
    else
    {
        Facets[FACET_TOP] = new Face;
        Facets[FACET_TOP]->Init(this, NULL, FACET_TOP, MaterialType);
    }

    Neibor = MAP->getCube((Sint32) Position.x, (Sint32) Position.y -1, (Sint32) Position.z);
    if(Neibor != NULL)
    {
        if(Neibor->Initalized == false)
        {
            Facets[FACET_NORTH_EAST] = new Face;
            Facets[FACET_NORTH_EAST]->Init(this, Neibor, FACET_NORTH_EAST, MaterialType);
        }
        else
        {
            Neibor->Facets[FACET_SOUTH_WEST]->CheckRemoval();
        }
    }
    else
    {
        Facets[FACET_NORTH_EAST] = new Face;
        Facets[FACET_NORTH_EAST]->Init(this, NULL, FACET_NORTH_EAST, MaterialType);
    }

    Neibor = MAP->getCube(Position.x + 1, Position.y, Position.z);
    if(Neibor != NULL)
    {
        if(Neibor->Initalized == false)
        {
            Facets[FACET_SOUTH_EAST] = new Face;
            Facets[FACET_SOUTH_EAST]->Init(this, Neibor, FACET_SOUTH_EAST, MaterialType);
        }
        else
        {
            Neibor->Facets[FACET_NORTH_WEST]->CheckRemoval();
        }
    }
    else
    {
        Facets[FACET_SOUTH_EAST] = new Face;
        Facets[FACET_SOUTH_EAST]->Init(this, NULL, FACET_SOUTH_EAST, MaterialType);
    }

    Neibor = MAP->getCube(Position.x, Position.y + 1, Position.z);
    if(Neibor != NULL )
    {
        if(Neibor->Initalized == false)
        {
            Facets[FACET_SOUTH_WEST] = new Face;
            Facets[FACET_SOUTH_WEST]->Init(this, Neibor, FACET_SOUTH_WEST, MaterialType);
        }
        else
        {
            Neibor->Facets[FACET_NORTH_EAST]->CheckRemoval();
        }
    }
    else
    {
        Facets[FACET_SOUTH_WEST] = new Face;
        Facets[FACET_SOUTH_WEST]->Init(this, NULL, FACET_SOUTH_WEST, MaterialType);
    }

    Neibor = MAP->getCube(Position.x - 1, Position.y, Position.z);
    if(Neibor != NULL)
    {
        if(Neibor->Initalized == false)
        {
            Facets[FACET_NORTH_WEST] = new Face;
            Facets[FACET_NORTH_WEST]->Init(this, Neibor, FACET_NORTH_WEST, MaterialType);
        }
        else
        {
            Neibor->Facets[FACET_SOUTH_EAST]->CheckRemoval();
        }
    }
    else
    {
        Facets[FACET_NORTH_WEST] = new Face;
        Facets[FACET_NORTH_WEST]->Init(this, NULL, FACET_NORTH_WEST, MaterialType);
    }

	return true;
}

void Cube::SetSlope(Slopping Type)
{
    if (Slopage == NULL)
    {
        Slopage = new Slope;
        Slopage->Init(this, Type);
    }
    else
    {
        Slopage->SetSlopeType(Type);
    }
}

bool Cube::Update()
{
	return true;
}

bool Cube::Draw()
{
	if (Initalized && Visible)
	{
	    if(Slopage != NULL)
	    {
	        Slopage->Draw();
	    }
	    else
	    {

            if(Facets[FACET_BOTTOM] != NULL)
            {
                Facets[FACET_BOTTOM]->Draw();
            }

            switch(SCREEN->MainCamera->getDirection())
            {
                case NORTH:
                {
                    if(Facets[FACET_NORTH_EAST] != NULL)
                    {
                        Facets[FACET_NORTH_EAST]->Draw();
                    }
                    if(Facets[FACET_NORTH_WEST] != NULL)
                    {
                        Facets[FACET_NORTH_WEST]->Draw();
                    }
                    break;
                }
                case EAST:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw();
                    }
                    if(Facets[FACET_NORTH_EAST] != NULL)
                    {
                        Facets[FACET_NORTH_EAST]->Draw();
                    }
                    break;
                }
                case SOUTH:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw();
                    }
                    if(Facets[FACET_SOUTH_WEST] != NULL)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw();
                    }
                    break;
                }
                case WEST:
                {
                    if(Facets[FACET_NORTH_WEST] != NULL)
                    {
                        Facets[FACET_NORTH_WEST]->Draw();
                    }
                    if(Facets[FACET_SOUTH_WEST] != NULL)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw();
                    }
                    break;
                }
                case NORTHWEST:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw();
                    }
                    break;
                }
                case NORTHEAST:
                {
                    if (Facets[FACET_SOUTH_WEST] != NULL)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw();
                    }
                    break;
                }
                case SOUTHWEST:
                {
                    if(Facets[FACET_NORTH_EAST] != NULL)
                    {
                        Facets[FACET_NORTH_EAST]->Draw();
                    }
                    break;
                }
                case SOUTHEAST:
                {
                    if(Facets[FACET_NORTH_WEST] != NULL)
                    {
                        Facets[FACET_NORTH_WEST]->Draw();
                    }
                    break;
                }
            }
        }
	}
	return true;
}
