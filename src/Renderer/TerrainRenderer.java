/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Renderer;

import Map.*;
import Game.Game;
import Game.Actor;
import Game.Pawn;
import Interface.GameCameraState;

import Nifty.GUI;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;
import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.ActionListener;

import com.jme3.light.AmbientLight;
import com.jme3.light.DirectionalLight;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Quaternion;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;

import com.jme3.scene.control.LodControl;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import com.jme3.math.FastMath;
import com.jme3.texture.Image;
import com.jme3.texture.Texture;
import java.util.ArrayList;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

/**
 * Rendering class for Terrain, tracks all Scene Nodes that Terrain geometry
 * attaches too and rebuilds the geometry when Cells are dirty.  Division of
 * Terrain into light/dark allows easy hiding of surface terrain and restriction
 * of directional sunlight to appropriate surfaces.
 * 
 * @author Impaler
 */
public class TerrainRenderer extends AbstractAppState implements ActionListener {
	
	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	ImageManager imagemanager = null;
	
	Node terrainNode = null;
	Node sunnyterrainNode = null;
	Node darkterrainNode = null;
	
	TileBuilder builder;
	LodControl TerrainLodControler;

	ConcurrentHashMap<CellCoordinate, Node> LightCellNodeMap;
	ConcurrentHashMap<CellCoordinate, Node> DarkCellNodeMap;
	ConcurrentHashMap<Integer, Node> ActorNodeMap;
	ConcurrentHashMap<Integer, Node> ZMapLight;
	ConcurrentHashMap<Integer, Node> ZMapDark;
	Material mat = null;
	
	boolean SunnyRendering = true;
	int Top; int Bottom;

	ExecutorService Executor;
	ArrayList<Future> CellRebuildingFutures;
	boolean DisplayToggle = true;

	public TerrainRenderer(ExecutorService Threadpool) {
		Executor = Threadpool;

		LightCellNodeMap = new ConcurrentHashMap<CellCoordinate, Node>();
		DarkCellNodeMap = new ConcurrentHashMap<CellCoordinate, Node>();
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
		
		registerWithInput(app.getInputManager());
	}
	
	public void attachToGame(Game TargetGame) {
		terrainNode = new Node();
		this.app.getRootNode().attachChild(terrainNode);
		
		darkterrainNode = new Node();
		sunnyterrainNode = new Node();
		terrainNode.attachChild(darkterrainNode);
		terrainNode.attachChild(sunnyterrainNode);

		this.TerrainLodControler = new LodControl();
		//this.TerrainLodControler.setTrisPerPixel(10);

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

	public void onAction(String name, boolean keyPressed, float tpf) {
        if (this.isEnabled()) {
			if (name.equals("TerrainRenderToggle") && keyPressed) {
				DisplayToggle = !DisplayToggle;
			}
        }
    }

	public void registerWithInput(InputManager inputManager) {
        String[] inputs = {"TerrainRenderToggle"};

        inputManager.addMapping("TerrainRenderToggle", new KeyTrigger(KeyInput.KEY_T));		
        inputManager.addListener(this, inputs);
    }

	public Node getCellNodeLight(CellCoordinate TargetCell) {
		Node CellNode = LightCellNodeMap.get(TargetCell);
		if (CellNode == null) {
			CellNode = new Node();	
		
			float x = (float) (TargetCell.X * MapCoordinate.CELLEDGESIZE);
			float y = (float) (TargetCell.Y * MapCoordinate.CELLEDGESIZE);

			CellNode.move(x, y, 0);

			getZNodeLight(TargetCell.Z).attachChild(CellNode);
			LightCellNodeMap.put(TargetCell, CellNode);
		}
		return CellNode;
	}

	public Node getCellNodeDark(CellCoordinate TargetCell) {
		Node CellNode = DarkCellNodeMap.get(TargetCell);
		if (CellNode == null) {
			CellNode = new Node();	
		
			float x = (float) (TargetCell.X * MapCoordinate.CELLEDGESIZE);
			float y = (float) (TargetCell.Y * MapCoordinate.CELLEDGESIZE);

			CellNode.move(x, y, 0);

			getZNodeDark(TargetCell.Z).attachChild(CellNode);
			DarkCellNodeMap.put(TargetCell, CellNode);
		}
		return CellNode;
	}

	private Node getZNodeLight(int zlevel) {
		Node targetnode = ZMapLight.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node();
			targetnode.move(0, 0, zlevel);
			ZMapLight.put(new Integer(zlevel), targetnode);
		}
		return targetnode;
	}

	private Node getZNodeDark(int zlevel) {
		Node targetnode = ZMapDark.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node();
			targetnode.move(0, 0, zlevel);
			ZMapDark.put(new Integer(zlevel), targetnode);
		}
		return targetnode;
	}

	public void RebuildDirtyCells(ConcurrentHashMap<CellCoordinate, Cell> cells) {
		for (Cell target : cells.values()) {
			CellCoordinate Coords = target.getCellCoordinates();

			Node CellLight = getCellNodeLight(Coords);
			Node CellDark = getCellNodeDark(Coords);
			
			Spatial light = CellLight.getChild("LightGeometry Cell" + target.toString());
			Spatial dark = CellDark.getChild("DarkGeometry Cell" + target.toString());
			
			if (light != null)
				light.setCullHint(Spatial.CullHint.Dynamic);
			if (dark != null)
				dark.setCullHint(Spatial.CullHint.Dynamic);
			
			if (target.isTerrainRenderingDirty()) {
				TerrainBuilder Builder = new TerrainBuilder(app, target, builder, mat, TerrainLodControler);
				Builder.setNodes(getCellNodeLight(Coords), getCellNodeDark(Coords));
				Executor.submit(Builder);

				target.setDirtyTerrainRendering(false);
				break;
			}
		}	
	}

	public void HideTerrain(ConcurrentHashMap<CellCoordinate, Cell> cells) {
		for (Cell target : cells.values()) {
			CellCoordinate Coords = target.getCellCoordinates();

			Node CellLight = getCellNodeLight(Coords);
			Node CellDark = getCellNodeDark(Coords);
			
			Spatial light = CellLight.getChild("LightGeometry Cell" + target.toString());
			Spatial dark = CellDark.getChild("DarkGeometry Cell" + target.toString());

			if (light != null)
				light.setCullHint(Spatial.CullHint.Always);
			if (dark != null)
				dark.setCullHint(Spatial.CullHint.Always);
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

					actorNode.setCullHint(Spatial.CullHint.Dynamic);
					MapCoordinate coords = target.getLocation();
					Node z = getZNodeLight(coords.Z);
					z.attachChild(actorNode);
					Vector3f Offset = new Vector3f();
					
					if (target instanceof Pawn) {
						Pawn PawnTarget = (Pawn) target;
						float MoveFraction = PawnTarget.getActionFraction(CurrentTick);
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
							if (MoveFraction >= 1.0) {
								MoveFraction = 0;
							}							
							
							MapCoordinate translated = new MapCoordinate(coords, MovingDirection);
							CubeShape shape = map.getCubeShape(translated);
							float CenterHeight = shape.centerHeight() + (translated.Z - coords.Z);							
							float EdgeHeight = shape.DirectionEdgeHeight(MovingDirection.Invert()) + (translated.Z - coords.Z);
							float CenterFraction = ((MoveFraction - 0.5f) * 2.0f);
							float EdgeFraction = 1.0f - CenterFraction;
							Height = (CenterHeight * CenterFraction) + (EdgeHeight * EdgeFraction);						
						}
						
						if (MovingDirection == Direction.DIRECTION_DESTINATION) {
							MoveFraction = 0;
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
	
	public void HideActors() {
		for (Node target : ActorNodeMap.values()) {
			target.setCullHint(Spatial.CullHint.Always);
		}
	}

	public void setSliceLevels(int top, int bottom) {
		Top = top; Bottom = bottom;
		
		for (Node targetnode : ZMapLight.values()) {
			float Z = targetnode.getLocalTranslation().getZ();
			if (Z <= Top && SunnyRendering) {
				sunnyterrainNode.attachChild(targetnode);
			} else {
				sunnyterrainNode.detachChild(targetnode);
			}
		}
		
		for (Node targetnode : ZMapDark.values()) {
			float Z = targetnode.getLocalTranslation().getZ();
			if (Z <= Top) {
				darkterrainNode.attachChild(targetnode);
			} else {
				darkterrainNode.detachChild(targetnode);
			}
		}
	}
	
	public void setSunnyVisibility(boolean newValue) {
		if (SunnyRendering != newValue) {
			SunnyRendering = newValue;
		
			for (Node target : ZMapLight.values()) {
				float Z = target.getLocalTranslation().getZ();
				if (Z > Bottom && Z <= Top && SunnyRendering) {
					target.setCullHint(Spatial.CullHint.Dynamic);
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
			if (DisplayToggle) {
				RebuildDirtyCells(map.getCellMap());
			} else {
				HideTerrain(map.getCellMap());
			}
			if (game.getTickRate() <= 256) {
				PopulateActors();	
			} else {
				HideActors();
			}
			GameCameraState cam = state.getState(GameCameraState.class);
			setSliceLevels(cam.getSliceTop(), cam.getSliceBottom());

			
			GUI gui = state.getState(GUI.class);
			String TimeString = ((game.hours %24) + ":" + (game.minutes % 60) + ":" + (game.seconds % 60));
			gui.UpdateText("Timelabel", TimeString);
		}
	}
}

