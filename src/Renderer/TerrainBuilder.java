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

package Renderer;

import com.jme3.app.Application;

import Map.*;
import Map.Coordinates.*;

import Renderer.TextureManager.TextureAtlasCoordinates;
import com.jme3.bounding.BoundingVolume;

import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import jme3tools.optimize.GeometryBatchFactory;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.Callable;

/**
 * Callable class that creates a optimized mesh for a Chunk and swaps it into
 * the Scene graph.
 *
 * @author Impaler
 */
public class TerrainBuilder implements Callable<Void> {

	Chunk BuildChunk;
	TileBuilder TileSource;
	Application app;
	Node TerrainLight, TerrainDark, ChunkLight, ChunkDark;
	Geometry[] LightBuildGeometry, DarkBuildGeometry;
	int DetailLevel;
	Spatial.CullHint hint;

	public TerrainBuilder(Application Parentapp, Chunk TargetChunk, TileBuilder Tiles, int LevelofDetail) {
		this.app = Parentapp;
		this.BuildChunk = TargetChunk;
		this.TileSource = Tiles;
		this.DetailLevel = LevelofDetail;
		
		this.LightBuildGeometry = new Geometry[BlockCoordinate.CHUNK_DETAIL_LEVELS];
		this.DarkBuildGeometry = new Geometry[BlockCoordinate.CHUNK_DETAIL_LEVELS];		
	}

	public void setNodes(Node LightChunkNode, Node DarkChunkNode) {
		this.ChunkLight = LightChunkNode;
		this.ChunkDark = DarkChunkNode;
		TerrainLight = new Node("TerrainLight");
		TerrainDark = new Node("TerrainDark");
	}

	public void setHint(Spatial.CullHint TerrainHint) {
		this.hint = TerrainHint;
	}

	public Void call() {
		TextureManager Texturing = TextureManager.getTextureManager();

			int BlockSize = 1 << this.DetailLevel;
			HashMap<FaceCoordinate, Face> faces = BuildChunk.getFaces(this.DetailLevel);
			Iterator<Map.Entry<FaceCoordinate, Face>> entries = faces.entrySet().iterator();
			while (entries.hasNext()) {
				Map.Entry<FaceCoordinate, Face> entry = entries.next();
				
				FaceCoordinate coords = entry.getKey();
				Face targetface = entry.getValue();

				TextureAtlasCoordinates AtlasCoords = Texturing.getTextureCoordinates(targetface.getFaceMaterialType(), targetface.getFaceSurfaceType());
				Mesh facemesh = TileSource.getMesh(targetface.getFaceShapeType(), AtlasCoords);
				if (facemesh != null) {
					Geometry geom = new Geometry("face", facemesh);
					//geom.scale(1.001f);  //T-Chunk junction hack
					
					geom.scale(BlockSize, BlockSize, BlockSize);
					geom.setLocalTranslation(new Vector3f(coords.getX() * BlockSize, coords.getY() * BlockSize, coords.getZ() * BlockSize));
					geom.setMaterial(TextureManager.getTextureManager().TerrainMaterial);

					if (targetface.isSunlit()) {
						TerrainLight.attachChild(geom);
					} else {
						TerrainDark.attachChild(geom);
					}
				}
			}

			GeometryBatchFactory.optimize(TerrainLight, true);
			if (TerrainLight.getQuantity() > 0) {
				LightBuildGeometry[this.DetailLevel] = (Geometry) TerrainLight.getChild(0);
				LightBuildGeometry[this.DetailLevel].setName("LightGeometry Chunk " + BuildChunk.toString() + "DetailLevel " + this.DetailLevel);
				TerrainLight.detachAllChildren();
			}

			GeometryBatchFactory.optimize(TerrainDark, true);
			if (TerrainDark.getQuantity() > 0) {
				DarkBuildGeometry[this.DetailLevel] = (Geometry) TerrainDark.getChild(0);
				DarkBuildGeometry[this.DetailLevel].setName("DarkGeometry Chunk " + BuildChunk.toString() + "DetailLevel " + this.DetailLevel);
				TerrainDark.detachAllChildren();
			}

		app.enqueue(new Callable() {
			public Object call() throws Exception {

				ChunkLight.detachChildNamed("LightGeometry Chunk " + BuildChunk.toString() + "DetailLevel " + DetailLevel);
				if (LightBuildGeometry[DetailLevel] != null) {
					ChunkLight.attachChild(LightBuildGeometry[DetailLevel]);
					LightBuildGeometry[DetailLevel].setCullHint(hint);
				}

				ChunkDark.detachChildNamed("DarkGeometry Chunk " + BuildChunk.toString() + "DetailLevel " + DetailLevel);
				if (DarkBuildGeometry[DetailLevel] != null) {
					ChunkDark.attachChild(DarkBuildGeometry[DetailLevel]);
					DarkBuildGeometry[DetailLevel].setCullHint(hint);
				}
				return null;
			}
		});
		return null;
	}
}
