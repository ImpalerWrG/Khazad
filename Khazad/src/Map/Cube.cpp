#include <stdafx.h>

#include <Cube.h>
#include <Cell.h>
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
    Faceted = false;

	Material = 0;

	setPosition(0.0, 0.0, 0.0);
    MAP->ChangeCubeCount(1);
}

void Cube::SetOwner(Cell* NewOwner, Uint8 X, Uint8 Y)
{
    Owner = NewOwner;
    CellX = X % CELLEDGESIZE;
    CellY = Y % CELLEDGESIZE;

    Vector3 OwnerPosition = Owner->getPosition();

    float RealX = OwnerPosition.x - (float)(CELLEDGESIZE / 2) + (float)X + (float)HALFCUBE;
    float RealY = OwnerPosition.y - (float)(CELLEDGESIZE / 2) + (float)Y + (float)HALFCUBE;
	setPosition(RealX, RealY, OwnerPosition.z);
}

Cube::~Cube()
{
    MAP->ChangeCubeCount(-1);

    if(Initalized)
    {
        MAP->ChangeInitedCubeCount(-1);
    }
    if(Slopage != NULL)
    {
        RemoveSlope();
    }
}

bool Cube::Init(Uint16 MaterialType)
{
	Initalized = true;

    Solid = true;
    Material = MaterialType;

    MAP->ChangeInitedCubeCount(1);

	return true;
}

bool Cube::InitFacesOpen()
{
    for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
    {
        Cube* NeiborCube = getNeiborCube(DirectionType);

        if(NeiborCube != NULL && NeiborCube->isSolid())
        {
            NeiborCube->InitFaces();
        }
    }
}

bool Cube::InitFacesSolid()
{
    if(!Hidden)
    {
        for(Facet FaceType = FACETS_START; FaceType < NUM_FACETS; ++FaceType)
        {
            if(getFacet(FaceType) == NULL)
            {
                if(FaceType == FACET_TOP)
                {
                    InitFace(FaceType);
                    continue;
                }

                if(getAdjacentCube(FaceType) != NULL)
                {
                    if(!getAdjacentCube(FaceType)->isSolid() || getAdjacentCube(FaceType)->isHidden())
                    {
                        InitFace(FaceType);
                    }
                }
                else
                {
                    InitFace(FaceType);
                }
            }
        }
    }
}

bool Cube::InitFaces()
{
    Initalized = true;

    for(Facet FaceType = FACETS_START; FaceType < NUM_FACETS; ++FaceType)
    {
        /*
        Cube* NeiborCube = getAdjacentCube(FaceType);

        if(NeiborCube != NULL && !NeiborCube->isSolid())
        {
            InitFace(FaceType);
        }
        */
        if(getFacet(FaceType) == NULL)
        {
            InitFace(FaceType);
        }
    }
}

bool Cube::InitFace(Facet FaceType)
{
    Face* TemporaryPointer = new Face;
    TemporaryPointer->Init(FaceType, Material);
    setFacet(TemporaryPointer, FaceType);
    getCellOwner()->setActive(true);

    Faceted = true;

    return true;
}

bool Cube::setMaterial(Uint16 MaterialType)
{
    Initalized = true;
    Material = MaterialType;

    for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
    {
        if (!getFacet(Face)->isConstructed())
        {
            getFacet(Face)->setMaterial(Material);
        }
    }
}

bool Cube::Open()
{
    Initalized = true;
    Solid = false;

    RemoveSlope();
}

Face* Cube::getFacet(Facet FacetType)
{
    return Owner->getFace(CellX, CellY, FacetType);
}

void Cube::setFacet(Face* NewFace, Facet FacetType)
{
    Owner->setFace(NewFace, CellX, CellY, FacetType);
    if(NewFace != NULL)
    {
        Faceted = true;
    }
    else
    {
        CheckFaceted();
    }
}

void Cube::InitConstructedFace(Facet FacetType, Uint16 MaterialType)
{
    Face* ConstructionFace = getFacet(FacetType);
    Initalized = true;

    if(ConstructionFace != NULL)  // Change the existing face rather then creating a new one
    {
        ConstructionFace->setConstructed(true);
        ConstructionFace->setMaterial(MaterialType);
        ConstructionFace->setVisible(true);
    }
    else // Face Material is independent of Cube Material
    {
        ConstructionFace = new Face;
        ConstructionFace->setConstructed(true);
        ConstructionFace->Init(FacetType, MaterialType);
        ConstructionFace->setVisible(true);
        Owner->setFace(ConstructionFace, CellX, CellY, FacetType);
    }

    Faceted = true;
    Owner->setActive(true);
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
        case FACET_NORTH:
            y -= 1;
            break;
        case FACET_EAST:
            x += 1;
            break;
        case FACET_SOUTH:
            y += 1;
            break;
        case FACET_WEST:
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
        case DIRECTION_NORTH:
            y -= 1;
            break;
        case DIRECTION_EAST:
            x += 1;
            break;
        case DIRECTION_SOUTH:
            y += 1;
            break;
        case DIRECTION_WEST:
            x -= 1;
            break;

        case DIRECTION_NORTHEAST:
            y -= 1;
            x += 1;
            break;
        case DIRECTION_SOUTHEAST:
            y += 1;
            x += 1;
            break;
        case DIRECTION_SOUTHWEST:
            y += 1;
            x -= 1;
            break;
        case DIRECTION_NORTHWEST:
            y -= 1;
            x -= 1;
            break;

        case DIRECTION_UP:
            z += 1;
            break;
        case DIRECTION_DOWN:
            z -= 1;
            break;
    }

    return MAP->getCube(x, y, z);
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
        case FACET_NORTH:
            y -= 1;
            break;
        case FACET_EAST:
            x += 1;
            break;
        case FACET_SOUTH:
            y += 1;
            break;
        case FACET_WEST:
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

        case FACET_NORTH:
            return FACET_SOUTH;

        case FACET_SOUTH:
            return FACET_NORTH;

        case FACET_WEST:
            return FACET_EAST;

        case FACET_EAST:
            return FACET_WEST;
    }
}

void Cube::CheckFaceted()
{
    Faceted = false;
    for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
    {
        if(getFacet(Face))
        {
            Faceted = true;
        }
    }
}

void Cube::DeleteFace(Facet FaceType)
{
    Face* Target = NULL;
    Target = getFacet(FaceType);

    if (Target != NULL)
    {
        setFacet(NULL, FaceType);
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
    for(Facet Face = FACETS_START; Face < NUM_FACETS; ++Face)
    {
        if(getFacet(Face))
        {
            getFacet(Face)->setVisible(NewValue);
        }
    }
}

void Cube::DetermineSlope()
{
    Slopping SlopeType = SLOPE_FLAT;

/*    bool NorthEastSolid = getAdjacentCube(FACET_NORTH_EAST) != NULL && getAdjacentCube(FACET_NORTH_EAST)->isSolid();
    bool SouthEastSolid = getAdjacentCube(FACET_SOUTH_EAST) != NULL && getAdjacentCube(FACET_SOUTH_EAST)->isSolid();
    bool NorthWestSolid = getAdjacentCube(FACET_NORTH_WEST) != NULL && getAdjacentCube(FACET_NORTH_WEST)->isSolid();
    bool SouthWestSolid = getAdjacentCube(FACET_SOUTH_WEST) != NULL && getAdjacentCube(FACET_SOUTH_WEST)->isSolid();

    bool NorthSolid = getNeiborCube(DIRECTION_NORTH) != NULL && getNeiborCube(DIRECTION_NORTH)->isSolid();
    bool SouthSolid = getNeiborCube(DIRECTION_SOUTH) != NULL && getNeiborCube(DIRECTION_SOUTH)->isSolid();
    bool WestSolid = getNeiborCube(DIRECTION_WEST) != NULL && getNeiborCube(DIRECTION_WEST)->isSolid();
    bool EastSolid = getNeiborCube(DIRECTION_EAST) != NULL && getNeiborCube(DIRECTION_EAST)->isSolid();*/
/*
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
    }*/
    SetSlope(SLOPE_SOUTH_WEST);
}

bool Cube::Update()
{
	return true;
}

Vector3 Cube::ConvertSpacialPoint(SpacialPoint Point)
{
    switch(Point)
    {
        case SCACIAL_POINT_CENTER:
            return Vector3(0.0, 0.0, 0.0);

        case SPACIAL_POINT_NORTH_TOP:
            return Vector3(-0.5, -0.5, 0.5);

        case SPACIAL_POINT_EAST_TOP:
            return Vector3(0.5, -0.5, 0.5);

        case SPACIAL_POINT_SOUTH_TOP:
            return Vector3(0.5, 0.5 , 0.5);

        case SPACIAL_POINT_WEST_TOP:
            return Vector3(-0.5, 0.5, 0.5);

        case SPACIAL_POINT_NORTH_BOTTOM:
            return Vector3(-0.5, -0.5, -0.5);

        case SPACIAL_POINT_EAST_BOTTOM:
            return Vector3(0.5, -0.5, -0.5);

        case SPACIAL_POINT_SOUTH_BOTTOM:
            return Vector3(0.5, 0.5, -0.5);

        case SPACIAL_POINT_WEST_BOTTOM:
            return Vector3(-0.5, 0.5, -0.5);

        Default:
            return Vector3(0.5, 0.5  -0.5);
    }

    return Vector3(0.0, 0.0, 0.0);
}

bool Cube::Draw(CameraOrientation Orientation, float xTranslate, float yTranslate)
{
    return false;

/*
	if (isInitalized())
	{
        if(Hidden && !DrawHidden)
        {
            return true;
        }
        if(SubTerranian && !DrawSubTerranian)
        {
            return true;
        }
        if(SkyView && !DrawSkyView)
        {
            return true;
        }
        if(SunLit && !DrawSunLit)
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
            if(getFacet(FACET_BOTTOM) != NULL)
            {
                getFacet(FACET_BOTTOM)->Draw(xTranslate, yTranslate);
            }

            if(getFacet(FACET_TOP) != NULL)
            {
                getFacet(FACET_TOP)->Draw(xTranslate, yTranslate);
            }
	    }

        if(Solid)
        {
            switch(Orientation)
            {
                case CAMERA_SOUTH:
                {
                    if(getFacet(FACET_NORTH_EAST) != NULL)
                    {
                        getFacet(FACET_NORTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_NORTH_WEST) != NULL)
                    {
                        getFacet(FACET_NORTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_WEST:
                {
                    if(getFacet(FACET_SOUTH_EAST) != NULL)
                    {
                        getFacet(FACET_SOUTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_NORTH_EAST) != NULL)
                    {
                        getFacet(FACET_NORTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_NORTH:
                {
                    if(getFacet(FACET_SOUTH_EAST) != NULL)
                    {
                        getFacet(FACET_SOUTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_SOUTH_WEST) != NULL)
                    {
                        getFacet(FACET_SOUTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_EAST:
                {
                    if(getFacet(FACET_NORTH_WEST) != NULL)
                    {
                        getFacet(FACET_NORTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_SOUTH_WEST) != NULL)
                    {
                        getFacet(FACET_SOUTH_WEST)->Draw(xTranslate, yTranslate);
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
                    if(getFacet(FACET_NORTH_EAST) != NULL)
                    {
                        getFacet(FACET_NORTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_NORTH_WEST) != NULL)
                    {
                        getFacet(FACET_NORTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_EAST:
                {
                    if(getFacet(FACET_SOUTH_EAST) != NULL)
                    {
                        getFacet(FACET_SOUTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_NORTH_EAST) != NULL)
                    {
                        getFacet(FACET_NORTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_SOUTH:
                {
                    if(getFacet(FACET_SOUTH_EAST) != NULL)
                    {
                        getFacet(FACET_SOUTH_EAST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_SOUTH_WEST) != NULL)
                    {
                        getFacet(FACET_SOUTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
                case CAMERA_WEST:
                {
                    if(getFacet(FACET_NORTH_WEST) != NULL)
                    {
                        getFacet(FACET_NORTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    if(getFacet(FACET_SOUTH_WEST) != NULL)
                    {
                        getFacet(FACET_SOUTH_WEST)->Draw(xTranslate, yTranslate);
                    }
                    break;
                }
            }
        }
	}

	return true;
	*/
}

void Cube::Dig()
{
    if(Solid)
    {
        Solid = false;
        Faceted = false;
        setHidden(false);

        Cell* TargetCell = getCellOwner();
        TargetCell->setDirtyDrawList(true);

        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            if(DirectionType != DIRECTION_DOWN)
            {
                Cube* NeiborCube = getNeiborCube(DirectionType);

                if(NeiborCube != NULL && NeiborCube->isHidden())
                {
                    NeiborCube->setHidden(false);
                    NeiborCube->getCellOwner()->setDirtyDrawList(true);
                }

                if(NeiborCube != NULL && NeiborCube->isSolid())
                {
                    NeiborCube->InitFacesSolid();
                    NeiborCube->getCellOwner()->setDirtyDrawList(true);
                }
            }
        }

        for(Facet FaceType = FACETS_START; FaceType < NUM_FACETS; ++FaceType)
        {
            if(FaceType != FACET_TOP && FaceType != FACET_BOTTOM)
            {
                DeleteFace(FaceType);
            }
        }

        if(getFacet(FACET_BOTTOM) == NULL)
        {
            InitFace(FACET_BOTTOM);
        }
        else
        {
            if(!getFacet(FACET_BOTTOM)->isConstructed())
            {
                InitFace(FACET_BOTTOM);
            }
        }
    }
    else if(Slopage)
    {
        RemoveSlope();
        InitFace(FACET_BOTTOM);
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            if(DirectionType != DIRECTION_DOWN)
            {
                Cube* NeiborCube = getNeiborCube(DirectionType);
                NeiborCube->getCellOwner()->setDirtyDrawList(true);
            }
        }
    }
    else if(!Solid && !Slopage)
    {
        DeleteFace(FACET_BOTTOM);
        Cube * LowerCube = getNeiborCube(DIRECTION_DOWN);
        getCellOwner()->setDirtyDrawList(true);
        if(LowerCube)
        {
            for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
            {
                Cube* NeiborCube = LowerCube->getNeiborCube(DirectionType);

                if(NeiborCube != NULL && NeiborCube->isHidden())
                {
                    NeiborCube->setHidden(false);
                    NeiborCube->getCellOwner()->setDirtyDrawList(true);
                }

                if(NeiborCube != NULL && NeiborCube->isSolid())
                {
                    NeiborCube->InitFacesSolid();
                    NeiborCube->getCellOwner()->setDirtyDrawList(true);
                }
            }
        }
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            Cube* NeiborCube = getNeiborCube(DirectionType);

            if(NeiborCube != NULL && NeiborCube->isHidden())
            {
                NeiborCube->setHidden(false);
                NeiborCube->getCellOwner()->setDirtyDrawList(true);
            }

            if(NeiborCube != NULL && NeiborCube->isSolid())
            {
                NeiborCube->InitFacesSolid();
                NeiborCube->getCellOwner()->setDirtyDrawList(true);
            }
        }
    }
}
