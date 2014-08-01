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
import com.jme3.scene.Spatial;
import com.jme3.material.Material;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.Callable;
import jme3tools.optimize.GeometryBatchFactory;

/**
 *
 * @author Impaler
 */
public class TerrainBuilder implements Callable<Void>  {
	
	Cell BuildCell;
	TileBuilder TileSource;
	Application app;
	
	Node TerrainLight, TerrainDark, ZLight, ZDark;
	Material TerrainMaterial;

	public TerrainBuilder(Application Parentapp, Cell TargetCell, TileBuilder Tiles, Material mat) {
		this.app = Parentapp;
		this.BuildCell = TargetCell;
		this.TileSource = Tiles;
		this.TerrainMaterial = mat;
	}

	public void setNodes(Node Celllight, Node Celldark, Node Light, Node Dark) {
		this.TerrainLight = Celllight;
		this.TerrainDark = Celldark;
		this.ZLight = Light;
		this.ZDark = Dark;
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

		GeometryBatchFactory.optimize(TerrainLight);
		GeometryBatchFactory.optimize(TerrainDark);
		
		app.enqueue(new Callable() {
			public Object call() throws Exception {	
				ZLight.attachChild(TerrainLight);
				ZDark.attachChild(TerrainDark);
				return null;
			}
		});
		return null;
	}
}
