#include <stdafx.h>

#include <Game.h>
#include <Camera.h>
#include <ScreenManager.h>
#include <Cube.h>
#include <Map.h>
#include <Face.h>
#include <Slope.h>
#include <Random.h>

Cube::Cube()
{
	setType(CUBE_ACTOR);

	Visible = false;
	Initalized = false;
	Slopage = NULL;
	Liquid = false;
	Solid = false;

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

    Solid = true;
    Material = MaterialType;

	return true;
}

bool Cube::InitAllFaces()
{
	Initalized = true;

    if(!(Solid || Liquid))
    {
        return false;
    }

    for(Facet FaceType = FACET_TOP; FaceType < NUM_FACETS; ++FaceType)
    {
        if(Facets[FaceType] == NULL)
        {
            Cube* NeiborCube = getAdjacentCube(FaceType);
            Cell* NeiborCell = getAdjacentCell(FaceType);

            if(NeiborCell != NULL) // && NeiborCell->isInitalized())
            {
                if (NeiborCube != NULL) //&& NeiborCube->isInitalized())
                {
                    Sint16 BestMaterial = FaceMaterial(FaceType);

                    if(BestMaterial != -1)
                    {
                        if(FaceType == FACET_TOP)
                        {
                            Face* TemporaryPointer = new Face;
                            TemporaryPointer->Init(NeiborCube, this, FACET_BOTTOM, (Uint16) BestMaterial);
                        }
                        else
                        {
                            Facets[FaceType] = new Face;
                            Facets[FaceType]->Init(this, NeiborCube, FaceType, (Uint16) BestMaterial);
                        }

                        getCellOwner()->setActive(true);
/*
                        if(FaceType == FACET_TOP || FaceType == FACET_BOTTOM) // Keep roofs of underground rooms invisible
                        {
                            if(!Facets[FaceType]->isConstructed()) // Unless they are themselves floors
                            {
                                Facets[FaceType]->setVisible(false);
                            }
                        }
                        */
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
    Face* ConstructionFace = Facets[FacetType];

    if(ConstructionFace != NULL)  // Change the existing face rather then creating a new one
    {
        ConstructionFace->setConstructed(true);
        ConstructionFace->setMaterial(MaterialType);
        ConstructionFace->setVisible(true);
    }
    else // Face Material is independent of Cube Material
    {
        Facets[FacetType] = new Face;
        Facets[FacetType]->setConstructed(true);
        Facets[FacetType]->Init(this, getAdjacentCube(FacetType), FacetType, MaterialType);
        Facets[FacetType]->setVisible(true);
    }

    getCellOwner()->setActive(true);
}

Sint16 Cube::FaceMaterial(Facet Type)
{
    Cube* NeiborCube = getAdjacentCube(Type);

    if(NeiborCube != NULL) // && NeiborCube->isInitalized())
    {
        if(Solid)
        {
            if(NeiborCube->isSolid())
            {
                return -1; // No solid-solid surfaces displayed
            }
            else
            {
                return Material;  // Solid materials dominated all others
            }
        }
        else
        {
            if(NeiborCube->isSolid())
            {
                return NeiborCube->getMaterial();
            }
            else
            {
                if(Liquid)
                {
                    if(!NeiborCube->getLiquid())
                    {
                        return 2;
                    }
                    else
                    {
                        return -1;
                    }
                }
                else
                {
                    if (NeiborCube->getLiquid())
                    {
                        return 2;
                    }

                    return -1;  // No Open-Open surfaces
                }
            }
        }
    }

    return -1; // No Null-Solid sufaces
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
}

void Cube::SetSlope(Slopping Type)
{
    Solid = false;
    Initalized = true;

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
    else
    {
        SetSlope(SLOPE_SOUTH_WEST);  // Default slope for isolated slopes, possibly replace with something better??
    }
}

bool Cube::Update()
{
	return true;
}

bool Cube::Draw(CameraOrientation Orientation, float xTranslate, float yTranslate, bool DrawHidden)
{
	if (isInitalized())
	{
        if(Hidden && !DrawHidden)
        {
            return true;
        }

	    if(Slopage != NULL)
	    {
	        Slopage->Draw(xTranslate, yTranslate);

	        return true;
	    }
	    else
	    {
            if(Facets[FACET_BOTTOM] != NULL && Facets[FACET_BOTTOM]->getFirstOwner() == this)
            {
                Facets[FACET_BOTTOM]->Draw(xTranslate, yTranslate);
            }

            if(Facets[FACET_TOP] != NULL && Facets[FACET_TOP]->getFirstOwner() == this)
            {
                Facets[FACET_TOP]->Draw(xTranslate, yTranslate);
            }
	    }

	    if(true) // Add option here
	    {
            if(Orientation == CAMERA_DOWN)
            {
                return true;
            }
            if(Facets[FACET_NORTH_EAST] != NULL && Facets[FACET_NORTH_EAST]->getFirstOwner() == this)
            {
                Facets[FACET_NORTH_EAST]->Draw(xTranslate, yTranslate);
            }
            if(Facets[FACET_NORTH_WEST] != NULL && Facets[FACET_NORTH_WEST]->getFirstOwner() == this)
            {
                Facets[FACET_NORTH_WEST]->Draw(xTranslate, yTranslate);
            }
            if(Facets[FACET_SOUTH_EAST] != NULL && Facets[FACET_SOUTH_EAST]->getFirstOwner() == this)
            {
                Facets[FACET_SOUTH_EAST]->Draw(xTranslate, yTranslate);
            }
            if(Facets[FACET_SOUTH_WEST] != NULL && Facets[FACET_SOUTH_WEST]->getFirstOwner() == this)
            {
                Facets[FACET_SOUTH_WEST]->Draw(xTranslate, yTranslate);
            }
            return true;
	    }

        if(Solid)
        {
            switch(Orientation)
            {
                case CAMERA_SOUTH:
                {
                    if(Facets[FACET_NORTH_EAST] != NULL && Facets[FACET_NORTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_NORTH_WEST] != NULL && Facets[FACET_NORTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_WEST:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL && Facets[FACET_SOUTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_NORTH_EAST] != NULL && Facets[FACET_NORTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_NORTH:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL && Facets[FACET_SOUTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_SOUTH_WEST] != NULL && Facets[FACET_SOUTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_EAST:
                {
                    if(Facets[FACET_NORTH_WEST] != NULL && Facets[FACET_NORTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_SOUTH_WEST] != NULL && Facets[FACET_SOUTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
            }
        }
        else //Draw the oposite sides to enclose rooms
        {
            switch(Orientation)
            {
                case CAMERA_NORTH:
                {
                    if(Facets[FACET_NORTH_EAST] != NULL && Facets[FACET_NORTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_NORTH_WEST] != NULL && Facets[FACET_NORTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_EAST:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL && Facets[FACET_SOUTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_NORTH_EAST] != NULL && Facets[FACET_NORTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_SOUTH:
                {
                    if(Facets[FACET_SOUTH_EAST] != NULL && Facets[FACET_SOUTH_EAST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_EAST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_SOUTH_WEST] != NULL && Facets[FACET_SOUTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_WEST:
                {
                    if(Facets[FACET_NORTH_WEST] != NULL && Facets[FACET_NORTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_NORTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    if(Facets[FACET_SOUTH_WEST] != NULL && Facets[FACET_SOUTH_WEST]->getFirstOwner() == this)
                    {
                        Facets[FACET_SOUTH_WEST]->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
            }
        }
	}

	return true;
}
