/* Copyright 2010 Kenneth 'Impaler' Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */

package Map;

import Data.DataTypes;

/**
 * Describes a Face both its shape, location inside a Cell and it's material
 * for gameplay purposes.
 * @author Impaler
 */
public class Face {

	FaceShape FaceType;
	byte LocationCoordinates;

	short SurfaceTypeID;
	short MaterialTypeID;	
	
	public Face() {
		SurfaceTypeID = DataTypes.INVALID_INDEX;
		MaterialTypeID = DataTypes.INVALID_INDEX;
		
		CubeShape SourceCube = new CubeShape((byte) 0);
		CubeShape AdjacentCube = new CubeShape((byte) 0);
		FaceType = new FaceShape(SourceCube, AdjacentCube, Direction.DIRECTION_NONE);
	}

	public Face(byte TargetCoordinates, Direction DirectionType) {
		SurfaceTypeID = DataTypes.INVALID_INDEX;
		MaterialTypeID = DataTypes.INVALID_INDEX;

		CubeShape SourceCube = new CubeShape((byte) 0);
		CubeShape AdjacentCube = new CubeShape((byte) 0);
		FaceType = new FaceShape(SourceCube, AdjacentCube, Direction.DIRECTION_NONE);
		LocationCoordinates = TargetCoordinates;
	}

	/*
	public void RefreshEntity(Ogre::StaticGeometry* CellGeometry, CellCoordinates CellPosition) {
		if (SurfaceTypeID != INVALID_INDEX && MaterialTypeID != INVALID_INDEX)
		{
			short X = (CellPosition.X * MapCoordinates.CELLEDGESIZE) + (LocationCoordinates >> MapCoordinates.CELLBITSHIFT);
			short Y = (CellPosition.Y * MapCoordinates.CELLEDGESIZE) + (LocationCoordinates & MapCoordinates.CELLBITFLAG);
			short Z = CellPosition.Z;

			if (FaceType.FaceDirection.ValueonAxis(AXIS_X) == 1) {
				X -= 1;
			}

			if (FaceType.FaceDirection.ValueonAxis(AXIS_Y) == 1) {
				Y -= 1;
			}

			if (FaceType.FaceDirection.ValueonAxis(AXIS_Z) == 1) {
				Z -= 1;
			}

			Ogre::MeshPtr FaceMesh = SHAPE->getFaceMesh(FaceType);
			if (!FaceMesh.isNull())
			{
				Ogre::Entity* NewEntity = RENDERER->getSceneManager()->createEntity(FaceMesh);
				NewEntity->setMaterial(TEXTURE->getOgreMaterial(MaterialTypeID, SurfaceTypeID));
				CellGeometry->addEntity(NewEntity, Ogre::Vector3(X, Y, Z));
				RENDERER->getSceneManager()->destroyEntity(NewEntity);
			}
		}
	}*/

	public void setFaceMaterialType(short NewMaterialTypeID) {
		if (NewMaterialTypeID != MaterialTypeID)
		{
			MaterialTypeID = NewMaterialTypeID;
			//Set Cell needsRedraw
		}
	}

	public void setFaceSurfaceType(short NewSurfaceTypeID) {
		if (NewSurfaceTypeID != SurfaceTypeID)
		{
			SurfaceTypeID = NewSurfaceTypeID;
			//Set Cell needsRedraw
		}
	}

	public void setFaceShapeType(FaceShape NewShape) {
		if (NewShape != FaceType)
		{
			FaceType = NewShape;
			//Set Cell needsRedraw
		}
	}

	public short getFaceMaterialType() {
		return MaterialTypeID; 
	}

    public short getFaceSurfaceType() {
		return SurfaceTypeID; 
	}

    public FaceShape getFaceShapeType() {
		return FaceType; 
	}
	/*
	void Save(boost::filesystem::basic_ofstream<char>& Stream) const
	{
		Stream.write((char*)&FaceType, sizeof(FaceType));
		Stream.write((char*)&LocationCoordinates, sizeof(LocationCoordinates));
		Stream.write((char*)&SurfaceTypeID, sizeof(SurfaceTypeID));
		Stream.write((char*)&MaterialTypeID, sizeof(MaterialTypeID));
	}

	void Load(boost::filesystem::basic_ifstream<char>& Stream)
	{
		Stream.read((char*)&FaceType, sizeof(FaceType));
		Stream.read((char*)&LocationCoordinates, sizeof(LocationCoordinates));
		Stream.read((char*)&SurfaceTypeID, sizeof(SurfaceTypeID));
		Stream.read((char*)&MaterialTypeID, sizeof(MaterialTypeID));
	}
	* */

}
