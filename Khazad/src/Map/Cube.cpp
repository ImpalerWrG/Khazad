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
	Liquid = false;

	for(Uint8 i = 0; i < NUM_FACETS; i++)
	{
		Facets[i] = NULL;
	}
}

Cube::~Cube()
{
	for(Uint8 i = 0; i < NUM_FACETS; i++)
	{
		if (Facets[i] != NULL)
		{
			//Facets[i]->CheckRemoval();
			// Not sure how to handle yet
		}
	}
}

bool Cube::Init(Uint16 MaterialType)
{
	Initalized = true;

    Material = MaterialType;

	return true;
}

bool Cube::InitAllFaces()
{
   // if(Initalized)
    //{
        for(Uint8 i = 0; i < NUM_FACETS; i++)
        {
            Cube* NeiborCube = getNeiborCube((Facet) i);
            Cell* NeiborCell = getNeiborCell((Facet) i);

            if(NeiborCell != NULL && NeiborCell->Initalized && !NeiborCell->isBasment())
            {
                if (NeiborCube != NULL) //&& NeiborCube->Initalized == false && NeiborCube->Facets[OpositeFace(Type)] == NULL)
                {
                    if(Solid ^ NeiborCube->isSolid())  // Faces are only needed ware a sold and non-solid meet
                    {
                        InitFace((Facet) i);
                    }
                }
            }
        }
    //}
}

bool Cube::setMaterial(Uint16 MaterialType)
{
    Material = MaterialType;

    for(Uint8 i = 0; i < NUM_FACETS; i++)
    {
        if (!Facets[i]->isConstructed())
        {
            Facets[i]->setMaterial(Material);
        }
    }
}

bool Cube::Open()
{
    Initalized = true;
    Solid = false;

    for(Uint8 i = 0; i < NUM_FACETS; i++)
    {
        Cube* NeiborCube = getNeiborCube((Facet) i);
        Cell* NeiborCell = getNeiborCell((Facet) i);

        if(NeiborCell != NULL && NeiborCell->Initalized && !NeiborCell->isBasment())
        {
            if (NeiborCube != NULL) //&& NeiborCube->Initalized == false && NeiborCube->Facets[OpositeFace(Type)] == NULL)
            {
                if(NeiborCube->isSolid())  // Faces are only needed ware a sold and non-solid meet
                {
                    InitFace((Facet) i);
                }
                else  // Remove faces between two Open Cubes
                {
                    NeiborCube->DeleteFace(OpositeFace((Facet) i));
                }
            }
            else
            {
                    // What to do? Initilize neiboring Cell and Cube?
            }
        }
    }

    RemoveSlope();
}

Face* Cube::getFacet(Facet Type)
{
    if(Initalized)
    {
        return Facets[Type];
    }
    return NULL;
}

void Cube::setFacet(Facet FacetType, Face* NewFace)
{
    Facets[FacetType] = NewFace;
}

void Cube::InitConstructedFace(Facet FacetType, Uint16 MaterialType)
{
    Cube* NeiborCube = getNeiborCube(FacetType);

    if(NeiborCube != NULL)
    {
        Face* OpposingFace = NeiborCube->getFacet(Cube::OpositeFace(FacetType));
        if(OpposingFace != NULL)
        {
            OpposingFace->setMaterial(MaterialType);  // Change the existing face rather then creating a new one
            OpposingFace->setConstructed(true);
        }
        else
        {
            Facets[Type] = new Face;
            Facets[Type]->setConstructed(true);
            Facets[Type]->Init(this, NeiborCube, FacetType, MaterialType);  // Face Material is independent of Cube Material
        }
    }
    else
    {
        Facets[Type] = new Face;
        Facets[Type]->setConstructed(true);
        Facets[Type]->Init(this, NeiborCube, FacetType, MaterialType);
    }
}

void Cube::InitFace(Facet Type)
{
    Cube* NeiborCube = getNeiborCube(Type);

    Facets[Type] = new Face;

    if(!Solid)
    {
        Facets[Type]->Init(this, NeiborCube, Type, NeiborCube->getMaterial());
    }
    else
    {
        Facets[Type]->Init(this, NeiborCube, Type, Material);
    }
}

Cube* Cube::getNeiborCube(Facet Type)
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
        case FACET_NORTH_EAST:
            y -= 1;
            break;
        case FACET_SOUTH_EAST:
            x += 1;
            break;
        case FACET_SOUTH_WEST:
            y += 1;
            break;
        case FACET_NORTH_WEST:
            x -= 1;
            break;
    }

    return MAP->getCube(x, y, z);
}

Cell* Cube::getNeiborCell(Facet Type)
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
        case FACET_NORTH_EAST:
            y -= 1;
            break;
        case FACET_SOUTH_EAST:
            x += 1;
            break;
        case FACET_SOUTH_WEST:
            y += 1;
            break;
        case FACET_NORTH_WEST:
            x -= 1;
            break;
    }

    return MAP->getCubeOwner(x, y, z);
}

Facet Cube::OpositeFace(Facet Type)
{
    switch(Type)
    {
        case FACET_TOP:
            return FACET_BOTTOM;

        case FACET_BOTTOM:
            return FACET_TOP;

        case FACET_NORTH_EAST:
            return FACET_SOUTH_WEST;

        case FACET_SOUTH_EAST:
            return FACET_NORTH_WEST;

        case FACET_SOUTH_WEST:
            return FACET_NORTH_EAST;

        case FACET_NORTH_WEST:
            return FACET_SOUTH_EAST;
    }
}

void Cube::DeleteFace(Facet Type)
{
    Face* Target = NULL;
    Cube* Neibor = NULL;
    Target = Facets[Type];

    if (Target != NULL)
    {
        // Find other owner of target Face
        if(Target->getFirstOwner() == this)
        {
            Neibor = Target->getSecondOwner();
        }
        else
        {
            Neibor = Target->getFirstOwner();
        }

        if(Neibor)  // Null the neibors reference to face
        {
            Neibor->Facets[OpositeFace(Type)] = NULL;
        }
        // And our own reference
        Facets[Type] = NULL;

        delete Target;
    }
	// Face is not present
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

void Cube::RemoveSlope()
{
    if (Slopage != NULL)
    {
        delete Slopage;
        Slopage = NULL;
    }
}

void Cube::setAllFacesVisiblity(bool NewValue)
{
    for(Uint8 i = 0; i < NUM_FACETS; i++)
    {
        if(Facets[i] != NULL)
        {
            Facets[i]->setVisible(NewValue);
        }
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

            if(SCREEN->MainCamera->isAllFacesDrawing())
            {
                if(Facets[FACET_NORTH_EAST] != NULL)
                {
                    Facets[FACET_NORTH_EAST]->Draw();
                }
                if(Facets[FACET_NORTH_WEST] != NULL)
                {
                    Facets[FACET_NORTH_WEST]->Draw();
                }
                if(Facets[FACET_SOUTH_EAST] != NULL)
                {
                    Facets[FACET_SOUTH_EAST]->Draw();
                }
                if(Facets[FACET_SOUTH_WEST] != NULL)
                {
                    Facets[FACET_SOUTH_WEST]->Draw();
                }
            }
            else
            {
                switch(SCREEN->MainCamera->getDirection())
                {
                    case NORTH:
                    case NORTHEAST:
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
                    case SOUTHEAST:
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
                    case SOUTHWEST:
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
                    case NORTHWEST:
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
                /*
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
                */
                }
            }
        }
	}
	return true;
}
