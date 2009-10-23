#include <stdafx.h>

#include <Cube.h>
#include <Cell.h>
#include <Map.h>
#include <Random.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <ModelManager.h>


/*
void Cube::DigChannel()
{
    setHidden(false);
    setShape(DATA->getLabelIndex("TILESHAPE_EMPTY"));
    // reveal tiles around, deig below
    for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
    {
        Cube* NeighborCube = getNeighborCube(DirectionType);

        if(NeighborCube != NULL)
        {
            if(DirectionType != DIRECTION_DOWN && DirectionType != DIRECTION_UP)
            {
                NeighborCube->setHidden(false);
                NeighborCube->getCellOwner()->setNeedsRedraw(true);
            }
            else if(DirectionType == DIRECTION_DOWN)
            {
                NeighborCube->Dig();
                NeighborCube->getCellOwner()->setNeedsRedraw(true);
            }
        }
    }
}

void Cube::DigSlope()
{
    if(isSolid())
    {
        setHidden(false);
        // set to floor

        // reveal tiles around
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            Cube* NeighborCube = getNeighborCube(DirectionType);
            if(DirectionType != DIRECTION_DOWN && DirectionType != DIRECTION_UP)
            {

                if(NeighborCube != NULL)
                {
                    NeighborCube->setHidden(false);
                    NeighborCube->getCellOwner()->setNeedsRedraw(true);
                }
            }
            if(DirectionType == DIRECTION_UP)
            {
                NeighborCube->DigChannel();
                NeighborCube->getCellOwner()->setNeedsRedraw(true);
            }
        }
        setShape(DATA->getLabelIndex("TILESHAPE_RAMP"));
        // update draw list of parent cell(s)
        getCellOwner()->setNeedsRedraw(true);
    }
    else if(getShape()!=DATA->getLabelIndex("TILESHAPE_EMPTY"))
    {
        Cube* NeighborCube = getNeighborCube(DIRECTION_DOWN);
        if(NeighborCube)
        {
            NeighborCube->DigSlope();
        }
    }
}

void Cube::Dig()
{
    if(isSolid() || isSlope())
    {
        setHidden(false);
        // set to floor
        setShape(DATA->getLabelIndex("TILESHAPE_FLOOR"));
        // reveal tiles around
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            if(DirectionType != DIRECTION_DOWN && DirectionType != DIRECTION_UP)
            {
                Cube* NeighborCube = getNeighborCube(DirectionType);
                if(NeighborCube != NULL)
                {
                    NeighborCube->setHidden(false);
                    NeighborCube->getCellOwner()->setNeedsRedraw(true);
                }
            }
        }
        // update draw list of parent cell(s)
        getCellOwner()->setNeedsRedraw(true);
    }
}



void Cube::setFacetMaterialType(Facet FacetType, Sint16 MaterialType)
{
    if (FacetType & 1)
    {
       FacetMaterialTypes[FacetType / 2] = MaterialType;

        // Check to tag cube as having visable faces for rendering
       if (FacetMaterialTypes[FacetType / 2] != -1)
       {
            Visible = true;
            Owner->setActive(true);
       }
    }
    else
    {
        Cube* AdjacentCube = getAdjacentCube(FacetType);
        if (AdjacentCube != NULL)
        {
            AdjacentCube->setFacetMaterialType(OppositeFacet(FacetType), MaterialType);

            // Check to tag cube as having visable faces for rendering
            if (AdjacentCube->getFacetMaterialType(OppositeFacet(FacetType)) != -1)
            {
                Visible = true;
                Owner->setActive(true);
            }
        }
    }
}

Sint16 Cube::getFacetMaterialType(Facet FacetType)
{
    if(FacetType % 2)
    {
        return FacetMaterialTypes[FacetType / 2];
    }
    else
    {
        Cube* AdjacentCube = getAdjacentCube(FacetType);
        if(AdjacentCube != NULL)
        {
            return AdjacentCube->getFacetMaterialType(OppositeFacet(FacetType));
        }
        return -1;
    }
}

void Cube::setShape(Sint16 TileShape)
{
    if(TileShape < 0 || TileShape >= DATA->getNumTileShape())
    {
        //CubeShapeType = DATA->getLabelIndex("TILESHAPE_EMPTY");
        //data.solid = !DATA->getTileShapeData(CubeShapeType)->isOpen();
        cerr << "can't set shape to -1" << endl;
        return;
    }

    if(CubeShapeType != TileShape)
    {
        CubeShapeType = TileShape;
        setSolid(!DATA->getTileShapeData(CubeShapeType)->isOpen());
        Owner->setNeedsRedraw(true);
    }

    if(Initalized)
    {
        RefreshFacetData();
    }
}

void Cube::RefreshFacetData()
{
    for (Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType)
    {
        if(FacetType == FACET_TOP)
        {
            continue;
        }

        if (isSolid())
        {
            if (getAdjacentCube(FacetType) != NULL)
            {
                if (!getAdjacentCube(FacetType)->isSolid())
                {
                    setFacetMaterialType(FacetType, CubeMaterialType);
                }
            }
            else
            {
                Vector3 NewCubePosition = getAdjacentCubePosition(FacetType);
                if(MAP->GenerateCube(NewCubePosition.x, NewCubePosition.y, NewCubePosition.z) != NULL)
                {
                    setFacetMaterialType(FacetType, CubeMaterialType);
                }
            }
        }
        else
        {
            if (getAdjacentCube(FacetType) != NULL)
            {
                if (getAdjacentCube(FacetType)->isSolid())
                {
                    setFacetMaterialType(FacetType, getAdjacentCube(FacetType)->getMaterial());
                    Owner->setActive(true);
                }
                else
                {
                    setFacetMaterialType(FacetType, -1);
                }
            }
            else
            {
                Vector3 NewCubePosition = getAdjacentCubePosition(FacetType);
                MAP->GenerateCube(NewCubePosition.x, NewCubePosition.y, NewCubePosition.z);
                setFacetMaterialType(FacetType, -1);
            }
        }
    }

    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 StairID = DATA->getLabelIndex("TILESHAPE_STAIR");
    static Sint16 EmptyID = DATA->getLabelIndex("TILESHAPE_EMPTY");

    if(CubeShapeType == FloorID)
    {
        setFacetMaterialType(FACET_BOTTOM, CubeMaterialType);
        Owner->setActive(true);
    }

    if(CubeShapeType == RampID || CubeShapeType == StairID)
    {
        setFacetMaterialType(FACET_BOTTOM, -1);
        Owner->setActive(true);
    }

    if(CubeShapeType == EmptyID)
    {
        if (getAdjacentCube(FACET_BOTTOM) != NULL)
        {
            if (getAdjacentCube(FACET_BOTTOM)->isSolid())
            {
                getAdjacentCube(FACET_BOTTOM)->setShape(FloorID);
            }
            setFacetMaterialType(FACET_BOTTOM, -1);
        }
    }
}


//TODO: pre-generating all possible configs and using them as templates would be faster. deferred
bool Cube::DrawFaces(float xTranslate, float yTranslate)
{
    // cached quads
    static const vertex vertices[6][4] =
    {
        // position, uv texture coords, normal vector - see vertex in Renderer.h
        // FACET_TOP
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        // FACET_BOTTOM
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),

        // FACET_NORTH
        vertex( 0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f,-1.0f, 0.0f ),
        vertex(-0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f,-1.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  0.0f,-1.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  0.0f, 0.0f,  0.0f,-1.0f, 0.0f ),
        // FACET_SOUTH
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f ),

        // FACET_EAST
        vertex( 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f ),
        // FACET_WEST
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f )
    };

    // work vector ptr
    vector<vertex>* vec;

    for (Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType)
    {
        if(isSolid())
        {
            if(FacetType == FACET_BOTTOM)
            {
                continue;
            }
            if(FacetType == FACET_TOP)
            {
                continue;
            }
        }
        else
        {
            if(FacetType == FACET_TOP)
            {
                continue;
            }
            // don't draw wall faces for anything but walls...
            /*
            if(FacetType != FACET_BOTTOM)
            {
                continue;
            }
            */
        }

        Sint16 CubeMaterialType = getFacetMaterialType(FacetType);

        if (CubeMaterialType != -1)
        {
            Uint32 Texture = TEXTURE->MapTexture(getFacetMaterialType(FacetType), getCubeSurfaceType());

            vector<vertex>* vec;

            if (!Owner->Geometry.count(Texture))
            {
                vec = new vector<vertex>;
                Owner->Geometry[Texture] = vec;
            }
            else
            {
                vec = Owner->Geometry[Texture];
            }

            vertex v3 = vertices[FacetType][3];
            v3.translate(xTranslate, yTranslate);
            vertex v2 = vertices[FacetType][2];
            v2.translate(xTranslate, yTranslate);
            vertex v1 = vertices[FacetType][1];
            v1.translate(xTranslate, yTranslate);
            vertex v0 = vertices[FacetType][0];
            v0.translate(xTranslate, yTranslate);

            vec->push_back(v3);
            vec->push_back(v1);
            vec->push_back(v0);

            vec->push_back(v3);
            vec->push_back(v2);
            vec->push_back(v1);
        }
    }
}

bool Cube::DrawSlope(float xTranslate, float yTranslate)
{
    if(!Visible)
    {
        return false;
    }
    SlopeIndex surroundings;
    surroundings.value = 0;

    uint8_t solid;
    // copy surroundings
    for(Direction i = DIRECTION_NORTHWEST; i <= DIRECTION_WEST; ++i)
    {
        solid = 0;
        Cube *neighbor = getNeighborCube(i);
        if( neighbor != NULL)
        {
            if(neighbor->isSolid())
            {
                solid = 2;
            }
            else if (neighbor->isSlope())
            {
                solid = 1;
            }
        }
        surroundings.value |= solid << (2 * i);
    }

    Uint32 Texture = TEXTURE->MapTexture(CubeMaterialType, getCubeSurfaceType());


    // create output vector if needed
    // FIXME: should be part of cell?
    vector <vertex>* vec;
    if(!Owner->Geometry.count(Texture))
    {
        vec = new vector< vertex >;
        Owner->Geometry[Texture] = vec;
        vec->reserve(256);
    }
    else
    {
        vec = Owner->Geometry[Texture];
    }
    // get slope geometry and mix it in
    vector <vertex> * slope = RENDERER->ModelMan->getSlope(surroundings);
    MixVertexVectorsOffset(slope, vec, xTranslate, yTranslate);
    return true;
}

*/
