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

import Map.Cell;
import Map.CubeCoordinate;
import Map.Face;
import Map.FaceCoordinate;
import Map.TileBuilder;

import Renderer.TextureManager.TextureAtlasCoordinates;
import com.jme3.bounding.BoundingVolume;

import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.scene.control.LodControl;

import jme3tools.optimize.GeometryBatchFactory;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.Callable;

/**
 * Callable class that creates a optimized mesh for a Cell and swaps it into
 * the Scene graph.
 *
 * @author Impaler
 */
public class TerrainBuilder implements Callable<Void> {

	Cell BuildCell;
	TileBuilder TileSource;
	Application app;
	Node TerrainLight, TerrainDark, CellLight, CellDark;
	Geometry[] LightBuildGeometry, DarkBuildGeometry;
	int DetailLevel;

	public TerrainBuilder(Application Parentapp, Cell TargetCell, TileBuilder Tiles, int LevelofDetail) {
		this.app = Parentapp;
		this.BuildCell = TargetCell;
		this.TileSource = Tiles;
		this.DetailLevel = LevelofDetail;
		
		this.LightBuildGeometry = new Geometry[CubeCoordinate.CELLDETAILLEVELS];
		this.DarkBuildGeometry = new Geometry[CubeCoordinate.CELLDETAILLEVELS];		
	}

	public void setNodes(Node LightCellNode, Node DarkCellNode) {
		this.CellLight = LightCellNode;
		this.CellDark = DarkCellNode;
		TerrainLight = new Node("TerrainLight");
		TerrainDark = new Node("TerrainDark");
	}

	public Void call() {
		TextureManager Texturing = TextureManager.getTextureManager();

		for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i ++) {
			int CubeSize = 1 << i;
			// Terrain Faces
			HashMap<FaceCoordinate, Face> faces = BuildCell.getFaces(i);
			Iterator<Map.Entry<FaceCoordinate, Face>> entries = faces.entrySet().iterator();
			while (entries.hasNext()) {
				Map.Entry<FaceCoordinate, Face> entry = entries.next();
				
				FaceCoordinate coords = entry.getKey();
				Face targetface = entry.getValue();

				TextureAtlasCoordinates AtlasCoords = Texturing.getTextureCoordinates(targetface.getFaceMaterialType(), targetface.getFaceSurfaceType());
				Mesh facemesh = TileSource.getMesh(targetface.getFaceShapeType(), AtlasCoords);
				if (facemesh != null) {
					Geometry geom = new Geometry("face", facemesh);
					//geom.scale(1.001f);  //T-Cell junction hack
					geom.scale(CubeSize, CubeSize, 1);
					geom.setLocalTranslation(new Vector3f(coords.getX() * CubeSize, coords.getY() * CubeSize, 0));
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
				LightBuildGeometry[i] = (Geometry) TerrainLight.getChild(0);
				LightBuildGeometry[i].setName("LightGeometry Cell " + BuildCell.toString() + "DetailLevel " + i);
				TerrainLight.detachAllChildren();
			}

			GeometryBatchFactory.optimize(TerrainDark, true);
			if (TerrainDark.getQuantity() > 0) {
				DarkBuildGeometry[i] = (Geometry) TerrainDark.getChild(0);
				DarkBuildGeometry[i].setName("DarkGeometry Cell " + BuildCell.toString() + "DetailLevel " + i);
				TerrainDark.detachAllChildren();
			}
		}

		app.enqueue(new Callable() {
			public Object call() throws Exception {
				for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i ++) {
					CellLight.detachChildNamed("LightGeometry Cell " + BuildCell.toString() + "DetailLevel " + i);
					if (LightBuildGeometry[i] != null) {
						CellLight.attachChild(LightBuildGeometry[i]);
						if (i == DetailLevel) {
							LightBuildGeometry[i].setCullHint(Spatial.CullHint.Dynamic);
						} else {
							LightBuildGeometry[i].setCullHint(Spatial.CullHint.Always);							
						}
					}

					CellDark.detachChildNamed("DarkGeometry Cell " + BuildCell.toString() + "DetailLevel " + i);
					if (DarkBuildGeometry[i] != null) {
						CellDark.attachChild(DarkBuildGeometry[i]);
						if (i == DetailLevel) {
							DarkBuildGeometry[i].setCullHint(Spatial.CullHint.Dynamic);
						} else {
							DarkBuildGeometry[i].setCullHint(Spatial.CullHint.Always);							
						}					
					}
				}
				return null;
			}
		});
		return null;
	}
}
