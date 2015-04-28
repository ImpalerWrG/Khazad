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
import Map.Face;
import Map.FaceCoordinate;
import Map.TileBuilder;

import Renderer.TextureManager.TextureAtlasCoordinates;

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
	Geometry LightBuildGeometry, DarkBuildGeometry;
	LodControl Controler;

	public TerrainBuilder(Application Parentapp, Cell TargetCell, TileBuilder Tiles, LodControl controler) {
		this.app = Parentapp;
		this.BuildCell = TargetCell;
		this.TileSource = Tiles;
		this.Controler = controler;
	}

	public void setNodes(Node LightCellNode, Node DarkCellNode) {
		this.CellLight = LightCellNode;
		this.CellDark = DarkCellNode;
		TerrainLight = new Node("TerrainLight");
		TerrainDark = new Node("TerrainDark");
	}

	public Void call() {
		TextureManager Texturing = TextureManager.getTextureManager();

		// Terrain Faces
		HashMap<FaceCoordinate, Face> faces = BuildCell.getFaces(0);
		Iterator<Map.Entry<FaceCoordinate, Face>> entries = faces.entrySet().iterator();
		while (entries.hasNext()) {
			Map.Entry<FaceCoordinate, Face> entry = entries.next();

			FaceCoordinate coords = entry.getKey();
			Face targetface = entry.getValue();

			TextureAtlasCoordinates AtlasCoords = Texturing.getTextureCoordinates(targetface.getFaceMaterialType(), targetface.getFaceSurfaceType());
			Mesh facemesh = TileSource.getMesh(targetface.getFaceShapeType(), AtlasCoords);
			if (facemesh != null) {
				Geometry geom = new Geometry("face", facemesh);
				geom.scale(1.001f);  //T-Cell junction hack
				geom.setLocalTranslation(new Vector3f(coords.getX(), coords.getY(), 0));
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
			LightBuildGeometry = (Geometry) TerrainLight.getChild(0);

			//LodGenerator lod = new LodGenerator(LightBuildGeometry);
			//lod.bakeLods(LodGenerator.TriangleReductionMethod.PROPORTIONAL, 0.75f, 0.93f);
			//LightBuildGeometry.addControl(new LodControl());

			LightBuildGeometry.setName("LightGeometry Cell" + BuildCell.toString());
		}
		GeometryBatchFactory.optimize(TerrainDark, true);
		if (TerrainDark.getQuantity() > 0) {
			DarkBuildGeometry = (Geometry) TerrainDark.getChild(0);

			//LodGenerator lod = new LodGenerator(DarkBuildGeometry);
			//lod.bakeLods(LodGenerator.TriangleReductionMethod.PROPORTIONAL, 0.75f, 0.93f);
			//DarkBuildGeometry.addControl(new LodControl());

			DarkBuildGeometry.setName("DarkGeometry Cell" + BuildCell.toString());
		}

		app.enqueue(new Callable() {
			public Object call() throws Exception {
				CellLight.detachChildNamed("LightGeometry Cell" + BuildCell.toString());
				if (LightBuildGeometry != null)
					CellLight.attachChild(LightBuildGeometry);
				CellLight.setCullHint(Spatial.CullHint.Dynamic);

				CellDark.detachChildNamed("DarkGeometry Cell" + BuildCell.toString());
				if (DarkBuildGeometry != null)
					CellDark.attachChild(DarkBuildGeometry);
				CellDark.setCullHint(Spatial.CullHint.Dynamic);

				return null;
			}
		});
		return null;
	}
}
