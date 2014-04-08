/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Renderer;

import Map.*;
import Game.Game;
import Game.Actor;
import Game.Pawn;

import Nifty.GUI;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;
import com.jme3.light.AmbientLight;
import com.jme3.light.DirectionalLight;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Quaternion;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import com.jme3.math.FastMath;
import com.jme3.texture.Image;
import com.jme3.texture.Texture;
import java.util.ArrayList;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledThreadPoolExecutor;

/**
 *
 * @author Impaler
 */
public class TerrainRenderer extends AbstractAppState {
	
	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	ImageManager imagemanager = null;
	
	Node terrainNode = null;
	Node sunnyterrainNode = null;
	Node darkterrainNode = null;
	
	TileBuilder builder;
	ConcurrentHashMap<CellCoordinate, Node> CellNodeMap;
	ConcurrentHashMap<Integer, Node> ActorNodeMap;
	ConcurrentHashMap<Integer, Node> ZMapLight;
	ConcurrentHashMap<Integer, Node> ZMapDark;
	Material mat = null;
	
	boolean SunnyRendering = true;
	int Top; int Bottom;

	ScheduledThreadPoolExecutor Executor;
	ArrayList<Future> CellRebuildingFutures;

	public TerrainRenderer(ScheduledThreadPoolExecutor Threadpool) {
		Executor = Threadpool;

		CellNodeMap = new ConcurrentHashMap<CellCoordinate, Node>();
		ActorNodeMap = new ConcurrentHashMap<Integer, Node>();
		ZMapLight = new ConcurrentHashMap<Integer, Node>();
		ZMapDark = new ConcurrentHashMap<Integer, Node>();
		builder = new TileBuilder();
	}

    @Override
    public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();

		ImageManager Images = ImageManager.getImageManager();
		Images.Initialize(assetmanager);
		imagemanager = Images;
	}
	
	public void attachToGame(Game TargetGame) {
		terrainNode = new Node();
		this.app.getRootNode().attachChild(terrainNode);
		
		darkterrainNode = new Node();
		sunnyterrainNode = new Node();
		terrainNode.attachChild(darkterrainNode);
		terrainNode.attachChild(sunnyterrainNode);

		Texture tex = assetmanager.loadTexture("Textures/grass1.png");
		Image withBorder = imagemanager.GeneratedOverLayImage(tex.getImage(), 0, 0);
		tex.setImage(withBorder);

		mat = new Material(assetmanager, "Common/MatDefs/Light/Lighting.j3md");
		mat.setTexture("DiffuseMap", tex);
		
		ColorRGBA Suncolor = ColorRGBA.White;
		AmbientLight glow = new AmbientLight();
		glow.setColor(Suncolor.mult(0.8f));

		TargetGame.getWeather().AttatchSun(sunnyterrainNode);
		terrainNode.addLight(glow);		
	}
	
	public void detachFromGame() {
		terrainNode = null;
		darkterrainNode = null;
		sunnyterrainNode = null;
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

	public void RebuildDirtyCells(ConcurrentHashMap<CellCoordinate, Cell> cells) {
		for (Cell target : cells.values()) {
			if (target.isDirty()) {
				CellCoordinate Coords = target.getCellCoordinates();
				Node CellNode = getNode(Coords);
				Node LightNode = (Node) CellNode.getChild("light");
				Node DarkNode = (Node) CellNode.getChild("dark");

				TerrainBuilder Builder = new TerrainBuilder(app, target, builder, mat);
				Builder.setNodes(LightNode, DarkNode, getZNodeLight(Coords.Z), getZNodeDark(Coords.Z));
				Executor.submit(Builder);

				target.setRenderingDirty(false);
				break;
			}
		}	
	}

	public void PopulateActors() {
		Game game = state.getState(Game.class);
		GameMap map = game.getMap();
		
		long CurrentTick = game.getCurrentTimeTick();
		
		ArrayList<Actor> actors = game.getActors();
		for (Actor target : actors)
		{
			if (target != null) {
				if (target.isDirty()) {
					Node actorNode = ActorNodeMap.get(target.getID());
					if (actorNode == null) {
										
						//Geometry = new Sphere();
						Spatial actorModel = assetmanager.loadModel("Models/Dwarf/Dwarf.j3o");
						actorModel.scale(0.25f, 0.25f, 0.25f);
						actorModel.rotate(1.5f, 0.0f, 0.0f);

						actorNode = new Node();
						actorNode.attachChild(actorModel);
						ActorNodeMap.put(new Integer(target.getID()), actorNode);
					}

					MapCoordinate coords = target.getLocation();
					Node z = getZNodeLight(coords.Z);
					z.attachChild(actorNode);
					Vector3f Offset = new Vector3f();
					
					if (target instanceof Pawn) {
						Pawn PawnTarget = (Pawn) target;
						float MoveFraction = PawnTarget.getMovementFraction(CurrentTick);
						Direction MovingDirection = PawnTarget.getMovementDirection();
						float Height = 0;
						
						if (MoveFraction <= 0.5) {
							CubeShape shape = map.getCubeShape(coords);
							float CenterHeight = shape.centerHeight();
							float EdgeHeight = shape.DirectionEdgeHeight(MovingDirection);
							float CenterFraction = (MoveFraction * 2.0f);
							float EdgeFraction = 1.0f - CenterFraction;
							Height = (CenterHeight * CenterFraction) + (EdgeHeight * EdgeFraction);						
						}
						
						if (MoveFraction > 0.5) {
							MapCoordinate translated = new MapCoordinate(coords, MovingDirection);
							CubeShape shape = map.getCubeShape(translated);
							float CenterHeight = shape.centerHeight() + (translated.Z - coords.Z);							
							float EdgeHeight = shape.DirectionEdgeHeight(MovingDirection.Invert()) + (translated.Z - coords.Z);
							float CenterFraction = ((MoveFraction - 0.5f) * 2.0f);
							float EdgeFraction = 1.0f - CenterFraction;
							Height = (CenterHeight * CenterFraction) + (EdgeHeight * EdgeFraction);						
							
							if (MoveFraction == 1.0) {
								MoveFraction = 0;
							}
						}
							
						Vector3f MoveVec = MovingDirection.toVector();
						Offset.addLocal(MoveVec.mult(MoveFraction));
						
						Quaternion rotation = actorNode.getLocalRotation();
						rotation.fromAngleAxis(MovingDirection.toDegree() * FastMath.DEG_TO_RAD, Vector3f.UNIT_Z);
						actorNode.setLocalRotation(rotation);
												
						actorNode.setLocalTranslation(coords.X + Offset.x, coords.Y + Offset.y, Height);
						
					} else {
						actorNode.setLocalTranslation(coords.X, coords.Y, 0);
					}					
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

	public Node getSunTerrainNode() {
		return sunnyterrainNode;
	}

	public Node getDarkTerrainNode() {
		return darkterrainNode;
	}

	@Override
	public void update(float tpf) {		
		Game game = state.getState(Game.class);
		if (game != null) {
			GameMap map = game.getMap();
			RebuildDirtyCells(map.getCellMap());
			//if (game.getTickRate() < 64) {
				PopulateActors();	
			//}
			
			GUI gui = state.getState(GUI.class);
			String TimeString = ((game.hours %24) + ":" + (game.minutes % 60) + ":" + (game.seconds % 60));
			gui.UpdateText("Timelabel", TimeString);
		}
	}

	@Override
    public void render(RenderManager rm) {
    }

	@Override
    public void postRender(){
    }
}

