/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Renderer;

import Map.*;
import Game.*;
import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;
import com.jme3.light.AmbientLight;
import com.jme3.light.DirectionalLight;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;
import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.scene.shape.Box;
import com.jme3.texture.Image;
import com.jme3.texture.Texture;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import jme3tools.optimize.GeometryBatchFactory;
/**
 *
 * @author Impaler
 */
public class TerrainRenderer extends AbstractAppState {
	
	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	ImageManager imagemanager = null;
	
	Node terrainNode;
	Node sunnyterrainNode;
	Node darkterrainNode;
	
	TileBuilder builder;
	HashMap<CellCoordinate, Node> CellNodeMap;
	HashMap<Integer, Node> ActorNodeMap;
	HashMap<Integer, Node> ZMapLight;
	HashMap<Integer, Node> ZMapDark;
	Material mat = null;
	
	boolean SunnyRendering = true;
	int Top; int Bottom;
	
	public TerrainRenderer() {
		CellNodeMap = new HashMap<CellCoordinate, Node>();
		ActorNodeMap = new HashMap<Integer, Node>();
		ZMapLight = new HashMap<Integer, Node>();
		ZMapDark = new HashMap<Integer, Node>();
		builder = new TileBuilder();
	}

    @Override
    public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.state = stateManager;

		terrainNode = new Node();
		this.app.getRootNode().attachChild(terrainNode);
		
		darkterrainNode = new Node();
		sunnyterrainNode = new Node();
		terrainNode.attachChild(darkterrainNode);
		terrainNode.attachChild(sunnyterrainNode);
		
		assetmanager = app.getAssetManager();
		
		ImageManager Images = ImageManager.getImageManager();
		Images.Initialize(assetmanager);
		imagemanager = Images;
		
		Texture tex = assetmanager.loadTexture("Textures/grass1.png");
		Image withBorder = imagemanager.GeneratedOverLayImage(tex.getImage(), 0, 0);
		tex.setImage(withBorder);

		mat = new Material(assetmanager, "Common/MatDefs/Light/Lighting.j3md");
		mat.setTexture("DiffuseMap", tex);
				
		DirectionalLight sun = new DirectionalLight();
		sun.setDirection(new Vector3f(1, 1, -1).normalizeLocal());
		ColorRGBA Suncolor = ColorRGBA.White;
		sun.setColor(Suncolor.mult(0.6f));
		
		AmbientLight  glow = new AmbientLight();
		glow.setColor(Suncolor.mult(0.8f));
				
		
		sunnyterrainNode.addLight(sun);
		terrainNode.addLight(glow);
	}
	
	private Node getNode(CellCoordinate TargetCell) {
		Node targetnode = CellNodeMap.get(TargetCell);
		if (targetnode != null) {
			terrainNode.detachChild(targetnode);
		}
			
		Node CellNode = new Node();

		float x = (float) (TargetCell.X * MapCoordinate.CELLEDGESIZE);
		float y = (float) (TargetCell.Y * MapCoordinate.CELLEDGESIZE);

		CellNode.move(x, y, 0);
		Node Dark = new Node("dark");
		CellNode.attachChild(Dark);
		Node light = new Node("light");
		CellNode.attachChild(light);
		
		CellNodeMap.put(TargetCell, CellNode);
		return CellNode;
	}
	
	private Node getZNodeLight(int zlevel) {
		Node targetnode = ZMapLight.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node();
			sunnyterrainNode.attachChild(targetnode);
			targetnode.move(0, 0, zlevel);
			ZMapLight.put(new Integer(zlevel), targetnode);
		}
		return targetnode;
	}

	private Node getZNodeDark(int zlevel) {
		Node targetnode = ZMapDark.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node();
			darkterrainNode.attachChild(targetnode);
			targetnode.move(0, 0, zlevel);
			ZMapDark.put(new Integer(zlevel), targetnode);
		}
		return targetnode;
	}

	public void BuildCellRendering(Cell TargetCell) {
		
		CellCoordinate Coords = TargetCell.getCellCoordinates();
		Node CellNode = getNode(Coords);
		
		Node LightNode = (Node) CellNode.getChild("light");
		Node DarkNode = (Node) CellNode.getChild("dark");
		
		// Terrain Faces
		HashMap<FaceCoordinate, Face> faces = TargetCell.getFaces();
		Iterator<Map.Entry<FaceCoordinate, Face>> entries = faces.entrySet().iterator();
		while (entries.hasNext()) {
			Map.Entry<FaceCoordinate, Face> entry = entries.next();
			
			FaceCoordinate coords = entry.getKey();
			Face targetface = entry.getValue();
			
			Mesh facemesh = builder.getMesh(targetface.getFaceShapeType());
			if (facemesh != null) {
				Geometry geom = new Geometry("face", facemesh);
				geom.setLocalTranslation(new Vector3f(coords.getX(), coords.getY(), 0));

				geom.setMaterial(mat);
				
				//Cell   coords.Coordinates
				if (true /*sunlit face*/) {
					LightNode.attachChild(geom);					
				} else {
					DarkNode.attachChild(geom);
				}
			}
		}
		
		// Actors

		Node ZLight = getZNodeLight(Coords.Z);
		ZLight.attachChild(GeometryBatchFactory.optimize(LightNode));
		Node ZDark = getZNodeDark(Coords.Z);	
		ZDark.attachChild(GeometryBatchFactory.optimize(DarkNode));
	}
	
	public void RebuildDirtyCells() {
		Game game = state.getState(Game.class);
		GameMap map = game.getMap();
		
		HashMap<CellCoordinate, Cell> cells = map.getCellMap();
		
		for (Cell target : cells.values()) {
			if (target.isDirty()) {
				BuildCellRendering(target);
				target.setRenderingDirty(false);
			}
		}

		ArrayList<Actor> actors = game.getActors();
		for (Actor target : actors)
		{
			if (target != null) {
				if (target.isDirty()) {
					Node actorNode = ActorNodeMap.get(target.getID());
					if (actorNode == null) {
						//Box box = new Box(0.7f, 0.7f, 0.7f);
						//Spatial actorModel = new Geometry("Box", box );
						//Material mat1 = new Material(assetmanager, "Common/MatDefs/Misc/Unshaded.j3md");
						//mat1.setColor("Color", ColorRGBA.Blue);
						//actorModel.setMaterial(mat1);
						Spatial actorModel = assetmanager.loadModel("Models/Steve Rig/Steve Rig.j3o");
						actorModel.scale(0.25f, 0.25f, 0.25f);
						actorModel.rotate(1.5f, 0.0f, 0.0f);

						actorNode = new Node();
						actorNode.attachChild(actorModel);
						ActorNodeMap.put(new Integer(target.getID()), actorNode);
					}

					MapCoordinate coords = target.getLocation();
					Node z = getZNodeLight(coords.Z);
					z.attachChild(actorNode);
					actorNode.setLocalTranslation(coords.X, coords.Y, 0);
					
				}
			}
		}
	}
	
	public void setSliceLevels(int top, int bottom) {
		Top = top; Bottom = bottom;
		
		for (Node target : ZMapLight.values()) {
			float Z = target.getLocalTranslation().getZ();
			if (Z > Bottom && Z <= Top && SunnyRendering) {
				target.setCullHint(Spatial.CullHint.Never);
			} else {
				target.setCullHint(Spatial.CullHint.Always);				
			}
		}
		
		for (Node target : ZMapDark.values()) {
			float Z = target.getLocalTranslation().getZ();
			if (Z > Bottom && Z <= Top) {
				target.setCullHint(Spatial.CullHint.Never);
			} else {
				target.setCullHint(Spatial.CullHint.Always);				
			}
		}	
	}
	
	public void setSunnyVisibility(boolean newValue) {
		if (SunnyRendering != newValue) {
			SunnyRendering = newValue;
		
			for (Node target : ZMapLight.values()) {
				float Z = target.getLocalTranslation().getZ();
				if (Z > Bottom && Z <= Top && SunnyRendering) {
					target.setCullHint(Spatial.CullHint.Never);
				} else {
					target.setCullHint(Spatial.CullHint.Always);				
				}
			}
		}
	}

	public Node getTerrainNode() {
		return terrainNode;
	}
	
	@Override
    public void update(float tpf) {
		RebuildDirtyCells();
    }

	@Override
    public void render(RenderManager rm) {
    }

	@Override
    public void postRender(){
    }
}

