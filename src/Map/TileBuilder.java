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

import Map.Coordinates.Direction;
import Map.Coordinates.BlockCoordinate;
import com.jme3.scene.Mesh;

import com.jme3.scene.VertexBuffer.Type;
import com.jme3.math.Vector3f;
import com.jme3.math.Vector2f;

import Renderer.TextureManager.TextureAtlasCoordinates;

import java.util.ArrayList;
import static com.jme3.util.BufferUtils.createFloatBuffer;
import static com.jme3.util.BufferUtils.createIntBuffer;
import java.io.Serializable;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.concurrent.ConcurrentHashMap;


/**
 * Factory for the creation of Meshes that describe the shape of each possible
 * cube shape 
 * @author Impaler
 */
public class TileBuilder implements Serializable {
	private static final long serialVersionUID = 1;

	ConcurrentHashMap<FaceShape, MeshData> MesheDataMap;	// map tileshape to mesh for retrival

	private class MeshData {
		ArrayList<Vector3f> Vertices; 
		ArrayList<Vector3f> Normals;
		ArrayList<Vector2f> TextureCoords;
		ArrayList<Integer> Indexes;

		public MeshData() {
			this.Vertices = new ArrayList<Vector3f>(4);
			this.Normals = new ArrayList<Vector3f>(4);
			this.TextureCoords = new ArrayList<Vector2f>(4);
			this.Indexes = new ArrayList<Integer>(4);
		}

		public MeshData(ArrayList<Vector3f> Verts, ArrayList<Vector3f> Norms, ArrayList<Vector2f> Textures,  ArrayList<Integer> Indexs) {
			this.Vertices = Verts;
			this.Normals = Norms;
			this.TextureCoords = Textures;
			this.Indexes = Indexs;
		}
	}

	public TileBuilder() {
		MesheDataMap = new ConcurrentHashMap<FaceShape, MeshData>();
	}

	public Mesh getMesh(FaceShape Shape, TextureAtlasCoordinates AtlasCoords) {
		MeshData TargetMeshData = MesheDataMap.get(Shape);

		if (TargetMeshData == null) {
			if (Shape.getFaceDirection() == Direction.DIRECTION_NONE) {
				TargetMeshData = createSlopeFaceMesh(Shape);
				if (TargetMeshData != null) 
					MesheDataMap.put(Shape, TargetMeshData);
				return finalizeMesh(TargetMeshData, AtlasCoords);
			} else {
				if (Shape.getFaceDirection() == Direction.DIRECTION_DOWN || Shape.getFaceDirection() == Direction.DIRECTION_UP) {
					if (Shape.getSourceBlockShape().hasFloor() || Shape.getSourceBlockShape().hasCeiling()) {
						TargetMeshData = createFlatFaceMesh(Shape);
						if (TargetMeshData != null)
							MesheDataMap.put(Shape, TargetMeshData);
						return finalizeMesh(TargetMeshData, AtlasCoords);
					}
					return finalizeMesh(TargetMeshData, AtlasCoords);
				} else {
					TargetMeshData = createSideFaceMesh(Shape);
					if (TargetMeshData != null)
						MesheDataMap.put(Shape, TargetMeshData);
					return finalizeMesh(TargetMeshData, AtlasCoords);
				}
			}
		} else {
			return finalizeMesh(TargetMeshData, AtlasCoords);
		}
	}

	private Mesh finalizeMesh(MeshData Data, TextureAtlasCoordinates AtlasCoords) {
		if (Data == null || AtlasCoords == null)
			return null;

		Mesh ManualObject = new Mesh();

		FloatBuffer Vertbuff = createFloatBuffer(3 * Data.Vertices.size());
		for (Vector3f element : Data.Vertices) {
			Vertbuff.put(element.x);
			Vertbuff.put(element.y);
			Vertbuff.put(element.z);
		}
		Vertbuff.flip();

		FloatBuffer Normbuff = createFloatBuffer(3 * Data.Normals.size());
		for (Vector3f element : Data.Normals) {
			Normbuff.put(element.x);
			Normbuff.put(element.y);
			Normbuff.put(element.z);
		}
		Normbuff.flip();


		FloatBuffer Texbuff = createFloatBuffer(2 * Data.TextureCoords.size());
		for (Vector2f element : Data.TextureCoords) {
			float X = AtlasCoords.Left + ((AtlasCoords.Right - AtlasCoords.Left) * element.x);
			float Y = AtlasCoords.Top + ((AtlasCoords.Bottom - AtlasCoords.Top) * element.y);

			Texbuff.put(X);
			Texbuff.put(Y);
		}
		Texbuff.flip();

		IntBuffer Indxbuff = createIntBuffer(Data.Indexes.size());
		Indxbuff.clear();
		for (Integer element : Data.Indexes) {
			Indxbuff.put(element.intValue());
		}
		Indxbuff.flip();

		ManualObject.setBuffer(Type.Position, 3, Vertbuff);
		ManualObject.setBuffer(Type.Normal,   3, Normbuff);					
		ManualObject.setBuffer(Type.TexCoord, 2, Texbuff);
		ManualObject.setBuffer(Type.Index,    3, Indxbuff);

		ManualObject.updateBound();
		return ManualObject;
	}

	private MeshData createFlatFaceMesh(FaceShape Shape) {

		boolean Triangle1 = false;
		boolean Triangle2 = false;

		byte NorthEastCorner = Shape.getSourceBlockShape().getNorthEastCorner();
		byte NorthWestCorner = Shape.getSourceBlockShape().getNorthWestCorner();
		byte SouthEastCorner = Shape.getSourceBlockShape().getSouthEastCorner();
		byte SouthWestCorner = Shape.getSourceBlockShape().getSouthWestCorner();
				
		ArrayList<Vector3f> Vertices = new ArrayList<Vector3f>(4);
		ArrayList<Vector3f> Normals = new ArrayList<Vector3f>(4);
		ArrayList<Vector2f> TextureCoords = new ArrayList<Vector2f>(4);
		ArrayList<Integer> Indexes = new ArrayList<Integer>(4);	

			final int SW = 0;  final int SE = 1;  final int NW = 2;  final int NE = 3;

			Vertices.add(SW, new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, ((SouthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
			Vertices.add(SE, new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, ((SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
			Vertices.add(NW, new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, ((NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
			Vertices.add(NE, new Vector3f( BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, ((NorthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));

			Normals.add(SW, Vector3f.UNIT_Z);
			Normals.add(SE, Vector3f.UNIT_Z);
			Normals.add(NW, Vector3f.UNIT_Z);
			Normals.add(NE, Vector3f.UNIT_Z);

			TextureCoords.add(SW, new Vector2f(0.0f, 0.0f));
			TextureCoords.add(SE, new Vector2f(1.0f, 0.0f));
			TextureCoords.add(NW, new Vector2f(0.0f, 1.0f));
			TextureCoords.add(NE, new Vector2f(1.0f, 1.0f));

			if (Shape.getSourceBlockShape().isSplit()) // Split along NW-SE line
			{
				if ((SouthEastCorner == BlockShape.CUBE_BOTTOM_HEIGHT && NorthEastCorner == BlockShape.CUBE_BOTTOM_HEIGHT && NorthWestCorner == BlockShape.CUBE_BOTTOM_HEIGHT) || (SouthEastCorner == BlockShape.CUBE_TOP_HEIGHT && NorthEastCorner == BlockShape.CUBE_TOP_HEIGHT && NorthWestCorner == BlockShape.CUBE_TOP_HEIGHT))
				{
					Indexes.add(SE);  // South East
					Indexes.add(NE);  // North East
					Indexes.add(NW);  // North West
					
					Triangle1 = true;
				}

				if ((NorthWestCorner == BlockShape.CUBE_BOTTOM_HEIGHT && SouthWestCorner == BlockShape.CUBE_BOTTOM_HEIGHT && SouthEastCorner == BlockShape.CUBE_BOTTOM_HEIGHT) || (NorthWestCorner == BlockShape.CUBE_TOP_HEIGHT && SouthWestCorner == BlockShape.CUBE_TOP_HEIGHT && SouthEastCorner == BlockShape.CUBE_TOP_HEIGHT))
				{
					Indexes.add(NW);
					Indexes.add(SW);
					Indexes.add(SE);

					Triangle2 = true;
				}
			}
			else // Split along SW-NE line
			{
				if ((NorthEastCorner == BlockShape.CUBE_BOTTOM_HEIGHT && NorthWestCorner == BlockShape.CUBE_BOTTOM_HEIGHT && SouthWestCorner == BlockShape.CUBE_BOTTOM_HEIGHT) || (NorthEastCorner == BlockShape.CUBE_TOP_HEIGHT && NorthWestCorner == BlockShape.CUBE_TOP_HEIGHT && SouthWestCorner == BlockShape.CUBE_TOP_HEIGHT))
				{
					Indexes.add(NE);
					Indexes.add(NW);
					Indexes.add(SW);

					Triangle1 = true;
				}

				if ((SouthWestCorner == BlockShape.CUBE_BOTTOM_HEIGHT && SouthEastCorner == BlockShape.CUBE_BOTTOM_HEIGHT && NorthEastCorner == BlockShape.CUBE_BOTTOM_HEIGHT) || (SouthWestCorner == BlockShape.CUBE_TOP_HEIGHT && SouthEastCorner == BlockShape.CUBE_TOP_HEIGHT && NorthEastCorner == BlockShape.CUBE_TOP_HEIGHT))
				{
					Indexes.add(SW);
					Indexes.add(SE);
					Indexes.add(NE);

					Triangle2 = true;
				}
			}

		if (Triangle1 || Triangle2) {
			return new MeshData(Vertices, Normals, TextureCoords, Indexes);
		} else {
			return null;
		}
	}
 
	private MeshData createSideFaceMesh(FaceShape Shape) {
		
		byte NorthEastCorner = Shape.getSourceBlockShape().getNorthEastCorner();
		byte NorthWestCorner = Shape.getSourceBlockShape().getNorthWestCorner();
		byte SouthEastCorner = Shape.getSourceBlockShape().getSouthEastCorner();
		byte SouthWestCorner = Shape.getSourceBlockShape().getSouthWestCorner();		

		ArrayList<Vector3f> Vertices = new ArrayList<Vector3f>();
		ArrayList<Vector3f> Normals = new ArrayList<Vector3f>();
		ArrayList<Vector2f> TextureCoords = new ArrayList<Vector2f>();
		ArrayList<Integer> Indexes = new ArrayList<Integer>();	

		boolean Triangle = false;
		BlockShape Block = Shape.getSourceBlockShape();
		BlockShape Adjacent = Shape.getAdjacentBlockShape();


		float XLeft = 0; float XRight = 0; float YLeft = 0; float YRight = 0;
		int LeftCorner = 0; int LeftAdjacentCorner = 0;
		int RightCorner = 0; int RightAdjacentCorner = 0;
		Vector3f Normal = new Vector3f();

		switch (Shape.getFaceDirection())
		{
			case DIRECTION_SOUTH:
				XLeft = -BlockCoordinate.HALF_BLOCK;  YLeft = -BlockCoordinate.HALF_BLOCK;  XRight =  BlockCoordinate.HALF_BLOCK;  YRight = -BlockCoordinate.HALF_BLOCK;
				LeftCorner = Block.getSouthWestCorner();   RightCorner = Block.getSouthEastCorner();
				LeftAdjacentCorner = Adjacent.getNorthWestCorner();  RightAdjacentCorner = Adjacent.getNorthEastCorner();
				Normal = Vector3f.UNIT_Y.negate();
				break;

			case DIRECTION_NORTH:
				XLeft =  BlockCoordinate.HALF_BLOCK;  YLeft =  BlockCoordinate.HALF_BLOCK;  XRight = -BlockCoordinate.HALF_BLOCK;  YRight =  BlockCoordinate.HALF_BLOCK;
				LeftCorner = Block.getNorthEastCorner();  RightCorner = Block.getNorthWestCorner();
				LeftAdjacentCorner = Adjacent.getSouthEastCorner();  RightAdjacentCorner = Adjacent.getSouthWestCorner();
				Normal = Vector3f.UNIT_Y;
				break;

			case DIRECTION_WEST:
				XLeft = -BlockCoordinate.HALF_BLOCK;  YLeft =  BlockCoordinate.HALF_BLOCK;  XRight = -BlockCoordinate.HALF_BLOCK;  YRight = -BlockCoordinate.HALF_BLOCK;
				LeftCorner = Block.getNorthWestCorner();  RightCorner = Block.getSouthWestCorner();
				LeftAdjacentCorner = Adjacent.getNorthEastCorner();  RightAdjacentCorner = Adjacent.getSouthEastCorner();
				Normal = Vector3f.UNIT_X.negate();
				break;

			case DIRECTION_EAST:
				XLeft =  BlockCoordinate.HALF_BLOCK;  YLeft = -BlockCoordinate.HALF_BLOCK;  XRight =  BlockCoordinate.HALF_BLOCK;  YRight =  BlockCoordinate.HALF_BLOCK;
				LeftCorner = Block.getSouthEastCorner();  RightCorner = Block.getNorthEastCorner();
				LeftAdjacentCorner = Adjacent.getSouthWestCorner();  RightAdjacentCorner = Adjacent.getNorthWestCorner();
				Normal = Vector3f.UNIT_X;
				break;

			default:
				break;
		}

		float LeftTop = Math.max(LeftCorner, LeftAdjacentCorner);
		LeftTop = (Math.min(BlockShape.CUBE_TOP_HEIGHT, Math.max(BlockShape.CUBE_BOTTOM_HEIGHT, LeftTop)) - 1.0f) / BlockShape.HEIGHT_FRACTIONS;

		float RightTop = Math.max(RightCorner, RightAdjacentCorner);
		RightTop = (Math.min(BlockShape.CUBE_TOP_HEIGHT, Math.max(BlockShape.CUBE_BOTTOM_HEIGHT, RightTop)) - 1.0f) / BlockShape.HEIGHT_FRACTIONS;
		
		float LeftBottom = Math.min(LeftCorner, LeftAdjacentCorner);
		LeftBottom = (Math.min(BlockShape.CUBE_TOP_HEIGHT, Math.max(BlockShape.CUBE_BOTTOM_HEIGHT, LeftBottom)) - 1.0f) / BlockShape.HEIGHT_FRACTIONS;
		if (Adjacent.isSky() && Block.hasFace(Shape.getFaceDirection()) && !Block.isSky()) {
			LeftBottom = (BlockShape.CUBE_BOTTOM_HEIGHT - 1.0f) / BlockShape.HEIGHT_FRACTIONS;
		}
		
		float RightBottom = Math.min(RightCorner, RightAdjacentCorner);
		RightBottom = (Math.min(BlockShape.CUBE_TOP_HEIGHT, Math.max(BlockShape.CUBE_BOTTOM_HEIGHT, RightBottom)) - 1.0f) / BlockShape.HEIGHT_FRACTIONS;
		if (Adjacent.isSky() && Block.hasFace(Shape.getFaceDirection()) && !Block.isSky()) {
			RightBottom = (BlockShape.CUBE_BOTTOM_HEIGHT - 1.0f) / BlockShape.HEIGHT_FRACTIONS;
		}
	
		Vertices.add(new Vector3f(XLeft, YLeft, LeftBottom - BlockCoordinate.HALF_BLOCK));  // Left Bottom
		Normals.add(Normal);
		TextureCoords.add(new Vector2f(0.0f, LeftBottom));

		Vertices.add(new Vector3f(XLeft, YLeft, LeftTop - BlockCoordinate.HALF_BLOCK));  // Left Top
		Normals.add(Normal);
		TextureCoords.add(new Vector2f(0.0f, LeftTop));

		Vertices.add(new Vector3f(XRight, YRight, RightBottom - BlockCoordinate.HALF_BLOCK));  // Right Bottom
		Normals.add(Normal);
		TextureCoords.add(new Vector2f(1.0f, RightBottom));

		Vertices.add(new Vector3f(XRight, YRight, RightTop - BlockCoordinate.HALF_BLOCK));  // Right Top
		Normals.add(Normal);
		TextureCoords.add(new Vector2f(1.0f, RightTop));


		if (LeftCorner >= LeftAdjacentCorner || RightCorner >= RightAdjacentCorner) { 
			if (LeftCorner > BlockShape.BELOW_CUBE_HEIGHT && RightCorner != BlockShape.BELOW_CUBE_HEIGHT) {
				Indexes.add(3);
				Indexes.add(1);
				Indexes.add(0);
				Triangle = true;
			}

			if (LeftCorner != BlockShape.BELOW_CUBE_HEIGHT && RightCorner > BlockShape.CUBE_BOTTOM_HEIGHT) {
				Indexes.add(0);
				Indexes.add(2);
				Indexes.add(3);
				Triangle = true;
			}
		} else {

			if (LeftAdjacentCorner > BlockShape.BELOW_CUBE_HEIGHT && RightAdjacentCorner != BlockShape.BELOW_CUBE_HEIGHT) {
				//Indexes.add(0);
				//Indexes.add(1);
				//Indexes.add(3);
				//Triangle = true;
			}

			if (LeftAdjacentCorner != BlockShape.BELOW_CUBE_HEIGHT && RightAdjacentCorner > BlockShape.CUBE_BOTTOM_HEIGHT) {
				//Indexes.add(3);
				//Indexes.add(2);
				//Indexes.add(0);
				//Triangle = true;
			}
		}
		

		if (Triangle) {
			return new MeshData(Vertices, Normals, TextureCoords, Indexes);
		} else {
			return null;
		}
	}

	private MeshData createSlopeFaceMesh(FaceShape Shape) {
		
		boolean Triangle1 = false;
		boolean Triangle2 = false;

		byte NorthEastCorner = Shape.getSourceBlockShape().getNorthEastCorner();
		byte NorthWestCorner = Shape.getSourceBlockShape().getNorthWestCorner();
		byte SouthEastCorner = Shape.getSourceBlockShape().getSouthEastCorner();
		byte SouthWestCorner = Shape.getSourceBlockShape().getSouthWestCorner();

		ArrayList<Vector3f> Vertices = new ArrayList<Vector3f>();
		ArrayList<Vector3f> Normals = new ArrayList<Vector3f>();
		ArrayList<Vector2f> TextureCoords = new ArrayList<Vector2f>();
		ArrayList<Integer> Indexes = new ArrayList<Integer>();	


		final int SW = 0;  final int SE = 1;  final int NW = 2;  final int NE = 3;

		{
			Vector3f SWv = new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK);
			Vector3f SEv = new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK);
			Vector3f NWv = new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK);
			Vector3f NEv = new Vector3f( BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, (((float) NorthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK);

			Vector3f NWBv = new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
			Vector3f SEBv = new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
			Vector3f NEBv = new Vector3f( BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
			Vector3f SWBv = new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);

					
			Vector3f Normal = new Vector3f();
			
			Vector3f X = Vector3f.UNIT_X;
			Vector3f Y = Vector3f.UNIT_Y;

			if (Shape.getSourceBlockShape().isSplit()) // Split along the NW-SE line
			{
				// Triangle1 SE->NE->NW
				if (SouthEastCorner > BlockShape.BELOW_CUBE_HEIGHT && NorthEastCorner > BlockShape.BELOW_CUBE_HEIGHT && NorthWestCorner > BlockShape.BELOW_CUBE_HEIGHT)
				{
					if (SouthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT || NorthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT || NorthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						if (SouthEastCorner < BlockShape.CUBE_TOP_HEIGHT || NorthEastCorner < BlockShape.CUBE_TOP_HEIGHT || NorthWestCorner < BlockShape.CUBE_TOP_HEIGHT)
						{
							Vertices.add(SEv);
							Normals.add(new Vector3f( NEv.subtract(SEv).cross( NWv.subtract(SEv)).normalize()));
							TextureCoords.add(new Vector2f(1.0f, 0.0f));
							Indexes.add(Vertices.size() - 1);

							Vertices.add(NEv);
							Normals.add(new Vector3f( NWv.subtract(NEv)).cross( SEv.subtract(NEv) ).normalize());
							TextureCoords.add(new Vector2f(1.0f, 1.0f));
							Indexes.add(Vertices.size() - 1);

							Vertices.add(NWv);
							Normals.add(new Vector3f( SEv.subtract(NWv)).cross( NEv.subtract(NWv) ).normalize());
							TextureCoords.add(new Vector2f(0.0f, 1.0f));
							Indexes.add(Vertices.size() - 1);

							Triangle1 = true;
						}
					}
				}

				// Triangle2 NW->SW->SE
				if (NorthWestCorner > BlockShape.BELOW_CUBE_HEIGHT && SouthWestCorner > BlockShape.BELOW_CUBE_HEIGHT && SouthEastCorner > BlockShape.BELOW_CUBE_HEIGHT)
				{
					if (NorthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT || SouthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT || SouthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						if (NorthWestCorner < BlockShape.CUBE_TOP_HEIGHT || SouthWestCorner < BlockShape.CUBE_TOP_HEIGHT || SouthEastCorner < BlockShape.CUBE_TOP_HEIGHT)
						{
							Vertices.add(new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
							Normals.add(new Vector3f( SWv.subtract(NWv).cross(SEv.subtract(NWv)).normalize()));
							TextureCoords.add(new Vector2f(0.0f, 1.0f));
							Indexes.add(Vertices.size() - 1);

							Vertices.add(new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
							Normals.add(new Vector3f( SEv.subtract(SWv)).cross(NWv.subtract(SWv)).normalize());
							TextureCoords.add(new Vector2f(0.0f, 0.0f));
							Indexes.add(Vertices.size() - 1);

							Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
							Normals.add(new Vector3f( NWv.subtract(SEv)).cross(SWv.subtract(SEv)).normalize());
							TextureCoords.add(new Vector2f(1.0f, 0.0f));
							Indexes.add(Vertices.size() - 1);

							Triangle2 = true;
						}
					}
				}

				// Vertical face inside Block when only one triangle is drawn
				if ((Triangle1 ^ Triangle2) && ((NorthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT || SouthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)))
				{
					if ((Triangle1 && SouthWestCorner < BlockShape.CUBE_TOP_HEIGHT) || (Triangle2 && NorthEastCorner < BlockShape.CUBE_TOP_HEIGHT))
					{
						/*
						Vector3f Vertex4 = new Vector3f( -MapCoordinates.HALFCUBE, MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE);
						Vector3f Vertex5 = new Vector3f( MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE);

						
						Vertices.add(new Vector3f(-MapCoordinates.HALFCUBE,  MapCoordinates.HALFCUBE, (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -MapCoordinates.HALFCUBE));
						Normals.add(Normal);
						TextureCoords.add(new Vector2f(1.0f - ((float) NorthWestCorner / BlockShape.HEIGHT_FRACTIONS), 0.0f));

						Vertices.add(new Vector3f( MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE, (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -MapCoordinates.HALFCUBE));
						Normals.add(Normal);
						TextureCoords.add(new Vector2f(1.0f - ((float) SouthEastCorner / BlockShape.HEIGHT_FRACTIONS), 1.0f));

						Vertices.add(new Vector3f( -MapCoordinates.HALFCUBE, MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE));  // North West Bottom  5
						Normals.add(Normal);
						TextureCoords.add(new Vector2f(1.0f, 0.0f));

						Vertices.add(new Vector3f( MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE, -MapCoordinates.HALFCUBE));  // South East Bottom  6
						Normals.add(Normal);
						TextureCoords.add(new Vector2f(1.0f, 1.0f));
*/

						if (Triangle1)
						{
							Normal = X.negate().add(Y.negate());

							if (NorthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 1.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 0.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(new Vector3f( -BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK));  // North West Bottom  5
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 0.0f));
								Indexes.add(Vertices.size() - 1);
								
								// SE->NW->NW Bottom
							}
							if (SouthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(new Vector3f( -BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK));  // North West Bottom  5
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 0.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK));  // South East Bottom  6
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 1.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 1.0f));
								Indexes.add(Vertices.size() - 1);
								
								// NW Bottom->SE Bottom->SE
							}
						}
						
						if (Triangle2)
						{
							Normal = X.add(Y);

							if (SouthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 0.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) SouthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 1.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK));  // South East Bottom  6
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 1.0f));
								Indexes.add(Vertices.size() - 1);

								//ManualObject->triangle(3, 4, 6);  // NW->SE->SE Bottom
							}
							if (NorthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(new Vector3f( BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK));  // South East Bottom  6
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 1.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(new Vector3f( -BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK));  // North West Bottom  5
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 0.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(new Vector3f(-BlockCoordinate.HALF_BLOCK,  BlockCoordinate.HALF_BLOCK, (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS) -BlockCoordinate.HALF_BLOCK));
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) NorthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 0.0f));
								Indexes.add(Vertices.size() - 1);

								//ManualObject->triangle(6, 5, 3);  // SE Bottom->NW Bottom->NW
							}
						}
					}
				}

				// Vertical bisector through whole cube
				if (!Triangle1 && !Triangle2 && NorthWestCorner >= BlockShape.CUBE_TOP_HEIGHT && SouthEastCorner >= BlockShape.CUBE_TOP_HEIGHT)
				{
					if (NorthEastCorner < BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						Normal = X.add(Y);
					}
					if (SouthWestCorner < BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						Normal = X.negate().add(Y.negate());	
					}

					int Start = Vertices.size();
					
					Vertices.add(NWv);  // North West  0
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(0.0f, 0.0f));

					Vertices.add(SEv);  // South East  1
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(0.0f, 1.0f));

					Vertices.add(NWBv);  // North West Bottom  2
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(1.0f, 0.0f));

					Vertices.add(SEBv);  // South East Bottom  3
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(1.0f, 1.0f));

					if (NorthEastCorner < BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						// NW Bottom-> NW-> SE
						Indexes.add(Start + 2);
						Indexes.add(Start + 0);
						Indexes.add(Start + 1); 
						
						// SE ->SE Bottom-> NW Bottom
						Indexes.add(Start + 1);
						Indexes.add(Start + 3);
						Indexes.add(Start + 2);
					}
					else
					{
						// SE-> NW-> NW Bottom
						Indexes.add(Start + 1);
						Indexes.add(Start + 0);
						Indexes.add(Start + 2);

						// NW Bottom ->SE Bottom-> SE
						Indexes.add(Start + 2);
						Indexes.add(Start + 3);
						Indexes.add(Start + 1); 
					}

					Triangle1 = true;
					Triangle2 = true;
				 }
			}
			else // Split along the SW-NE line
			{
				// Triangle1 NE->NW->SW
				if (NorthEastCorner > BlockShape.BELOW_CUBE_HEIGHT && NorthWestCorner > BlockShape.BELOW_CUBE_HEIGHT && SouthWestCorner > BlockShape.BELOW_CUBE_HEIGHT)
				{
					if (NorthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT || NorthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT || SouthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						if (NorthEastCorner < BlockShape.CUBE_TOP_HEIGHT || NorthWestCorner < BlockShape.CUBE_TOP_HEIGHT || SouthWestCorner < BlockShape.CUBE_TOP_HEIGHT)
						{
							Vertices.add(NEv);  // North East
							Normals.add( (NWv.subtract(NEv)).cross(SWv.subtract(NEv)).normalize());
							TextureCoords.add(new Vector2f(1.0f, 1.0f));
							Indexes.add(Vertices.size() - 1);
							
							Vertices.add(NWv);  // North West
							Normals.add( (SWv.subtract(NWv)).cross( NEv.subtract(NWv)).normalize());
							TextureCoords.add(new Vector2f(0.0f, 1.0f));
							Indexes.add(Vertices.size() - 1);
							
							Vertices.add(SWv);  // South West
							Normals.add( (NEv.subtract(SWv)).cross( NWv.subtract(SWv)).normalize());
							TextureCoords.add(new Vector2f(0.0f, 0.0f));
							Indexes.add(Vertices.size() - 1);
							
							Triangle1 = true;
						}
					}
				}

				// Triangle2 SW->SE->NE
				if (SouthWestCorner > BlockShape.BELOW_CUBE_HEIGHT && SouthEastCorner > BlockShape.BELOW_CUBE_HEIGHT && NorthEastCorner > BlockShape.BELOW_CUBE_HEIGHT)
				{
					if (SouthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT || SouthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT || NorthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						if (SouthWestCorner < BlockShape.CUBE_TOP_HEIGHT || SouthEastCorner < BlockShape.CUBE_TOP_HEIGHT || NorthEastCorner < BlockShape.CUBE_TOP_HEIGHT)
						{
							Vertices.add(SWv);  // South West
							Normals.add(( SEv.subtract(SWv)).cross( NEv.subtract(SWv)).normalize());
							TextureCoords.add(new Vector2f(0.0f, 0.0f));
							Indexes.add(Vertices.size() - 1);

							Vertices.add(SEv);  // South East
							Normals.add(( NEv.subtract(SEv)).cross( SWv.subtract(SEv)).normalize());
							TextureCoords.add(new Vector2f(1.0f, 0.0f));
							Indexes.add(Vertices.size() - 1);

							Vertices.add(NEv);  // North East
							Normals.add(( SWv.subtract(NEv)).cross( SEv.subtract(NEv)).normalize());
							TextureCoords.add(new Vector2f(1.0f, 1.0f));
							Indexes.add(Vertices.size() - 1);
							
							Triangle2 = true;
						}
					}
				}

				// Vertical face inside Block when only one triangle is drawn
				if ((Triangle1 ^ Triangle2) && ((NorthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT || SouthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)))
				{
					if ((Triangle1 && SouthEastCorner < BlockShape.CUBE_TOP_HEIGHT) || (Triangle2 && NorthWestCorner < BlockShape.CUBE_TOP_HEIGHT))
					{
						if (Triangle1)
						{
							Normal = X.add(Y.negate());

							if (NorthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(SWBv);  // South West Bottom  6
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 1.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(NEBv);  // North East Bottom  5
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 0.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(NEv);  // North East  3
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) NorthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 0.0f));
								Indexes.add(Vertices.size() - 1);
								
								// SW Bottom->NE Bottom->NE
							}
							if (SouthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(NEv);  // North East
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) NorthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 0.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(SWv);  // South West
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) SouthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 1.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(SWBv);  // South West Bottom
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 1.0f));
								Indexes.add(Vertices.size() - 1);

								// NE->SW->SW Bottom
							}
						}
						if (Triangle2)
						{
							Normal = Y.add(X.negate());
		
							if (NorthEastCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(SWv);  // South West  4
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) SouthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 1.0f));
								Indexes.add(Vertices.size() - 1);
								
								Vertices.add(NEv);  // North East  3
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) NorthEastCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 0.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(NEBv);  // North East Bottom  5
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 0.0f));
								Indexes.add(Vertices.size() - 1);

								// SW->NE->NE Bottom
							}
							if (SouthWestCorner > BlockShape.CUBE_BOTTOM_HEIGHT)
							{
								Vertices.add(NEBv);  // North East Bottom
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 0.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(SWBv);  // South West Bottom
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f, 1.0f));
								Indexes.add(Vertices.size() - 1);

								Vertices.add(SWv);  // South West
								Normals.add(Normal);
								TextureCoords.add(new Vector2f(1.0f - (((float) SouthWestCorner - 1) / BlockShape.HEIGHT_FRACTIONS), 1.0f));
								Indexes.add(Vertices.size() - 1);
							
								// NE Bottom->SW Bottom->SW
							}
						}
					}
				}

				// Vertical bisector through whole cube
				if (!Triangle1 && !Triangle2 && SouthWestCorner >= BlockShape.CUBE_TOP_HEIGHT && NorthEastCorner >= BlockShape.CUBE_TOP_HEIGHT)
				{
					if (SouthEastCorner < BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						Normal = (X.negate()).add(Y.negate());
					}
					if (NorthWestCorner < BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						Normal = X.add(Y);
					}

					int Start = Vertices.size();
							
					Vertices.add(NEv);  // North East
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(0.0f, 0.0f));

					Vertices.add(SWv);  // South West
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(0.0f, 1.0f));

					Vertices.add(NEBv);  // North East Bottom
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(1.0f, 0.0f));

					Vertices.add(SWBv);  // South West Bottom
					Normals.add(Normal);
					TextureCoords.add(new Vector2f(1.0f, 1.0f));

					if (SouthEastCorner < BlockShape.CUBE_BOTTOM_HEIGHT)
					{
						Indexes.add(Start + 2);
						Indexes.add(Start + 0);
						Indexes.add(Start + 1);
						// NW Bottom-> NW-> SW
						
						Indexes.add(Start + 1);
						Indexes.add(Start + 3);
						Indexes.add(Start + 2);
						// SW ->SW Bottom-> NW Bottom
					}
					else
					{
						Indexes.add(Start + 1);
						Indexes.add(Start + 0);
						Indexes.add(Start + 2);
						 // SE-> NW-> NW Bottom
						
						Indexes.add(Start + 2);
						Indexes.add(Start + 3);
						Indexes.add(Start + 1);
						// NW Bottom ->SE Bottom-> SE
					}

					Triangle1 = true;
					Triangle2 = true;
				 }
			}
		}

		if (Triangle1 || Triangle2) {
			return new MeshData(Vertices, Normals, TextureCoords, Indexes);
		} else {
			return null;
		}
	}
}