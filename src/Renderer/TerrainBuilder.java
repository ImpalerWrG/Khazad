/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Renderer;

import com.jme3.app.Application;

import Map.Cell;
import Map.CellCoordinate;
import Map.Face;
import Map.FaceCoordinate;
import Map.TileBuilder;

import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.Node;
import com.jme3.scene.control.LodControl;
import com.jme3.material.Material;
import jme3tools.optimize.GeometryBatchFactory;
import jme3tools.optimize.LodGenerator;

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
public class TerrainBuilder implements Callable<Void>  {
	
	Cell BuildCell;
	TileBuilder TileSource;
	Application app;
	
	Node TerrainLight, TerrainDark, CellLight, CellDark;
	Geometry LightBuildGeometry, DarkBuildGeometry;
	Material TerrainMaterial;
	LodControl Controler;

	public TerrainBuilder(Application Parentapp, Cell TargetCell, TileBuilder Tiles, Material mat, LodControl controler) {
		this.app = Parentapp;
		this.BuildCell = TargetCell;
		this.TileSource = Tiles;
		this.TerrainMaterial = mat;
		this.Controler = controler;
	}

	public void setNodes(Node LightCellNode, Node DarkCellNode) {
		this.CellLight = LightCellNode;
		this.CellDark = DarkCellNode;
		TerrainLight = new Node();
		TerrainDark = new Node();
	}

	public Void call() {
		CellCoordinate Coords = BuildCell.getCellCoordinates();

		// Terrain Faces
		HashMap<FaceCoordinate, Face> faces = BuildCell.getFaces();
		Iterator<Map.Entry<FaceCoordinate, Face>> entries = faces.entrySet().iterator();
		while (entries.hasNext()) {
			Map.Entry<FaceCoordinate, Face> entry = entries.next();
			
			FaceCoordinate coords = entry.getKey();
			Face targetface = entry.getValue();
			
			Mesh facemesh = TileSource.getMesh(targetface.getFaceShapeType());
			if (facemesh != null) {
				Geometry geom = new Geometry("face", facemesh);
				geom.setLocalTranslation(new Vector3f(coords.getX(), coords.getY(), 0));
				geom.setMaterial(TerrainMaterial);
				
				//Cell   coords.Coordinates
				if (true /*sunlit face*/) {
					TerrainLight.attachChild(geom);
				} else {
					TerrainDark.attachChild(geom);
				}
			}
		}

		GeometryBatchFactory.optimize(TerrainLight, true);
		if (TerrainLight.getQuantity() > 0) {
			LightBuildGeometry = (Geometry) TerrainLight.getChild(0);
			
			LodGenerator lod = new LodGenerator(LightBuildGeometry);
			lod.bakeLods(LodGenerator.TriangleReductionMethod.PROPORTIONAL, 0.5f, 0.75f);
			LightBuildGeometry.addControl(new LodControl());
			
			LightBuildGeometry.setName("LightGeometry Cell" + BuildCell.toString());
		}
		GeometryBatchFactory.optimize(TerrainDark, true);
		if (TerrainDark.getQuantity() > 0) {
			DarkBuildGeometry = (Geometry) TerrainDark.getChild(0);

			LodGenerator lod = new LodGenerator(DarkBuildGeometry);
			lod.bakeLods(LodGenerator.TriangleReductionMethod.PROPORTIONAL, 0.5f, 0.75f);
			DarkBuildGeometry.addControl(new LodControl());

			DarkBuildGeometry.setName("DarkGeometry Cell" + BuildCell.toString());
		}
		
		app.enqueue(new Callable() {
			public Object call() throws Exception {
				CellLight.detachChildNamed("LightGeometry Cell" + BuildCell.toString());
				if (LightBuildGeometry != null)
					CellLight.attachChild(LightBuildGeometry);
				
				CellDark.detachChildNamed("DarkGeometry Cell" + BuildCell.toString());
				if (DarkBuildGeometry != null)
					CellDark.attachChild(DarkBuildGeometry);
				
				return null;
			}
		});
		return null;
	}
}
