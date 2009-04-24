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
	Solid = true;

	Material = 6;

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
	Initalized = true;

    for(Uint8 i = 0; i < NUM_FACETS; i++)
    {
        Cube* NeiborCube = getAdjacentCube((Facet) i);
        Cell* NeiborCell = getAdjacentCell((Facet) i);

        if(NeiborCell != NULL && NeiborCell->Initalized && !NeiborCell->isBasment())
        {
            if (NeiborCube != NULL && NeiborCube->Initalized) //&& NeiborCube->Initalized == false && NeiborCube->Facets[OpositeFace(Type)] == NULL)
            {
                if(Solid ^ NeiborCube->isSolid())  // Faces are only needed ware a sold and non-solid meet
                {
                    Face* NeiborFace = NeiborCube->Facets[OpositeFace((Facet) i)];
                    if(NeiborFace != NULL && !NeiborFace->isConstructed())
                    {
                        InitFace((Facet) i);
                    }
                    if(i == FACET_TOP || i == FACET_BOTTOM) // Keep roofs of underground rooms invisible
                    {
                        if(Facets[i])
                        {
                            if(!Facets[i]->isConstructed()) // Unless they are themselves floors
                            {
                                Facets[i]->setVisible(false);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool Cube::setMaterial(Uint16 MaterialType)
{
    Initalized = true;
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
        Cube* NeiborCube = getAdjacentCube((Facet) i);
        Cell* NeiborCell = getAdjacentCell((Facet) i);

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
    Cube* NeiborCube = getAdjacentCube(FacetType);

    if(NeiborCube != NULL)
    {
        Face* OpposingFace = NeiborCube->getFacet(Cube::OpositeFace(FacetType));
        if(OpposingFace != NULL)
        {
            OpposingFace->setMaterial(MaterialType);  // Change the existing face rather then creating a new one
            OpposingFace->setConstructed(true);
            OpposingFace->setVisible(true);
        }
        else
        {
            Facets[FacetType] = new Face;
            Facets[FacetType]->setConstructed(true);
            Facets[FacetType]->Init(this, NeiborCube, FacetType, MaterialType);  // Face Material is independent of Cube Material
        }
    }
    else
    {
        Facets[FacetType] = new Face;
        Facets[FacetType]->setConstructed(true);
        Facets[FacetType]->Init(this, NeiborCube, FacetType, MaterialType);
    }
}

void Cube::InitFace(Facet Type)
{
    Cube* NeiborCube = getAdjacentCube(Type);

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

Cube* Cube::getNeiborCube(Direction Type)
{
    Sint32 x = Position.x;
    Sint32 y = Position.y;
    Sint32 z = Position.z;

    switch(Type)
    {
        case NORTHEAST:
            y -= 1;
            break;
        case SOUTHEAST:
            x += 1;
            break;
        case SOUTHWEST:
            y += 1;
            break;
        case NORTHWEST:
            x -= 1;
            break;

        case NORTH:
            y -= 1;
            x -= 1;
            break;
        case SOUTH:
            x += 1;
            y += 1;
            break;
        case WEST:
            x -= 1;
            y += 1;
            break;
        case EAST:
            x += 1;
            y -= 1;
            break;
    }

    return MAP->getCube(x, y, z);
}

Cell* Cube::getCellOwner()
{
    return MAP->getCubeOwner(Position.x, Position.y, Position.z);
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
    Solid = false;

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

void Cube::DetermineSlope()
{
    Slopping SlopeType = SLOPE_FLAT;

    bool NorthEastSolid = false;
    bool SouthEastSolid = false;
    bool NorthWestSolid = false;
    bool SouthWestSolid = false;

    bool NorthSolid = false;
    bool SouthSolid = false;
    bool WestSolid = false;
    bool EastSolid = false;

    if(getAdjacentCube(FACET_NORTH_EAST) != NULL && getAdjacentCube(FACET_NORTH_EAST)->isSolid())
    {
        NorthEastSolid = true;
    }

    if(getAdjacentCube(FACET_SOUTH_EAST) != NULL && getAdjacentCube(FACET_SOUTH_EAST)->isSolid())
    {
        SouthEastSolid = true;
    }

    if(getAdjacentCube(FACET_NORTH_WEST) != NULL && getAdjacentCube(FACET_NORTH_WEST)->isSolid())
    {
       NorthWestSolid = true;
    }

    if(getAdjacentCube(FACET_SOUTH_WEST) != NULL && getAdjacentCube(FACET_SOUTH_WEST)->isSolid())
    {
        SouthWestSolid = true;
    }


    if(getNeiborCube(NORTH) != NULL && getNeiborCube(NORTH)->isSolid())
    {
        NorthSolid = true;
    }

    if(getNeiborCube(SOUTH) != NULL && getNeiborCube(SOUTH)->isSolid())
    {
        SouthSolid = true;
    }

    if(getNeiborCube(WEST) != NULL && getNeiborCube(WEST)->isSolid())
    {
       WestSolid = true;
    }

    if(getNeiborCube(EAST) != NULL && getNeiborCube(EAST)->isSolid())
    {
        EastSolid = true;
    }


    if(NorthEastSolid)
    {
        SlopeType = SLOPE_SOUTH_WEST;
    }

    if(SouthEastSolid)
    {
        SlopeType = SLOPE_NORTH_WEST;
    }

    if(NorthWestSolid)
    {
        SlopeType = SLOPE_SOUTH_EAST;
    }




    if(SouthWestSolid)
    {
        SlopeType = SLOPE_NORTH_EAST;
    }

    if(NorthEastSolid && SouthEastSolid)
    {
        SlopeType = SLOPE_LARGE_WEST;
    }

    if(SouthEastSolid && SouthWestSolid)
    {
        SlopeType = SLOPE_LARGE_NORTH;
    }

    if(NorthWestSolid && SouthWestSolid)
    {
        SlopeType = SLOPE_LARGE_EAST;
    }

    if(NorthEastSolid && NorthWestSolid)
    {
        SlopeType = SLOPE_LARGE_SOUTH;
    }


    if(SlopeType == SLOPE_FLAT) // No slope yet found, try corners
    {
        if(SouthSolid)
        {
            SlopeType = SLOPE_SMALL_NORTH;
        }

        if(NorthSolid)
        {
            SlopeType = SLOPE_SMALL_SOUTH;
        }

        if(EastSolid)
        {
            SlopeType = SLOPE_SMALL_WEST;
        }

        if(WestSolid)
        {
            SlopeType = SLOPE_SMALL_EAST;
        }
    }

    if(SlopeType != SLOPE_FLAT)
    {
        SetSlope(SlopeType);
    }
}

bool Cube::Update()
{
	return true;
}

bool Cube::Draw()
{
	if (Initalized ) //&& Visible)
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
