#include <Cell.h>

#include <Map.h>
#include <Face.h>
#include <Actor.h>
#include <DataManager.h>
#include <Renderer.h>
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
            CubeShapeTypes[i][j] = TILESHAPE_EMPTY;
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

            if (CubeShape == TILESHAPE_WALL)
            {
                for (Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
                {
                    TargetMapCoordinates = TranslateCubeToMap(TargetCubeCoordinates);
                    TargetMapCoordinates.TranslateMapCoordinates(DirectionType);

                    if (ParentMap->isCubeInited(TargetMapCoordinates))
                    {
                        if (ParentMap->getCubeShape(TargetMapCoordinates) == TILESHAPE_EMPTY)
                        {
                            Face* NewFace = addFace(TargetCubeCoordinates, DirectionType);

                            NewFace->setFaceMaterialType(CubeMaterial);

                            NewFace->setFaceSurfaceType(CubeSurface, DirectionType);
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
        //CellGeometry->setCastShadows(false);
    }
    else
    {
        CellGeometry = RENDERER->getSceneManager()->getStaticGeometry(buffer);
        CellGeometry->destroy();
    }

    CellGeometry->addSceneNode(CellSceneNode);

    CellGeometry->build();

    DestroyAllAttachedEntities(CellSceneNode);
}

void Cell::DestroyAllAttachedEntities(Ogre::SceneNode* TargetNode)
{
   // Destroy all the attached objects
   Ogre::SceneNode::ObjectIterator Nodeit = TargetNode->getAttachedObjectIterator();

   while (Nodeit.hasMoreElements())
   {
      Ogre::MovableObject* Object = static_cast<Ogre::MovableObject*> (Nodeit.getNext());
      TargetNode->getCreator()->destroyMovableObject(Object);
   }

   // Recurse to child SceneNodes
   Ogre::SceneNode::ChildNodeIterator itChild = TargetNode->getChildIterator();

   while (itChild.hasMoreElements())
   {
      Ogre::SceneNode* ChildNode = static_cast<Ogre::SceneNode*> (itChild.getNext());
      DestroyAllAttachedEntities(ChildNode);
   }
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

bool Cell::isCubeSloped(CubeCoordinates Coordinates)
{
    TileShape CubeShape = getCubeShape(Coordinates);
    return (CubeShape > TILESHAPE_EMPTY && CubeShape < TILESHAPE_WALL);
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

