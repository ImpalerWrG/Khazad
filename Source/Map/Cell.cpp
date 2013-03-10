#include <Cell.h>

#include <Game.h>
#include <Map.h>
#include <Face.h>
#include <Actor.h>
#include <DataManager.h>

#include <Renderer.h>
#include <TextureManager.h>
#include <TerrainRender.h>


Cell::Cell()
{
    CubeShape EmptyCube = CubeShape(BELOW_CUBE_HEIGHT);

    CubeShapeTypes.assign(EmptyCube);
    CubeMaterialTypes.assign(INVALID_INDEX);

    Hidden.reset();
    SubTerranean.reset();
    SkyView.reset();
    SunLit.reset();

	Render = NULL;
}

Cell::~Cell()
{

}

void Cell::setCellPosition(CellCoordinates Coordinates)
{
    thisCellCoordinates = Coordinates;
    if (Render == NULL)
	{
		Render = new TerrainRendering(this);
	} else {
		//Render->
	}
}

void Cell::setCubeShape(CubeCoordinates Coordinates, CubeShape NewShape)
{
    if (NewShape != CubeShapeTypes[Coordinates])
    {
        CubeShapeTypes[Coordinates] = NewShape;
        Render->setNeedsReRendering();

        Face* TargetFace = getFace(FaceCoordinates(Coordinates, DIRECTION_NONE));
        if (TargetFace != NULL)
        {
            setFaceShape(FaceCoordinates(Coordinates, DIRECTION_NONE), FaceShape(NewShape, DIRECTION_NONE));
        }
    }
}

void Cell::BuildFaceData()
{
	Map* ParentMap = GAME->getMap();
    MapCoordinates TargetMapCoordinates;
    bool Debug = true;

    CubeCoordinates TargetCube = 0;
    do
    {
        CubeShape Shape = getCubeShape(TargetCube);
        int16_t CubeMaterial = getCubeMaterial(TargetCube);

        static int16_t NEHEMaterial = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
        static int16_t WallSurface = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
        static int16_t FloorSurface = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

        for (uint8_t i = 0, DirectionType = Direction::AXIAL_DIRECTIONS[i]; i < NUM_AXIAL_DIRECTIONS; ++i, DirectionType = Direction::AXIAL_DIRECTIONS[i])
        {
            FaceCoordinates FaceLocation = FaceCoordinates(TargetCube, DirectionType);
            MapCoordinates ModifiedCoordinates = MapCoordinates(thisCellCoordinates, TargetCube);
            ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

            CubeShape AdjacentShape = ParentMap->getCubeShape(ModifiedCoordinates);

            if (!Shape.isSky())
            {
                if (ParentMap->isCubeInited(ModifiedCoordinates) && AdjacentShape.isSky())
                {
                    if (Shape.hasFace(DirectionType))
                    {
                        Face* NewFace = ParentMap->addFace(MapCoordinates(thisCellCoordinates, TargetCube), DirectionType);

                        NewFace->setFaceMaterialType(CubeMaterial);
                        NewFace->setFaceSurfaceType(WallSurface);
                        NewFace->setFaceShapeType(FaceShape(Shape, DirectionType));
                    }
                }

                if (!AdjacentShape.isEmpty())
                {
                    if (DirectionType == DIRECTION_DOWN && Shape.hasFloor())
                    {
                        Face* NewFace = ParentMap->addFace(MapCoordinates(thisCellCoordinates, TargetCube), DirectionType);

                        NewFace->setFaceMaterialType(ParentMap->getCubeMaterial(ModifiedCoordinates));
                        NewFace->setFaceSurfaceType(FloorSurface);
                        NewFace->setFaceShapeType(FaceShape(Shape, DirectionType));
                    }
                }
            }
        }

        if (!Shape.isEmpty() && !Shape.isSolid())
        {
            Face* NewFace = addFace(FaceCoordinates(TargetCube, DIRECTION_NONE));

            NewFace->setFaceMaterialType(CubeMaterial);
            NewFace->setFaceSurfaceType(FloorSurface);
            NewFace->setFaceShapeType(FaceShape(Shape, DIRECTION_NONE));
        }

        TargetCube++;

    }
    while (TargetCube != 0);  // End Loop when Byte rolls over
	Render->setNeedsReRendering();
}

int16_t Cell::getFaceMaterialType(FaceCoordinates TargetCoordinates) const
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceMaterialType() : INVALID_INDEX;
}

int16_t Cell::getFaceSurfaceType(FaceCoordinates TargetCoordinates) const
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceSurfaceType() : INVALID_INDEX;
}

bool Cell::setFaceMaterialType(FaceCoordinates TargetCoordinates, int16_t MaterialTypeID)
{
    Face* TargetFace = getFace(TargetCoordinates);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceMaterialType(MaterialTypeID);
        Render->setNeedsReRendering();
        return true;
    }
    return false;
}

bool Cell::setFaceSurfaceType(FaceCoordinates TargetCoordinates, int16_t SurfaceTypeID)
{
    Face* TargetFace = getFace(TargetCoordinates);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceSurfaceType(SurfaceTypeID);
        Render->setNeedsReRendering();
        return true;
    }
    return false;
}

FaceShape Cell::getFaceShape(FaceCoordinates TargetCoordinates) const
{
    Face* TargetFace = getFace(TargetCoordinates);
    return TargetFace != NULL ? TargetFace->getFaceShapeType() : FaceShape(CubeShape(false), DIRECTION_NONE);
}

bool Cell::setFaceShape(FaceCoordinates TargetCoordinates, FaceShape NewShape)
{
    Face* TargetFace = getFace(TargetCoordinates);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceShapeType(NewShape);
        Render->setNeedsReRendering();
        return true;
    }
    return false;
}

bool Cell::removeFace(FaceCoordinates TargetCoordinates)
{
    uint16_t Key = TargetCoordinates.FaceKey();

    if (Faces.find(Key) != Faces.end())
    {
        delete Faces.find(Key)->second;
        Faces.erase(Key);

        Render->setNeedsReRendering();
        return true;
    }
    return false;
}

Face* Cell::addFace(FaceCoordinates TargetCoordinates)
{
    uint16_t Key = TargetCoordinates.FaceKey();

    if (Faces.find(Key) == Faces.end())
    {
        Face* NewFace = new Face(TargetCoordinates.Coordinates, TargetCoordinates.FaceDirection);
        Faces[Key] = NewFace;

        return NewFace;
    }
    else
    {
        return Faces.find(Key)->second;
    }
}

void Cell::addActor(Actor* NewActor)
{
	//Render->
    //CellSceneNode->addChild(NewActor->getNode());
    //NewActor->setVisible(Visible);
}

void Cell::removeActor(Actor* DepartingActor)
{
    //CellSceneNode->removeChild(DepartingActor->getNode());
}
