#include <stdafx.h>

#include <Cell.h>
#include <Map.h>
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

    BuildFaceData();

    MAP->ChangeInitedCellCount(1);

    return true;
}

void Cell::setPosition(CellCoordinates Coordinates)
{
    CellPosition.x = (float) (Coordinates.X * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    CellPosition.y = (float) (Coordinates.Y * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    CellPosition.z = (float) Coordinates.Z;

    thisCellCoodinates = Coordinates;
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

bool Cell::DrawFaces(CubeCoordinates Coordinates)
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

    for (Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType, ++FacetType)
    {
        if(isCubeSolid(Coordinates))
        {
            if(FacetType == FACET_BOTTOM)
            {
                continue;
            }
        }

        if (FacetType == FACET_BOTTOM && isCubeSloped(Coordinates))
        {
            continue; // No bottom faces underneath slopes
        }

        Face* TargetFace = getFace(Coordinates, FacetType);
        if (TargetFace != NULL)
        {
            Sint16 FaceMaterial = getFaceMaterialType(Coordinates, FacetType);
            Sint16 FaceSurface = getFaceSurfaceType(Coordinates, FacetType);
            Uint32 Texture = TEXTURE->MapTexture(FaceMaterial, FaceSurface);

            {
                if (!Geometry.count(Texture))
                {
                    TextureVector = new vector<vertex>;
                    Geometry[Texture] = TextureVector;
                }
                else
                {
                    TextureVector = Geometry[Texture];
                }

                vertex v3 = vertices[FacetType][3];            v3.translate((float) Coordinates.X, (float) Coordinates.Y);
                vertex v2 = vertices[FacetType][2];            v2.translate((float) Coordinates.X, (float) Coordinates.Y);
                vertex v1 = vertices[FacetType][1];            v1.translate((float) Coordinates.X, (float) Coordinates.Y);
                vertex v0 = vertices[FacetType][0];            v0.translate((float) Coordinates.X, (float) Coordinates.Y);

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

bool Cell::DrawSlope(CubeCoordinates Coordinates)
{
    SlopeIndex surroundings;
    surroundings.value = 0;

    Uint8 solid;
    MapCoordinates UnModifiedCoordinates = TranslateCubeToMap(Coordinates);

    // copy surroundings
    for(Direction TestDirection = DIRECTION_NORTHWEST; TestDirection <= DIRECTION_WEST; ++TestDirection)
    {
        MapCoordinates ModifiedCoordinates = UnModifiedCoordinates;
        TranslateCoordinates(ModifiedCoordinates.X, ModifiedCoordinates.Y, ModifiedCoordinates.Z, TestDirection);

        solid = 0;
        if(MAP->isCubeSolid(ModifiedCoordinates))
        {
            solid = 2;
        }
        else if (MAP->isCubeSloped(ModifiedCoordinates))
        {
            solid = 1;
        }
        surroundings.value |= solid << (2 * TestDirection); // Bit shift to create slope Index
    }

    Uint32 Texture = TEXTURE->MapTexture(getCubeMaterial(Coordinates), getCubeSurface(Coordinates));

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
    vector <vertex>* slopeVetices = MODEL->getSlope(surroundings);
    MixVertexVectorsOffset(slopeVetices, VertexTextureVector, Coordinates.X, Coordinates.Y);

    return true;
}

void Cell::setCubeShape(CubeCoordinates Coordinates, Sint16 TileShape)
{
    if (TileShape != CubeShapeTypes[Coordinates.X][Coordinates.Y])
    {
        CubeShapeTypes[Coordinates.X][Coordinates.Y] = TileShape;
        setCubeSolid(Coordinates, !DATA->getTileShapeData(TileShape)->isOpen());
        setNeedsRedraw(true);
    }
}

void Cell::BuildFaceData()
{
    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 StairID = DATA->getLabelIndex("TILESHAPE_STAIR");
    static Sint16 EmptyID = DATA->getLabelIndex("TILESHAPE_EMPTY");

    CubeCoordinates TargetCubeCoordinates;
    MapCoordinates TargetMapCoordinates;

    for (TargetCubeCoordinates.X = 0; TargetCubeCoordinates.X < CELLEDGESIZE; TargetCubeCoordinates.X += 1)
    {
        for (TargetCubeCoordinates.Y = 0; TargetCubeCoordinates.Y < CELLEDGESIZE; TargetCubeCoordinates.Y += 1)
        {
            Sint16 CubeShape = getCubeShape(TargetCubeCoordinates);
            Sint16 CubeMaterial = getCubeMaterial(TargetCubeCoordinates);
            Sint16 CubeSurface = getCubeSurface(TargetCubeCoordinates);

            for (Facet FacetType = FACETS_START; FacetType < NUM_FACETS; ++FacetType)
            {
                TargetMapCoordinates = TranslateCubeToMap(TargetCubeCoordinates);
                TranslateCoordinates(TargetMapCoordinates.X, TargetMapCoordinates.Y, TargetMapCoordinates.Z, FacetType);

                if (isCubeSolid(TargetCubeCoordinates))
                {
                    if (MAP->isCubeInited(TargetMapCoordinates))
                    {
                        if (!MAP->isCubeSolid(TargetMapCoordinates))
                        {
                            Face* NewFace = addFace(TargetCubeCoordinates, FacetType);

                            NewFace->MaterialTypeID = CubeMaterial;
                            NewFace->PositiveAxisSurfaceTypeID = CubeSurface;
                            NewFace->NegativeAxisSurfaceTypeID = CubeSurface;
                        }
                    }
                }
            }

            if (CubeShape == FloorID)
            {
                Face* NewFace = addFace(TargetCubeCoordinates, FACET_BOTTOM);

                NewFace->MaterialTypeID = CubeMaterial;
                NewFace->PositiveAxisSurfaceTypeID = CubeSurface;
                NewFace->NegativeAxisSurfaceTypeID = CubeSurface;
            }
        }
    }
}

void Cell::UpdateRenderLists(WallDisplayMode Mode)
{
    //maps between texture and vertex vectors
    Geometry.clear();

    if(Initialized)
    {
        CubeCoordinates TargetCubeCoordinates;

        for (TargetCubeCoordinates.X = 0; TargetCubeCoordinates.X < CELLEDGESIZE; TargetCubeCoordinates.X += 1)
        {
            for (TargetCubeCoordinates.Y = 0; TargetCubeCoordinates.Y < CELLEDGESIZE; TargetCubeCoordinates.Y += 1)
            {
                if (RENDERER->isCubeDrawn(TranslateCubeToMap(TargetCubeCoordinates)))
                {
                    if(isCubeSloped(TargetCubeCoordinates))
                    {
                        DrawSlope(TargetCubeCoordinates);
                    }
                    DrawFaces(TargetCubeCoordinates);
                }
            }
        }

        //TODO: only regenerate changed ones
        // destroy old
        ClearROs();
        // for each material in normal geometry
        for( map<int16_t, vector <vertex>* >::iterator it = Geometry.begin(); it != Geometry.end(); ++it)
        {
            Sint16 Texture = it->first;
            vector <vertex>* vertices = it->second;

            // generate VBOs out of vertex arrays
            RenderObject * RO = RENDERER->CreateRenderObject(vertices);
            delete vertices;
            // create descriptor
            ROstore tempRO;
            if(ROs.count(Texture))
            {
                tempRO = ROs[Texture];
                tempRO.normal = RO;
            }
            else
            {
                tempRO= ROstore(RO);
            }
            ROs[Texture] = tempRO;
        }
    }
}

void Cell::setLiquid(CubeCoordinates Coordinates, bool liquidtype, Uint8 NewValue)
{
    LiquidType.set(TranslateCubeToIndex(Coordinates), liquidtype);
    LiquidLevel[Coordinates.X][Coordinates.Y] = NewValue;
}

inline Uint32 Cell::GenerateFaceKey(CubeCoordinates Coordinates, Facet FacetType)
{
    Uint32 Key = Coordinates.Y;
    Key <<= 8;
    Key += Coordinates.X;
    Key <<= 8;
    Key += FacetType;

    return Key;
}

Face* Cell::getFace(CubeCoordinates Coordinates, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(Coordinates);
        TranslateCoordinates(TargetCoordinates.X, TargetCoordinates.Y, TargetCoordinates.Z, FacetType);
        return MAP->getFace(TargetCoordinates, OppositeFacet(FacetType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        Uint32 Key = GenerateFaceKey(Coordinates, FacetType);

        if (Faces.find(Key) == Faces.end())
        {
            return NULL;
        }
        else
        {
            return Faces.find(Key)->second;
        }
    }
}

bool Cell::hasFace(CubeCoordinates Coordinates, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(Coordinates);
        TranslateCoordinates(TargetCoordinates.X, TargetCoordinates.Y, TargetCoordinates.Z, FacetType);
        return MAP->hasFace(TargetCoordinates, OppositeFacet(FacetType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        Uint32 Key = GenerateFaceKey(Coordinates, FacetType);

        return Faces.find(Key) != Faces.end();
    }
}

Sint16 Cell::getFaceMaterialType(CubeCoordinates Coordinates, Facet FacetType)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        return TargetFace->MaterialTypeID;
    }
}

Sint16 Cell::getFaceSurfaceType(CubeCoordinates Coordinates, Facet FacetType)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

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

bool Cell::setFaceMaterialType(CubeCoordinates Coordinates, Facet FacetType, Sint16 MaterialTypeID)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        if (TargetFace->MaterialTypeID != MaterialTypeID)
        {
            TargetFace->MaterialTypeID = MaterialTypeID;
            setNeedsRedraw(true);
            return true;
        }
    }
    return false;
}

bool Cell::setFaceSurfaceType(CubeCoordinates Coordinates, Facet FacetType, Sint16 SurfaceTypeID)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        if (FacetType & 1)
        {
            if (TargetFace->PositiveAxisSurfaceTypeID != SurfaceTypeID)
            {
                TargetFace->PositiveAxisSurfaceTypeID = SurfaceTypeID;
                setNeedsRedraw(true);
                return true;
            }
        }
        else
        {
            if (TargetFace->NegativeAxisSurfaceTypeID != SurfaceTypeID)
            {
                TargetFace->NegativeAxisSurfaceTypeID = SurfaceTypeID;
                setNeedsRedraw(true);
                return true;
            }
        }
    }
    return false;
}

bool Cell::setBothFaceSurfaces(CubeCoordinates Coordinates, Facet FacetType, Sint16 SurfaceTypeID)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        if (TargetFace->PositiveAxisSurfaceTypeID != SurfaceTypeID)
        {
            TargetFace->PositiveAxisSurfaceTypeID = SurfaceTypeID;
            setNeedsRedraw(true);
            return true;
        }
        if (TargetFace->NegativeAxisSurfaceTypeID != SurfaceTypeID)
        {
            TargetFace->NegativeAxisSurfaceTypeID = SurfaceTypeID;
            setNeedsRedraw(true);
            return true;
        }
    }
    return false;
}

bool Cell::removeFace(CubeCoordinates Coordinates, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(Coordinates);
        TranslateCoordinates(TargetCoordinates.X, TargetCoordinates.Y, TargetCoordinates.Z, FacetType);
        return MAP->removeFace(TargetCoordinates, OppositeFacet(FacetType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        Uint32 Key = GenerateFaceKey(Coordinates, FacetType);

        if (Faces.find(Key) != Faces.end())
        {
            delete Faces.find(Key)->second;
            Faces.erase(Key);

            setNeedsRedraw(true);
            return true;
        }
    }
    return false;
}

Face* Cell::addFace(CubeCoordinates Coordinates, Facet FacetType)
{
    if (FacetType & 1)  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(Coordinates);
        TranslateCoordinates(TargetCoordinates.X, TargetCoordinates.Y, TargetCoordinates.Z, FacetType);
        return MAP->addFace(TargetCoordinates, OppositeFacet(FacetType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        Uint32 Key = GenerateFaceKey(Coordinates, FacetType);

        if (Faces.find(Key) == Faces.end())
        {
            Face* NewFace = new Face();
            Faces[Key] = NewFace;

            setActive(true);
            setNeedsRedraw(true);
            return NewFace;
        }
        else
        {
            return Faces.find(Key)->second;
        }
    }
}

Vector3 Cell::getCubePosition(CubeCoordinates Coordinates)
{
    float X = CellPosition.x - (float)(CELLEDGESIZE / 2) + (float)Coordinates.X + (float)HALFCUBE;
    float Y = CellPosition.y - (float)(CELLEDGESIZE / 2) + (float)Coordinates.Y + (float)HALFCUBE;

	return Vector3(X, Y, CellPosition.z);
}

MapCoordinates Cell::TranslateCubeToMap(CubeCoordinates Coordinates)
{
    MapCoordinates NewCoordinates;

    NewCoordinates.X = (thisCellCoodinates.X * CELLEDGESIZE) + Coordinates.X;
    NewCoordinates.Y = (thisCellCoodinates.Y * CELLEDGESIZE) + Coordinates.Y;
    NewCoordinates.Z = thisCellCoodinates.Z;

    return NewCoordinates;
}

Uint16 Cell::TranslateCubeToIndex(CubeCoordinates Coordinates)
{
    return (Coordinates.X * CELLEDGESIZE) + Coordinates.Y;
}

bool Cell::isCubeSloped(CubeCoordinates Coordinates) //TODO move data int Shape definitions
{
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 StairID = DATA->getLabelIndex("TILESHAPE_STAIR");

    Sint16 CubeShapeID = getCubeShape(Coordinates);
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
    //RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true, 1, 1, 1);
}
