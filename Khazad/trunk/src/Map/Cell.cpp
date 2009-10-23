#include <stdafx.h>

#include <Cell.h>
#include <Cube.h>
#include <Map.h>
#include <DFTypes.h>
#include <Building.h>
#include <Tree.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <Renderer.h>
#include <ModelManager.h>

Cell::Cell()
{
    Active = false;
    ActiveLiquid = false;
    Initialized = false;

    for(Uint8 i = 0; i < CELLEDGESIZE; i++)
    {
        for(Uint8 j = 0; j < CELLEDGESIZE; j++)
        {
            CubeShapeTypes[i][j] = -1;
            CubeMaterialTypes[i][j] = -1;
            CubeSurfaceTypes[i][j] = -1;
            LiquidLevel[i][j] = 0;
        }
    }

    Hidden.reset();
    SubTerranean.reset();
    SkyView.reset();
    SunLit.reset();
    Solid.reset();
    LiquidType.reset();

    MAP->ChangeCellCount(1);
}

Cell::~Cell()
{
    MAP->ChangeCellCount(-1);

    if(Initialized)
    {
        MAP->ChangeInitedCellCount(-1);
    }

    ClearROs();
}

bool Cell::Init()
{
    Initialized = true;
    NeedsRedraw = true;

    MAP->ChangeInitedCellCount(1);

    for(Uint32 CubeX = 0; CubeX < CELLEDGESIZE; CubeX++)
    {
        for(Uint32 CubeY = 0; CubeY < CELLEDGESIZE; CubeY++)
        {

        }
    }

    return true;
}

void Cell::setPosition(Sint32 X, Sint32 Y, Sint32 Z)
{
    CellPosition.x = (float) X + (CELLEDGESIZE / 2) - HALFCUBE;
    CellPosition.y = (float) Y + (CELLEDGESIZE / 2) - HALFCUBE;
    CellPosition.z = (float) Z;

    CellX = X / CELLEDGESIZE;
    CellY = Y / CELLEDGESIZE;
    CellZ = Z;
}

bool Cell::Update()
{
    return true;
}

void Cell::Render(CameraOrientation CurrentOrientation)
{
    for( map<int16_t, ROstore >::iterator it = ROs.begin(); it != ROs.end(); ++it)
    {
        TEXTURE->BindTexture(it->first);
        if(it->second.normal)
        {
            RENDERER->CallRenderObject(it->second.normal);
        }
    }
    for(Uint32 i = 0; i< trees.size(); i++)
    {
        trees[i]->Draw();
    }
}

bool Cell::DrawFaces(Uint8 CubeX, Uint8 CubeY)
{
    // cached quads
    static const vertex vertices[6][4] =
    {
        // position, uv texture coords, normal vector - see vertex in Renderer.h
        // FACET_WEST
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f ),
        // FACET_EAST
        vertex( 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f ),

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

        // FACET_BOTTOM
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        // FACET_TOP
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f )
    };

    // work vector ptr
    vector<vertex>* TextureVector;

    for (Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType)
    {
        if(isCubeSolid(CubeX, CubeY))
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
        }

        Face* TargetFace = getFace(CubeX, CubeY, FacetType);
        if (TargetFace != NULL)
        {
            Sint16 CubeMaterialType = getFaceMaterialType(CubeX, CubeY, FacetType);
            if (CubeMaterialType != -1)
            {
                Uint32 Texture = TEXTURE->MapTexture(getFaceMaterialType(CubeX, CubeY, FacetType), getCubeSurface(CubeX, CubeY));

                if (!Geometry.count(Texture))
                {
                    TextureVector = new vector<vertex>;
                    Geometry[Texture] = TextureVector;
                }
                else
                {
                    TextureVector = Geometry[Texture];
                }

                vertex v3 = vertices[FacetType][3];            v3.translate((float) CubeX, (float) CubeY);
                vertex v2 = vertices[FacetType][2];            v2.translate((float) CubeX, (float) CubeY);
                vertex v1 = vertices[FacetType][1];            v1.translate((float) CubeX, (float) CubeY);
                vertex v0 = vertices[FacetType][0];            v0.translate((float) CubeX, (float) CubeY);

                TextureVector->push_back(v3);
                TextureVector->push_back(v1);
                TextureVector->push_back(v0);

                TextureVector->push_back(v3);
                TextureVector->push_back(v2);
                TextureVector->push_back(v1);
            }
        }
    }
}

bool Cell::DrawSlope(Uint8 CubeX, Uint8 CubeY)
{
    float xTranslate = CubeX;
    float yTranslate = CubeY;

    //if(!Visible)
    //{
    //    return false;
    //}

    SlopeIndex surroundings;
    surroundings.value = 0;

    Uint8 solid;
    Sint32 X, Y, Z;

    // copy surroundings
    for(Direction TestDirection = DIRECTION_NORTHWEST; TestDirection <= DIRECTION_WEST; ++TestDirection)
    {
        X = (CellX * CELLEDGESIZE) + CubeX;
        Y = (CellY * CELLEDGESIZE) + CubeY;
        Z = CellZ;

        TranslateCordinates(X, Y, Z, TestDirection);

        solid = 0;
        if(MAP->isCubeSolid(X, Y, Z))
        {
            solid = 2;
        }
        else if (MAP->isCubeSloped(X, Y, Z))
        {
            solid = 1;
        }
        surroundings.value |= solid << (2 * TestDirection); // Bit shift to create slope Index
    }

    Uint32 Texture = TEXTURE->MapTexture(getCubeMaterial(CubeX, CubeY), getCubeSurface(CubeX, CubeY));

    // create output vector if needed
    // FIXME: should be part of cell?
    vector <vertex>* VertexTextureVector;
    if(!Geometry.count(Texture))
    {
        VertexTextureVector = new vector <vertex>;
        Geometry[Texture] = VertexTextureVector;
        VertexTextureVector->reserve(256); // ???
    }
    else
    {
        VertexTextureVector = Geometry[Texture];
    }

    // get slope geometry and mix it in
    vector <vertex>* slopeVetices = RENDERER->ModelMan->getSlope(surroundings);
    MixVertexVectorsOffset(slopeVetices, VertexTextureVector, CubeX, CubeY);
    return true;
}

void Cell::setCubeShape(Uint8 CubeX, Uint8 CubeY, Sint16 TileShape)
{
    CubeShapeTypes[CubeX][CubeY] = TileShape;

    setCubeSolid(CubeX, CubeY, !DATA->getTileShapeData(TileShape)->isOpen());
}

void Cell::BuildFaceData()
{
    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 StairID = DATA->getLabelIndex("TILESHAPE_STAIR");
    static Sint16 EmptyID = DATA->getLabelIndex("TILESHAPE_EMPTY");

    for (Uint16 x = 0; x < CELLEDGESIZE; x++)
    {
        for (Uint16 y = 0; y < CELLEDGESIZE; y++)
        {
            Sint16 CubeShape = getCubeShape(x, y);
            Sint16 CubeMaterial = getCubeMaterial(x, y);

            if(CubeShape == FloorID)
            {
                setFaceMaterialType(x, y, FACET_BOTTOM, CubeMaterial);
            }

            for (Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType)
            {
                if (FacetType == FACET_TOP)
                {
                    continue;
                }

                Sint32 X, Y, Z;

                X = (CellX * CELLEDGESIZE) + x;
                Y = (CellY * CELLEDGESIZE) + y;
                Z = CellZ;

                TranslateCordinates(X, Y, Z, FacetType);

                if (isCubeSolid(x, y))
                {
                    if (!MAP->isCubeSolid(X, Y, Z))
                    {
                        setFaceMaterialType(x, y, FacetType, CubeMaterial);
                    }
                }
                /*
                else
                {
                    if (MAP->isCubeSolid(X, Y, Z))
                    {
                        setFaceMaterialType(x, y, FacetType, CubeMaterial);
                    }
                }
                */
            }
        }
    }
}

void Cell::UpdateRenderLists()
{
    //maps between texture and vertex vectors
    Geometry.clear();

    Cube* LoopCube = NULL;
    if(Initialized)
    {
        for (Uint16 x = 0; x < CELLEDGESIZE; x++)
        {
            for (Uint16 y = 0; y < CELLEDGESIZE; y++)
            {
                if (RENDERER->isCubeDrawn((CellX * CELLEDGESIZE) + x, (CellY * CELLEDGESIZE) + y, CellZ))
                {
                    if(isCubeSloped(x, y))
                    {
                        DrawSlope(x, y);
                    }
                    DrawFaces(x, y);
                }
            }
        }

        //TODO: only regenerate changed ones
        // destroy old
        ClearROs();
        // for each material in normal geometry
        for( map<int16_t, vector <vertex>* >::iterator it = Geometry.begin(); it != Geometry.end(); ++it)
        {
            Sint16 material = it->first;
            vector <vertex>* vertices = it->second;

            // generate VBOs out of vertex arrays
            RenderObject * RO = RENDERER->CreateRenderObject(vertices);
            delete vertices;
            // create descriptor
            ROstore tempRO;
            if(ROs.count(material))
            {
                tempRO = ROs[material];
                tempRO.normal = RO;
            }
            else
            {
                tempRO= ROstore(RO,NULL);
            }
            ROs[material] = tempRO;
        }
    }
}

void Cell::setLiquid(Uint8 CubeX, Uint8 CubeY, bool liquidtype, Uint8 NewValue)
{
    LiquidType.set(((CubeX * CELLEDGESIZE) + CubeY), liquidtype);
    LiquidLevel[CubeX][CubeY] = NewValue;
}

Face* Cell::getFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        if(FacetType == FACET_EAST)
        {
            if (CubeX == CELLEDGESIZE)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->getFace(((CellX + 1) * CELLEDGESIZE), (CellY * CELLEDGESIZE) + CubeY, CellZ, FACET_WEST);
            }
            else
            {
                Uint32 Key = CubeY;
                Key <<= 8;
                Key += CubeX + 1;  // Jump one position to the East along X axis

                if (EastWestFaces.find(Key) == EastWestFaces.end())
                {
                    return NULL;
                }
                else
                {
                    return EastWestFaces.find(Key)->second;
                }
            }
        }
        if(FacetType == FACET_SOUTH)
        {
            if (CubeY == CELLEDGESIZE && FACET_SOUTH)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->getFace((CellX * CELLEDGESIZE) + CubeX, (CellY + 1) * CELLEDGESIZE, CellZ, FACET_NORTH);
            }
            else
            {
                Uint32 Key = CubeX;
                Key <<= 8;
                Key += CubeY + 1;  // Jump one position to the South on the Y axis

                if (NorthSouthFaces.find(Key) == NorthSouthFaces.end())
                {
                    return NULL;
                }
                else
                {
                    return NorthSouthFaces.find(Key)->second;
                }
            }
        }
        if(FacetType == FACET_TOP)  // All top faces are stored in the cell above
        {
            return MAP->getFace((CellX * CELLEDGESIZE) + CubeX, (CellY * CELLEDGESIZE) + CubeY, CellZ + 1, FACET_BOTTOM);
        }
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        if(FacetType == FACET_WEST)
        {
            Uint32 Key = CubeY;
            Key <<= 8;
            Key += CubeX;

            if (EastWestFaces.find(Key) == EastWestFaces.end())
            {
                return NULL;
            }
            else
            {
                return EastWestFaces.find(Key)->second;
            }
        }
        if(FacetType == FACET_NORTH)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            if (NorthSouthFaces.find(Key) == NorthSouthFaces.end())
            {
                return NULL;
            }
            else
            {
                return NorthSouthFaces.find(Key)->second;
            }
        }
        if(FacetType == FACET_BOTTOM)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            if (BottomFaces.find(Key) == BottomFaces.end())
            {
                return NULL;
            }
            else
            {
                return BottomFaces.find(Key)->second;
            }
        }
    }
}

bool Cell::hasFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        if (FacetType == FACET_EAST)
        {
            if (CubeX == CELLEDGESIZE)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->hasFace(((CellX + 1) * CELLEDGESIZE), (CellY * CELLEDGESIZE) + CubeY, CellZ, FACET_WEST);
            }
            else
            {
                Uint32 Key = CubeY;
                Key <<= 8;
                Key += CubeX + 1;  // Jump one position to the East along X axis

                return EastWestFaces.find(Key) != EastWestFaces.end();
            }
        }
        if (FacetType == FACET_SOUTH)
        {
            if (CubeY == CELLEDGESIZE && FACET_SOUTH)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->hasFace((CellX * CELLEDGESIZE) + CubeX, (CellY + 1) * CELLEDGESIZE, CellZ, FACET_NORTH);
            }
            else
            {
                Uint32 Key = CubeX;
                Key <<= 8;
                Key += CubeY + 1;  // Jump one position to the South on the Y axis

                return NorthSouthFaces.find(Key) != NorthSouthFaces.end();
            }
        }
        if (FacetType == FACET_TOP)  // All top faces are stored in the cell above
        {
            return MAP->hasFace((CellX * CELLEDGESIZE) + CubeX, (CellY * CELLEDGESIZE) + CubeY, CellZ + 1, FACET_BOTTOM);
        }
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        if (FacetType == FACET_WEST)
        {
            Uint32 Key = CubeY;
            Key <<= 8;
            Key += CubeX;

            return EastWestFaces.find(Key) != EastWestFaces.end();
        }
        if (FacetType == FACET_NORTH)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            return NorthSouthFaces.find(Key) != NorthSouthFaces.end();
        }
        if (FacetType == FACET_BOTTOM)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            return BottomFaces.find(Key) != BottomFaces.end();
        }
    }
}

Sint16 Cell::getFaceMaterialType(Uint8 CubeX, Uint8 CubeY, Facet FacetType)
{
    Face* TargetFace = getFace(CubeX, CubeY, FacetType);

    if (TargetFace != NULL)
    {
        return TargetFace->MaterialTypeID;
    }
}

Sint16 Cell::getFaceSurfaceType(Uint8 CubeX, Uint8 CubeY, Facet FacetType)
{
    Face* TargetFace = getFace(CubeX, CubeY, FacetType);

    if (TargetFace != NULL)
    {
        if (FacetType & 1)
        {
            return TargetFace->PositiveAxisSurfaceTypeID;
        }
        else
        {
            return TargetFace->NegativeAxisSurfaceTypeID;
        }
    }
}

bool Cell::setFaceMaterialType(Uint8 CubeX, Uint8 CubeY, Facet FacetType, Sint16 MaterialTypeID)
{
    Face* TargetFace = getFace(CubeX, CubeY, FacetType);

    if (TargetFace == NULL)
    {
        TargetFace = addFace(CubeX, CubeY, FacetType);
        if (TargetFace != NULL)
        {
            TargetFace->MaterialTypeID = MaterialTypeID;
            return true;
        }
    }
    return false;
}

bool Cell::setFaceSurfaceType(Uint8 CubeX, Uint8 CubeY, Facet FacetType, Sint16 SurfaceTypeID)
{
    Face* TargetFace = getFace(CubeX, CubeY, FacetType);

    if (TargetFace != NULL)
    {
        TargetFace = addFace(CubeX, CubeY, FacetType);
        if (TargetFace != NULL)
        {
            if (FacetType & 1)
            {
                TargetFace->PositiveAxisSurfaceTypeID = SurfaceTypeID;
                return true;
            }
            else
            {
                TargetFace->NegativeAxisSurfaceTypeID = SurfaceTypeID;
                return true;
            }
        }
    }
    return false;
}

bool Cell::removeFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        if (FacetType == FACET_EAST)
        {
            if (CubeX == CELLEDGESIZE)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->removeFace(((CellX + 1) * CELLEDGESIZE), (CellY * CELLEDGESIZE) + CubeY, CellZ, FACET_WEST);
            }
            else
            {
                Uint32 Key = CubeY;
                Key <<= 8;
                Key += CubeX + 1;  // Jump one position to the East along X axis

                if (EastWestFaces.find(Key) != EastWestFaces.end())
                {
                    delete EastWestFaces.find(Key)->second;
                    EastWestFaces.erase(Key);
                    return true;
                }
            }
        }
        if (FacetType == FACET_SOUTH)
        {
            if (CubeY == CELLEDGESIZE && FACET_SOUTH)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->removeFace((CellX * CELLEDGESIZE) + CubeX, (CellY + 1) * CELLEDGESIZE, CellZ, FACET_NORTH);
            }
            else
            {
                Uint32 Key = CubeX;
                Key <<= 8;
                Key += CubeY + 1;  // Jump one position to the South on the Y axis

                if (NorthSouthFaces.find(Key) == NorthSouthFaces.end())
                {
                    delete NorthSouthFaces.find(Key)->second;
                    NorthSouthFaces.erase(Key);
                    return true;
                }
            }
        }
        if (FacetType == FACET_TOP)  // All top faces are stored in the cell above
        {
            return MAP->removeFace((CellX * CELLEDGESIZE) + CubeX, (CellY * CELLEDGESIZE) + CubeY, CellZ + 1, FACET_BOTTOM);
        }
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        if(FacetType == FACET_WEST)
        {
            Uint32 Key = CubeY;
            Key <<= 8;
            Key += CubeX;

            if (EastWestFaces.find(Key) == EastWestFaces.end())
            {
                delete EastWestFaces.find(Key)->second;
                EastWestFaces.erase(Key);
                return true;
            }
        }
        if(FacetType == FACET_NORTH)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            if (NorthSouthFaces.find(Key) == NorthSouthFaces.end())
            {
                delete NorthSouthFaces.find(Key)->second;
                NorthSouthFaces.erase(Key);
                return true;
            }
        }
        if(FacetType == FACET_BOTTOM)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            if (BottomFaces.find(Key) == BottomFaces.end())
            {
                delete BottomFaces.find(Key)->second;
                BottomFaces.erase(Key);
                return true;
            }
        }
    }
    return false;
}

Face* Cell::addFace(Uint8 CubeX, Uint8 CubeY, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        if (FacetType == FACET_EAST)
        {
            if (CubeX == CELLEDGESIZE)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->addFace(((CellX + 1) * CELLEDGESIZE), (CellY * CELLEDGESIZE) + CubeY, CellZ, FACET_WEST);
            }
            else
            {
                Uint32 Key = CubeY;
                Key <<= 8;
                Key += CubeX + 1;  // Jump one position to the East along X axis

                if (EastWestFaces.find(Key) == EastWestFaces.end())
                {
                    Face* NewFace = new Face();
                    EastWestFaces[Key] = NewFace;
                    setActive(true);
                    return NewFace;
                }
            }
        }
        if (FacetType == FACET_SOUTH)
        {
            if (CubeY == CELLEDGESIZE && FACET_SOUTH)  // Jump to adjacent Cells for edge Faces
            {
                return MAP->addFace((CellX * CELLEDGESIZE) + CubeX, (CellY + 1) * CELLEDGESIZE, CellZ, FACET_NORTH);
            }
            else
            {
                Uint32 Key = CubeX;
                Key <<= 8;
                Key += CubeY + 1;  // Jump one position to the South on the Y axis

                if (NorthSouthFaces.find(Key) == NorthSouthFaces.end())
                {
                    Face* NewFace = new Face();
                    NorthSouthFaces[Key] = NewFace;
                    setActive(true);
                    return NewFace;
                }
            }
        }
        if (FacetType == FACET_TOP)  // All top faces are stored in the cell above
        {
            return MAP->addFace((CellX * CELLEDGESIZE) + CubeX, (CellY * CELLEDGESIZE) + CubeY, CellZ + 1, FACET_BOTTOM);
        }
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        if(FacetType == FACET_WEST)
        {
            Uint32 Key = CubeY;
            Key <<= 8;
            Key += CubeX;

            if (EastWestFaces.find(Key) == EastWestFaces.end())
            {
                Face* NewFace = new Face();
                EastWestFaces[Key] = NewFace;
                setActive(true);
                return NewFace;
            }
        }
        if(FacetType == FACET_NORTH)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            if (NorthSouthFaces.find(Key) == NorthSouthFaces.end())
            {
                Face* NewFace = new Face();
                NorthSouthFaces[Key] = NewFace;
                setActive(true);
                return NewFace;
            }
        }
        if(FacetType == FACET_BOTTOM)
        {
            Uint32 Key = CubeX;
            Key <<= 8;
            Key += CubeY;

            if (BottomFaces.find(Key) == BottomFaces.end())
            {
                Face* NewFace = new Face();
                BottomFaces[Key] = NewFace;
                setActive(true);
                return NewFace;
            }
        }
    }
    return NULL;
}

/*
Vector3 Cube::getAdjacentCubePosition(Facet FacetType)
{
    Sint32 x = Position.x;
    Sint32 y = Position.y;
    Sint32 z = Position.z;

    switch(FacetType)
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
    return Vector3(x, y, z);
}
*/

Cell* Cell::getAdjacentCell(Facet FacetType)
{
    Sint32 x = CellX;
    Sint32 y = CellY;
    Sint32 z = CellZ;

    switch(FacetType)
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

    return MAP->getCell(x, y, z);
}

Vector3 Cell::getCubePosition(Uint8 CubeX, Uint8 CubeY)
{
    float X = CellPosition.x - (float)(CELLEDGESIZE / 2) + (float)CubeX + (float)HALFCUBE;
    float Y = CellPosition.y - (float)(CELLEDGESIZE / 2) + (float)CubeY + (float)HALFCUBE;

	return Vector3(X, Y, CellPosition.z);
}

bool Cell::isCubeSloped(Uint8 CubeX, Uint8 CubeY) //TODO move data int Shape definitions
{
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 StairID = DATA->getLabelIndex("TILESHAPE_STAIR");

    Sint16 CubeShapeID = getCubeShape(CubeX, CubeY);
    return (CubeShapeID == RampID || CubeShapeID == StairID);
}

void Cell::ClearROs()
{
    for( map<int16_t, ROstore >::iterator it = ROs.begin(); it != ROs.end(); ++it)
    {
        ROstore tmp = it->second;
        if(tmp.normal)
        {
            RENDERER->DeleteRenderObject( tmp.normal );
        }
    }
    ROs.clear();
}

void Cell::DrawCellCage()
{
    Vector3 AdjustedPoint = CellPosition;

    AdjustedPoint.x -= (CELLEDGESIZE / 2) - HALFCUBE;
    AdjustedPoint.y -= (CELLEDGESIZE / 2) - HALFCUBE;

    RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true, 1, 1, 1);
}
