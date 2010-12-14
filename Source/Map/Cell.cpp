#include <Cell.h>

#include <Map.h>
#include <Face.h>
#include <Actor.h>
#include <DataManager.h>
#include <Renderer.h>
#include <Geology.h>
#include <TextureManager.h>


Cell::Cell()
{
    Active = false;
    ActiveLiquid = false;
    Initialized = false;

    for(uint8_t i = 0; i < CELLEDGESIZE; i++)
    {
        for(uint8_t j = 0; j < CELLEDGESIZE; j++)
        {
            CubeShapeTypes[i][j] = NUM_TILESHAPES;
            CubeMaterialTypes[i][j] = INVALID_INDEX;
            CubeSurfaceTypes[i][j] = INVALID_INDEX;
            LiquidLevel[i][j] = 0;
        }
    }

    Hidden.reset();
    SubTerranean.reset();
    SkyView.reset();
    SunLit.reset();
    Solid.reset();
    LiquidType.reset();

    CellSceneNode = RENDERER->getRootNode()->createChildSceneNode();
}

Cell::~Cell()
{
    //ParentMap->ChangeCellCount(-1);

}

bool Cell::InitializeCell(Map* Parent)
{
    Initialized = true;
    NeedsRedraw = true;

    ParentMap = Parent;
    BuildFaceData();

    return true;
}

void Cell::setCellPosition(CellCoordinates Coordinates)
{
    float x = (float) (Coordinates.X * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    float y = (float) (Coordinates.Y * CELLEDGESIZE) + (CELLEDGESIZE / 2) - HALFCUBE;
    float z = (float) Coordinates.Z;

    CellSceneNode->setPosition(x, y, z);
    thisCellCoordinates = Coordinates;
}

void Cell::LoadCellData(Geology* MapGeology)
{
    CubeCoordinates TargetCubeCoordinates = CubeCoordinates(0, 0);
    static int16_t RoughWallID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");


    for (TargetCubeCoordinates.X = 0; TargetCubeCoordinates.X < CELLEDGESIZE; TargetCubeCoordinates.X += 1)
    {
        for (TargetCubeCoordinates.Y = 0; TargetCubeCoordinates.Y < CELLEDGESIZE; TargetCubeCoordinates.Y += 1)
        {
            TileShape Shape = MapGeology->getTileShapeAtCoordinates(TargetCubeCoordinates, thisCellCoordinates.Z);

            if (Shape != TILESHAPE_EMPTY)
            {
                int16_t MaterialType = MapGeology->getRockTypeAtCoordinates(TargetCubeCoordinates, thisCellCoordinates.Z);

                setCubeMaterial(TargetCubeCoordinates, MaterialType);
                if (MaterialType != INVALID_INDEX)
                {
                    setCubeShape(TargetCubeCoordinates, Shape);
                    setCubeSurface(TargetCubeCoordinates, RoughWallID);
                }
            }
            else
            {
                setCubeShape(TargetCubeCoordinates, Shape);
                setCubeMaterial(TargetCubeCoordinates, INVALID_INDEX);
            }
        }
    }
}

bool Cell::Update()
{
    return true;
}

/*
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
    for(uint32_t i = 0; i< trees.size(); i++)
    {
        //trees[i]->Draw(CurrentOrientation);
    }

    for(uint32_t i = 0; i< buildings.size(); i++)
    {
        //buildings[i]->Draw(CurrentOrientation);
    }

    for(uint32_t i = 0; i < LocalActors.size(); i++)
    {
        LocalActors[i]->Draw(CurrentOrientation);
    }


    // HACK!!! get these into the RenderObject
    CubeCoordinates TargetCubeCoordinates;

    for (TargetCubeCoordinates.X = 0; TargetCubeCoordinates.X < CELLEDGESIZE; TargetCubeCoordinates.X += 1)
    {
        for (TargetCubeCoordinates.Y = 0; TargetCubeCoordinates.Y < CELLEDGESIZE; TargetCubeCoordinates.Y += 1)
        {
            if (isCubeDrawn(TargetCubeCoordinates))
            {
                int16_t ModelID = DATA->getTileShapeData(getCubeShape(TargetCubeCoordinates))->getModelID();
                if (ModelID != INVALID_INDEX)
                {
                    Model* model = MODEL->getModel(ModelID);
                    float Scale = DATA->getModelData(ModelID)->getScalar();

                    if (model != NULL)
                    {
                        glPushMatrix();

                            glTranslatef(TargetCubeCoordinates.X, TargetCubeCoordinates.Y, -HALFCUBE);
                            glScalef(Scale, Scale, Scale);

                            int16_t MaterialID = getCubeMaterial(TargetCubeCoordinates);
                            TEXTURE->BindTexture(TEXTURE->MapTexture(MaterialID,  TEXTURE->PickImageTexture(MaterialID, getCubeSurface(TargetCubeCoordinates))));
                            model->Render();


                        glPopMatrix();
                    }
                }
            }
        }
    }
}


bool Cell::DrawFaces(CubeCoordinates Coordinates)
{
    // cached quads
    static const vertex vertices[6][4] =
    {
        // position, uv texture coords, normal vector - see vertex in Renderer.h
        // DIRECTION_BOTTOM
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        // DIRECTION_TOP
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f ),

        // DIRECTION_NORTH
        vertex( 0.5f,-0.5f, 0.5f,  0.0f, 1.0f,  0.0f,-1.0f, 0.0f ),
        vertex(-0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  0.0f,-1.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  0.0f,-1.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  0.0f, 0.0f,  0.0f,-1.0f, 0.0f ),
        // DIRECTION_SOUTH
        vertex(-0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f ),
        vertex( 0.5f, 0.5f, 0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f ),

        // Direction_WEST
        vertex(-0.5f,-0.5f, 0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f, 0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f, 0.5f,-0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f ),
        vertex(-0.5f,-0.5f,-0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f ),
        // DIRECTION_EAST
        vertex( 0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f, 0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f,-0.5f,-0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f ),
        vertex( 0.5f, 0.5f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f )
    };

    // work vector ptr
    vector<vertex>* TextureVector;

    for (Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType, ++DirectionType)
    {
        if(isCubeSolid(Coordinates))
        {
            if(DirectionType == DIRECTION_DOWN)
            {
                continue;
            }
        }

        if (DirectionType == DIRECTION_DOWN && isCubeSloped(Coordinates))
        {
            continue; // No bottom faces underneath slopes
        }

        MapCoordinates AdjacentCubeCoordinates = TranslateCubeToMap(Coordinates);
        AdjacentCubeCoordinates.TranslateMapCoordinates(DirectionType);

        if (DirectionType != DIRECTION_DOWN && !RENDERER->isCubeDrawn(AdjacentCubeCoordinates))
        {
            continue;
        }

        Face* TargetFace = getFace(Coordinates, DirectionType);
        if (TargetFace != NULL)
        {
            int16_t FaceMaterial = getFaceMaterialType(Coordinates, DirectionType);
            int16_t FaceSurface = getFaceSurfaceType(Coordinates, DirectionType);
            uint32_t Texture = TEXTURE->MapTexture(FaceMaterial, TEXTURE->PickImageTexture(FaceMaterial, FaceSurface));

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

                vertex v3 = vertices[DirectionType][3];            v3.translate((float) Coordinates.X, (float) Coordinates.Y);
                vertex v2 = vertices[DirectionType][2];            v2.translate((float) Coordinates.X, (float) Coordinates.Y);
                vertex v1 = vertices[DirectionType][1];            v1.translate((float) Coordinates.X, (float) Coordinates.Y);
                vertex v0 = vertices[DirectionType][0];            v0.translate((float) Coordinates.X, (float) Coordinates.Y);

                TextureVector->push_back(v3);
                TextureVector->push_back(v1);
                TextureVector->push_back(v0);

                TextureVector->push_back(v3);
                TextureVector->push_back(v2);
                TextureVector->push_back(v1);
            }
        }
    }

    if (getLiquidLevel(Coordinates) != 0)
    {
        int16_t LiquidMaterial;
        int16_t LiquidSurface = DATA->getLabelIndex("SURFACETYPE_LIQUID");

        if (getLiquidType(Coordinates))
        {
            LiquidMaterial = DATA->getLabelIndex("MATERIAL_LAVA");
        }
        else
        {
            LiquidMaterial = DATA->getLabelIndex("MATERIAL_WATER");
        }
        uint32_t Texture = TEXTURE->MapTexture(LiquidMaterial,  TEXTURE->PickImageTexture(LiquidMaterial, LiquidSurface));

        if (!Geometry.count(Texture))
        {
            TextureVector = new vector<vertex>;
            Geometry[Texture] = TextureVector;
        }
        else
        {
            TextureVector = Geometry[Texture];
        }

        vertex v3 = vertices[DIRECTION_UP][3];            v3.translate((float) Coordinates.X, (float) Coordinates.Y);
        vertex v2 = vertices[DIRECTION_UP][2];            v2.translate((float) Coordinates.X, (float) Coordinates.Y);
        vertex v1 = vertices[DIRECTION_UP][1];            v1.translate((float) Coordinates.X, (float) Coordinates.Y);
        vertex v0 = vertices[DIRECTION_UP][0];            v0.translate((float) Coordinates.X, (float) Coordinates.Y);

        TextureVector->push_back(v3);
        TextureVector->push_back(v1);
        TextureVector->push_back(v0);

        TextureVector->push_back(v3);
        TextureVector->push_back(v2);
        TextureVector->push_back(v1);
    }
}


bool Cell::DrawSlope(CubeCoordinates Coordinates)
{
    SlopeIndex surroundings;
    surroundings.value = 0;

    uint8_t solid;
    MapCoordinates UnModifiedCoordinates = TranslateCubeToMap(Coordinates);

    // copy surroundings
    for(Direction TestDirection = COMPASS_DIRECTIONS_START; TestDirection < NUM_COMPASS_DIRECTIONS; ++TestDirection)
    {
        // HACK to convert to old direction system used to index the slope model
        int DirectionNumber;

        switch(TestDirection)
        {
            case DIRECTION_NORTH:
            	 DirectionNumber = 1;
            	 break;
            case DIRECTION_SOUTH:
            	 DirectionNumber = 5;
            	 break;
            case DIRECTION_WEST:
            	 DirectionNumber = 7;
            	 break;
            case DIRECTION_EAST:
            	 DirectionNumber = 3;
            	 break;
            case DIRECTION_NORTHWEST:
            	 DirectionNumber = 0;
            	 break;
            case DIRECTION_SOUTHEAST:
            	 DirectionNumber = 4;
            	 break;
            case DIRECTION_NORTHEAST:
            	 DirectionNumber = 2;
            	 break;
            case DIRECTION_SOUTHWEST:
            	 DirectionNumber = 6;
            	 break;
        }
        // HACK

        MapCoordinates ModifiedCoordinates = UnModifiedCoordinates;
        ModifiedCoordinates.TranslateMapCoordinates(TestDirection);

        solid = 0;
        if(MAP->isCubeSolid(ModifiedCoordinates))
        {
            solid = 2;
        }
        else if (MAP->isCubeSloped(ModifiedCoordinates))
        {
            solid = 1;
        }
        surroundings.value |= solid << (2 * DirectionNumber); // Bit shift to create slope Index
    }

    uint32_t Texture = TEXTURE->MapTexture(getCubeMaterial(Coordinates), TEXTURE->PickImageTexture(getCubeMaterial(Coordinates), getCubeSurface(Coordinates)));

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
*/

void Cell::setCubeShape(CubeCoordinates Coordinates, TileShape NewShape)
{
    if (NewShape != CubeShapeTypes[Coordinates.X][Coordinates.Y])
    {
        CubeShapeTypes[Coordinates.X][Coordinates.Y] = NewShape;
        setCubeSolid(Coordinates, NewShape == TILESHAPE_WALL);
        setNeedsRedraw(true);
    }
}

void Cell::BuildFaceData()
{
    CubeCoordinates TargetCubeCoordinates;
    MapCoordinates TargetMapCoordinates;

    for (TargetCubeCoordinates.X = 0; TargetCubeCoordinates.X < CELLEDGESIZE; TargetCubeCoordinates.X += 1)
    {
        for (TargetCubeCoordinates.Y = 0; TargetCubeCoordinates.Y < CELLEDGESIZE; TargetCubeCoordinates.Y += 1)
        {
            TileShape CubeShape = getCubeShape(TargetCubeCoordinates);
            int16_t CubeMaterial = getCubeMaterial(TargetCubeCoordinates);
            int16_t CubeSurface = getCubeSurface(TargetCubeCoordinates);

            for (Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
            {
                TargetMapCoordinates = TranslateCubeToMap(TargetCubeCoordinates);
                TargetMapCoordinates.TranslateMapCoordinates(DirectionType);

                if (isCubeSolid(TargetCubeCoordinates))
                {
                    if (ParentMap->isCubeInited(TargetMapCoordinates))
                    {
                        if (!ParentMap->isCubeSolid(TargetMapCoordinates))
                        {
                            Face* NewFace = addFace(TargetCubeCoordinates, DirectionType);

                            NewFace->setFaceMaterialType(CubeMaterial);

                            NewFace->setFaceSurfaceType(CubeSurface, DirectionType);
                            //NewFace->setFaceSurfaceType(CubeSurface, DirectionType);
                        }
                    }
                }
            }

            if (getLiquidLevel(TargetCubeCoordinates) != 0)
            {
                setActive(true);
            }

            if (CubeShape > TILESHAPE_EMPTY && CubeShape < TILESHAPE_WALL)
            {
                addSlope(TargetCubeCoordinates, (TileShape) CubeShape);
            }
        }
    }
}

void Cell::BuildStaticGeometry()
{
    char buffer[64];
    sprintf(buffer, "Cell%i-%i-%i",  thisCellCoordinates.X, thisCellCoordinates.Y, thisCellCoordinates.Z);

    Ogre::StaticGeometry* CellGeometry;

    if (!RENDERER->getSceneManager()->hasStaticGeometry(buffer))
    {
        CellGeometry = RENDERER->getSceneManager()->createStaticGeometry(buffer);
        CellGeometry->setCastShadows(false);
    }
    else
    {
        CellGeometry = RENDERER->getSceneManager()->getStaticGeometry(buffer);
        CellGeometry->destroy();
    }

    CellGeometry->addSceneNode(CellSceneNode);

    CellSceneNode->setVisible(false);
    CellGeometry->build();
}

/*
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
                setCubeDrawn(TargetCubeCoordinates, RENDERER->isCubeDrawn(TranslateCubeToMap(TargetCubeCoordinates)));

                if (isCubeDrawn(TargetCubeCoordinates))
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
            int16_t Texture = it->first;
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

    Geometry.clear();
}
*/

void Cell::setLiquid(CubeCoordinates Coordinates, bool liquidtype, uint8_t NewValue)
{
    LiquidType.set(TranslateCubeToIndex(Coordinates), liquidtype);
    LiquidLevel[Coordinates.X][Coordinates.Y] = NewValue;
}

inline uint32_t Cell::GenerateFaceKey(CubeCoordinates Coordinates, Direction DirectionType)
{
    uint32_t Key = Coordinates.Y;
    Key <<= 8;
    Key += Coordinates.X;
    Key <<= 8;
    Key += DirectionType;

    return Key;
}

Face* Cell::getFace(CubeCoordinates TargetCoordinates, Direction DirectionType)
{
    if (isDirectionPositive(DirectionType))  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(TargetCoordinates);
        TargetCoordinates.TranslateMapCoordinates(DirectionType);
        return ParentMap->getFace(TargetCoordinates, OppositeDirection(DirectionType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        uint32_t Key = GenerateFaceKey(TargetCoordinates, DirectionType);

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

bool Cell::hasFace(CubeCoordinates TargetCoordinates, Direction DirectionType)
{
    if (isDirectionPositive(DirectionType))  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(TargetCoordinates);
        TargetCoordinates.TranslateMapCoordinates(DirectionType);
        return ParentMap->hasFace(TargetCoordinates, OppositeDirection(DirectionType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        uint32_t Key = GenerateFaceKey(TargetCoordinates, DirectionType);

        return Faces.find(Key) != Faces.end();
    }
}

int16_t Cell::getFaceMaterialType(CubeCoordinates Coordinates, Direction DirectionType)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        return TargetFace->getFaceMaterialType();
    }
    return INVALID_INDEX;
}

int16_t Cell::getFaceSurfaceType(CubeCoordinates Coordinates, Direction DirectionType)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        if (isDirectionPositive(DirectionType))
        {
            return TargetFace->getPositiveAxisSurfateType();
        }
        else
        {
            return TargetFace->getNegativeAxisSurfaceType();
        }
    }
}

bool Cell::setFaceMaterialType(CubeCoordinates Coordinates, Direction DirectionType, int16_t MaterialTypeID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceMaterialType(MaterialTypeID);
        return true;
    }
    return false;
}

bool Cell::setFaceSurfaceType(CubeCoordinates Coordinates, Direction DirectionType, int16_t SurfaceTypeID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceSurfaceType(SurfaceTypeID, DirectionType);
        return true;
    }
    return false;
}

bool Cell::setBothFaceSurfaces(CubeCoordinates Coordinates, Direction DirectionType, int16_t SurfaceTypeID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceSurfaceType(SurfaceTypeID, DirectionType);
        TargetFace->setFaceSurfaceType(SurfaceTypeID, OppositeDirection(DirectionType));
        return true;
    }
    return false;
}

bool Cell::removeFace(CubeCoordinates TargetCoordinates, Direction DirectionType)
{
    if (isDirectionPositive(DirectionType))  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(TargetCoordinates);
        TargetCoordinates.TranslateMapCoordinates(DirectionType);
        return ParentMap->removeFace(TargetCoordinates, OppositeDirection(DirectionType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        uint32_t Key = GenerateFaceKey(TargetCoordinates, DirectionType);

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

Face* Cell::addFace(CubeCoordinates Coordinates, Direction DirectionType)
{
    if (isDirectionPositive(DirectionType))  // True for East, South and Top some of which will require calls to other Cells
    {
        MapCoordinates TargetCoordinates = TranslateCubeToMap(Coordinates);
        TargetCoordinates.TranslateMapCoordinates(DirectionType);
        return ParentMap->addFace(TargetCoordinates, OppositeDirection(DirectionType));
    }
    else  // All West, North and Bottom Faces will be within this Cell
    {
        uint32_t Key = GenerateFaceKey(Coordinates, DirectionType);

        if (Faces.find(Key) == Faces.end())
        {
            Ogre::SceneNode* NewFaceNode = CellSceneNode->createChildSceneNode();
            NewFaceNode->setPosition(Coordinates.X - (CELLEDGESIZE / 2) + HALFCUBE, Coordinates.Y - (CELLEDGESIZE / 2) + HALFCUBE, 0);
            Face* NewFace = new Face(NewFaceNode, DirectionType);
            Faces[Key] = NewFace;

            return NewFace;
        }
        else
        {
            return Faces.find(Key)->second;
        }
    }
}

void Cell::addSlope(CubeCoordinates Coordinates, TileShape NewShape)
{
    Ogre::SceneNode* NewSlopeNode = CellSceneNode->createChildSceneNode();
    NewSlopeNode->setPosition(Coordinates.X - (CELLEDGESIZE / 2) + HALFCUBE, Coordinates.Y - (CELLEDGESIZE / 2) + HALFCUBE, 0);

    char buffer[64];
    sprintf(buffer, "Slope%i", (uint16_t) NewShape);

    Ogre::Entity* NewSlope = RENDERER->getSceneManager()->createEntity(buffer);
    NewSlope->setMaterial(TEXTURE->getOgreMaterial(getCubeMaterial(Coordinates), getCubeSurface(Coordinates)));

    NewSlopeNode->attachObject(NewSlope);
}

Ogre::Vector3 Cell::getCubePosition(CubeCoordinates Coordinates)
{
    float X = CellSceneNode->getPosition().x - (float)(CELLEDGESIZE / 2) + (float)Coordinates.X + (float)HALFCUBE;
    float Y = CellSceneNode->getPosition().y - (float)(CELLEDGESIZE / 2) + (float)Coordinates.Y + (float)HALFCUBE;

	return Ogre::Vector3(X, Y, CellSceneNode->getPosition().z);
}

MapCoordinates Cell::TranslateCubeToMap(CubeCoordinates Coordinates)
{
    MapCoordinates NewCoordinates;

    NewCoordinates.X = (thisCellCoordinates.X * CELLEDGESIZE) + Coordinates.X;
    NewCoordinates.Y = (thisCellCoordinates.Y * CELLEDGESIZE) + Coordinates.Y;
    NewCoordinates.Z = thisCellCoordinates.Z;

    return NewCoordinates;
}

uint16_t Cell::TranslateCubeToIndex(CubeCoordinates Coordinates)
{
    return (Coordinates.X * CELLEDGESIZE) + Coordinates.Y;
}

bool Cell::isCubeSloped(CubeCoordinates Coordinates) //TODO move data int Shape definitions
{
    static int16_t RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static int16_t StairID = DATA->getLabelIndex("TILESHAPE_STAIR");

    int16_t CubeShapeID = getCubeShape(Coordinates);
    return (CubeShapeID == RampID || CubeShapeID == StairID);
}

void Cell::DrawCellCage()
{
    //RENDERER->DrawCage(AdjustedPoint, CELLEDGESIZE, CELLEDGESIZE, 1.0, true, 1, 1, 1);
}

int Cell::addActor(Actor* NewActor)
{
    LocalActors.push_back(NewActor);
    return LocalActors.size() - 1;
}

void Cell::removeActor(int Index)
{
    if (LocalActors.size() != 0)
    {
        Actor* LastActor = LocalActors.back();

        if (LastActor != NULL)
        {
            LastActor->setCellActorIndex(Index);
        }
        LocalActors[Index] = LastActor;

        LocalActors.pop_back();
    }
}

