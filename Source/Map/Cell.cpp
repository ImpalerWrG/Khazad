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
        Render->setDirty();

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

        //static int16_t NEHEMaterial = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
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
	Render->setDirty();
}

void Cell::RegisterWithRendering()
{
	Render->RegisterWithRendering();
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
        Render->setDirty();
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
        Render->setDirty();
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
        Render->setDirty();
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

        Render->setDirty();
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
	Render->AddActor(NewActor);
}

void Cell::removeActor(Actor* DepartingActor)
{
	Render->RemoveActor(DepartingActor);
}

void Cell::setNeedsReRendering()
{
	Render->setDirty();
}

void Cell::Save(boost::filesystem::basic_ofstream<char>& Stream) const
{
	Stream.write((char*)&thisCellCoordinates.X, sizeof(thisCellCoordinates.X));
	Stream.write((char*)&thisCellCoordinates.Y, sizeof(thisCellCoordinates.Y));
	Stream.write((char*)&thisCellCoordinates.Z, sizeof(thisCellCoordinates.Z));

	for (int i = 0; i < CUBESPERCELL; i++)
	{
		Stream.write((char*)&CubeShapeTypes[i], sizeof(CubeShapeTypes[i]));
		Stream.write((char*)&CubeMaterialTypes[i], sizeof(CubeMaterialTypes[i]));
	}

	int16_t FaceCount = Faces.size();
	Stream.write((char*)&FaceCount, sizeof(FaceCount));
	for (std::map<uint16_t, Face*>::const_iterator it = Faces.begin(); it != Faces.end(); it++)
	{
		uint16_t Key = it->first;
		Stream.write((char*)&Key, sizeof(Key));
		it->second->Save(Stream);
	}
}

void Cell::Load(boost::filesystem::basic_ifstream<char>& Stream)
{
	Stream.read((char*)&thisCellCoordinates.X, sizeof(thisCellCoordinates.X));
	Stream.read((char*)&thisCellCoordinates.Y, sizeof(thisCellCoordinates.Y));
	Stream.read((char*)&thisCellCoordinates.Z, sizeof(thisCellCoordinates.Z));

	for (int i = 0; i < CUBESPERCELL; i++)
	{
		Stream.read((char*)&CubeShapeTypes[i], sizeof(CubeShapeTypes[i]));
		Stream.read((char*)&CubeMaterialTypes[i], sizeof(CubeMaterialTypes[i]));
	}

	int16_t FaceCount;
	Stream.read((char*)&FaceCount, sizeof(FaceCount));
	for (int i = 0; i < FaceCount; i++)
	{
		uint16_t Key;
		Stream.read((char*)&Key, sizeof(Key));
		Face* NewFace = new Face();
		NewFace->Load(Stream);
		Faces[Key] = NewFace;
	}

	Render = new TerrainRendering(this);
}
