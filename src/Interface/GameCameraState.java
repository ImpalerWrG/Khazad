/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Interface;


import Game.Game;
import Job.JobManager;
import Job.ExcavateJob;
import Map.MapCoordinate;
import Map.CubeShape;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.font.BitmapFont;
import com.jme3.font.BitmapText;
import com.jme3.material.Material;
import com.jme3.scene.Geometry;
import com.jme3.math.ColorRGBA;

import com.jme3.scene.Node;
import com.jme3.math.Vector3f;
import com.jme3.math.Plane;
import com.jme3.math.Ray;
import com.jme3.math.Vector2f;
import com.jme3.scene.Mesh;
import com.jme3.scene.VertexBuffer;
import com.jme3.scene.shape.Sphere;
import com.jme3.util.BufferUtils;

import com.jme3.collision.CollisionResult;
import com.jme3.collision.CollisionResults;
import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.MouseInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.AnalogListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.MouseAxisTrigger;
import com.jme3.input.controls.MouseButtonTrigger;
import com.jme3.scene.Spatial;

import Renderer.TerrainRenderer;


/**
 *  Manages the main games parrelel projection Camera 
 *
 *  @author    Impaler
 */
public class GameCameraState extends AbstractAppState implements ActionListener, AnalogListener {

	public enum CameraMode {
		NORMAL,
		SELECT_VOLUME,
		SELECTING_VOLUME,
		SELECT_SURFACE,
		SELECTING_SURFACE
	}

	private Node rootnode;
	private Node terrainnode;
	private Node LookNode;

    private SimpleApplication app;
    private GameCamera MainCamera;
	private AppStateManager state; 

	BitmapText hudText;
	private Geometry CursorBox;
	private Geometry SelectionBox;

	private CameraMode CurrentMode = CameraMode.NORMAL;

	private boolean LeftDown;
	private boolean RightDown;
	private boolean MiddleDown;
	private boolean LShiftDown;
	private boolean RShiftDown;
	private boolean Shift;

	private float OldMouseX;
	private float OldMouseY;
	private float XChange;
	private float YChange;
	
	private Plane SelectionPlane = null;

	private MapCoordinate MouseLocation = new MapCoordinate();
	private MapCoordinate SelectionOrigin = new MapCoordinate();
	private MapCoordinate SelectionTerminus = new MapCoordinate();

	public VolumeSelection Volume;

    public GameCameraState() {
    }

    @Override
    public void initialize(AppStateManager stateManager, Application app) {
        super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.rootnode = this.app.getRootNode();

		TerrainRenderer rend = stateManager.getState(TerrainRenderer.class);
		this.terrainnode = rend.getTerrainNode();
				
        if (app.getInputManager() != null) {
       
            if (MainCamera == null) {
				LookNode = new Node();
				this.app.getRootNode().attachChild(LookNode);
				LookNode.setLocalTranslation(1, 2, 3);
				
				Sphere eye = new Sphere(10, 10, 0.5f);
				Geometry blue = new Geometry("EyeBall", eye);
				blue.setLocalTranslation(new Vector3f(0, 0, 0));
				Material mat1 = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
				mat1.setColor("Color", ColorRGBA.White);
				blue.setMaterial(mat1);
				LookNode.attachChild(blue);
				LookNode.setCullHint(Spatial.CullHint.Always);										
				
                MainCamera = new GameCamera(app.getCamera(), LookNode);
            }
        }
		registerWithInput(app.getInputManager());

		
		BuildCursorBox();
		BuildSelectionBox();
		BuildText();
    }
	
	public void BuildCursorBox() {
		
		Material mark_mat = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
		mark_mat.setColor("Color", ColorRGBA.Red);
		
		Mesh WireBoxMesh = new Mesh();
		WireBoxMesh.setMode(Mesh.Mode.Lines);
		WireBoxMesh.setLineWidth(3);
		
		Vector3f [] vertices = new Vector3f[8];
		vertices[0] = new Vector3f( MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE);
		vertices[1] = new Vector3f(-MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE);
		vertices[2] = new Vector3f(-MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE);
		vertices[3] = new Vector3f( MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE);
		
		vertices[4] = new Vector3f( MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		vertices[5] = new Vector3f(-MapCoordinate.HALFCUBE,  MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		vertices[6] = new Vector3f(-MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		vertices[7] = new Vector3f( MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		
		int [] indexes = { 0,1, 1,2, 2,3, 3,0, 0,4, 1,5, 2,6, 3,7, 4,5, 5,6, 6,7, 7,4 };
		
		WireBoxMesh.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
		WireBoxMesh.setBuffer(VertexBuffer.Type.Index,    3, BufferUtils.createIntBuffer(indexes));
		WireBoxMesh.updateBound();
		
		CursorBox = new Geometry("Camera Mouse Box", WireBoxMesh);		
		CursorBox.setMaterial(mark_mat);
	}

	public void BuildSelectionBox() {
		Material mark_mat = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
		mark_mat.setColor("Color", ColorRGBA.Green);
		
		Mesh WireBoxMesh = new Mesh();
		WireBoxMesh.setMode(Mesh.Mode.Lines);
		WireBoxMesh.setLineWidth(3);
		
		int maxX = Math.max(SelectionTerminus.X, SelectionOrigin.X);
		int maxY = Math.max(SelectionTerminus.Y, SelectionOrigin.Y);
		int maxZ = Math.max(SelectionTerminus.Z, SelectionOrigin.Z);

		int minX = Math.min(SelectionTerminus.X, SelectionOrigin.X);
		int minY = Math.min(SelectionTerminus.Y, SelectionOrigin.Y);
		int minZ = Math.min(SelectionTerminus.Z, SelectionOrigin.Z);

		Vector3f [] vertices = new Vector3f[8];
		vertices[0] = new Vector3f(maxX - minX + MapCoordinate.HALFCUBE, maxY - minY + MapCoordinate.HALFCUBE,  maxZ - minZ + MapCoordinate.HALFCUBE);
		vertices[1] = new Vector3f(-MapCoordinate.HALFCUBE,  maxY - minY + MapCoordinate.HALFCUBE,  maxZ - minZ + MapCoordinate.HALFCUBE);
		vertices[2] = new Vector3f(-MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE,  maxZ - minZ + MapCoordinate.HALFCUBE);
		vertices[3] = new Vector3f(maxX - minX + MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE,  maxZ - minZ + MapCoordinate.HALFCUBE);
		
		vertices[4] = new Vector3f(maxX - minX +  MapCoordinate.HALFCUBE, maxY - minY + MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		vertices[5] = new Vector3f(-MapCoordinate.HALFCUBE,  maxY - minY + MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		vertices[6] = new Vector3f(-MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		vertices[7] = new Vector3f(maxX - minX +  MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE, -MapCoordinate.HALFCUBE);
		
		int [] indexes = { 0,1, 1,2, 2,3, 3,0, 0,4, 1,5, 2,6, 3,7, 4,5, 5,6, 6,7, 7,4 };
		
		WireBoxMesh.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
		WireBoxMesh.setBuffer(VertexBuffer.Type.Index,    3, BufferUtils.createIntBuffer(indexes));
		WireBoxMesh.updateBound();
		
		SelectionBox = new Geometry("Camera Mouse Box", WireBoxMesh);		
		SelectionBox.setMaterial(mark_mat);		
	}

	public void BuildText() {
		BitmapFont font = this.app.getAssetManager().loadFont("Interface/Fonts/Default.fnt");
		
		hudText = new BitmapText(font, false);          
		hudText.setSize(font.getCharSet().getRenderedSize());      // font size
		hudText.setColor(ColorRGBA.Blue);                             // font color
		hudText.setText("You can write any string here");             // the text
		hudText.setLocalTranslation(300, hudText.getLineHeight(), 0); // position
		this.app.getGuiNode().attachChild(hudText);
	}

	protected void ConvertMouseMovementToVector() {
		Vector2f Mouse = app.getInputManager().getCursorPosition();
		
		XChange = OldMouseX - Mouse.x;
		YChange = OldMouseY - Mouse.y;
		
		OldMouseX = Mouse.x;
		OldMouseY = Mouse.y;
	}

	protected Vector3f CreateTranslationVector() {
		Vector3f LookVector = MainCamera.TargetNode.getWorldTranslation().subtract(MainCamera.CamNode.getWorldTranslation());

		LookVector.normalizeLocal();
		float zComp = LookVector.z;
		LookVector.z = 0;
		LookVector.normalizeLocal();

		Vector3f TempUpVector = Vector3f.UNIT_Z;

		Vector3f CrossProduct = TempUpVector.cross(LookVector);
		CrossProduct.normalizeLocal();

		Vector3f Temp1 = CrossProduct.mult(-XChange).mult(MainCamera.TranslationFactor);  //
		Vector3f Temp2 = LookVector.mult(-YChange).mult(MainCamera.TranslationFactor).divide(zComp);

		XChange = YChange = 0; // Consume the Mouse movement
		return Temp1.add(Temp2);
	}

    public void onAction(String name, boolean keyPressed, float tpf) {
        if (this.isEnabled()) {
			if (name.equals("LeftClick")) {
				LeftDown = keyPressed;
				//if (!RightDown && keyPressed) {
				//}
				
				if (CurrentMode == CameraMode.SELECTING_VOLUME && !keyPressed)
					completeVolumeSelection();
					
				if (CurrentMode == CameraMode.SELECT_VOLUME && keyPressed)
					setMode(CameraMode.SELECTING_VOLUME);
			}
			
			if (name.equals("RightClick")) {
				RightDown = keyPressed;

				if (CurrentMode == CameraMode.SELECT_VOLUME) {
					setMode(CameraMode.NORMAL);
				}
				if (CurrentMode == CameraMode.SELECTING_VOLUME) {
					setMode(CameraMode.NORMAL);
				}
			}

			if (name.equals("MiddleClick")) {
				MiddleDown = keyPressed;
				if (MiddleDown) {
					LookNode.setCullHint(Spatial.CullHint.Never);					
				} else {
					LookNode.setCullHint(Spatial.CullHint.Always);										
				}
			}
			
			if (name.equals("ArrowUp") && keyPressed) {
				MainCamera.ChangeViewLevel(1);
				TerrainRenderer rend = state.getState(TerrainRenderer.class);
				rend.setSliceLevels(MainCamera.SliceTop, MainCamera.SliceBottom);
			}
			if (name.equals("ArrowDown") && keyPressed) {
				MainCamera.ChangeViewLevel(-1);
				TerrainRenderer rend = state.getState(TerrainRenderer.class);
				rend.setSliceLevels(MainCamera.SliceTop, MainCamera.SliceBottom);
			}
			
			if (name.equals("RShift") && keyPressed) {
				RShiftDown = keyPressed;
				Shift = RShiftDown || LShiftDown;
			}

			if (name.equals("LShift") && keyPressed) {
				LShiftDown = keyPressed;
				Shift = RShiftDown || LShiftDown;
			}

        }
    }

	private void AnalogNormal(String name, float value, float tpf) {
		ConvertMouseMovementToVector();
		updateMousePosition();
			
        if (name.equals("mouseLeft")) {
			if (MiddleDown) {
				MainCamera.RotateCamera(value);
			} else {
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());				
			}
        } else if (name.equals("mouseRight")) {
			if (MiddleDown) {
				MainCamera.RotateCamera(-value);
			} else {
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("mouseUp")) {
			if (MiddleDown) {
				MainCamera.PitchCamera(value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("mouseDown")) {
			if (MiddleDown) {
				MainCamera.PitchCamera(-value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("ZoomIn")) {
            MainCamera.zoomCamera(value);
        } else if (name.equals("ZoomOut")) {
            MainCamera.zoomCamera(-value);
        }		
	}
	
	private void AnalogSelectingVolume(String name, float value, float tpf) {
		ConvertMouseMovementToVector();

		if (Shift) { // Z axis stretching
			Vector3f LookVector = MainCamera.TargetNode.getWorldTranslation().subtract(MainCamera.CamNode.getWorldTranslation());

			LookVector.normalizeLocal();
			float zComp = LookVector.z;
			LookVector.z = 0;
			LookVector.normalizeLocal();

			//Vector3f TempUpVector = Vector3f.UNIT_Z;

			//Vector3f CrossProduct = TempUpVector.cross(LookVector);
			//CrossProduct.normalizeLocal();

			//Vector3f Temp1 = CrossProduct.mult(-XChange).mult(MainCamera.TranslationFactor);  //
			Vector3f Temp2 = LookVector.mult(-YChange).mult(MainCamera.TranslationFactor).divide(zComp);
			
		} else { // XY plane intersection
			Ray ray = MainCamera.getMouseRay(app.getInputManager().getCursorPosition());
			Vector3f IntersectLocation = new Vector3f();
			ray.intersectsWherePlane(SelectionPlane, IntersectLocation);

			SelectionTerminus.Set((int) IntersectLocation.x, (int) IntersectLocation.y, (int) SelectionOrigin.Z);

			this.rootnode.detachChild(CursorBox);
			this.rootnode.detachChild(SelectionBox);					
			BuildSelectionBox();

			int minX = Math.min((int) SelectionTerminus.X, SelectionOrigin.X);
			int minY = Math.min((int) SelectionTerminus.Y, SelectionOrigin.Y);
			int minZ = Math.min((int) SelectionTerminus.Z, SelectionOrigin.Z);

			SelectionBox.setLocalTranslation(new Vector3f(minX, minY, minZ));
			this.rootnode.attachChild(SelectionBox);
		}

        if (name.equals("mouseLeft")) {
			if (MiddleDown) {
				MainCamera.RotateCamera(value);
			} else {
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());				
			}
        } else if (name.equals("mouseRight")) {
			if (MiddleDown) {
				MainCamera.RotateCamera(-value);
			} else {
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("mouseUp")) {
			if (MiddleDown) {
				MainCamera.PitchCamera(value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("mouseDown")) {
			if (MiddleDown) {
				MainCamera.PitchCamera(-value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("ZoomIn")) {
            MainCamera.zoomCamera(value);
        } else if (name.equals("ZoomOut")) {
            MainCamera.zoomCamera(-value);
        }		
		
	}

    public void onAnalog(String name, float value, float tpf) {
		switch (CurrentMode) {
			case NORMAL:
				AnalogNormal(name, value, tpf);
				break;
				
			case SELECT_VOLUME:
				AnalogNormal(name, value, tpf);
				break;

			case SELECTING_VOLUME:
				AnalogSelectingVolume(name, value, tpf);
				break;
		}
    }

    public void registerWithInput(InputManager inputManager) {
        String[] inputs = {"LeftClick", "RightClick", "MiddleClick", "mouseDown", "mouseUp", "mouseLeft", "mouseRight", "ZoomIn", "ZoomOut", "ArrowUp", "ArrowDown", "RShift", "LShift"};

        inputManager.addMapping("mouseDown", new MouseAxisTrigger(1, true));
        inputManager.addMapping("mouseUp", new MouseAxisTrigger(1, false));
        inputManager.addMapping("ZoomIn", new MouseAxisTrigger(2, true));
        inputManager.addMapping("ZoomOut", new MouseAxisTrigger(2, false));
        inputManager.addMapping("mouseLeft", new MouseAxisTrigger(0, true));
        inputManager.addMapping("mouseRight", new MouseAxisTrigger(0, false));
        
		inputManager.addMapping("LeftClick", new MouseButtonTrigger(MouseInput.BUTTON_LEFT));
        inputManager.addMapping("RightClick", new MouseButtonTrigger(MouseInput.BUTTON_RIGHT));
        inputManager.addMapping("MiddleClick", new MouseButtonTrigger(MouseInput.BUTTON_MIDDLE));

        inputManager.addMapping("ArrowUp", new KeyTrigger(KeyInput.KEY_UP));
        inputManager.addMapping("ArrowDown", new KeyTrigger(KeyInput.KEY_DOWN));
        inputManager.addMapping("RShift", new KeyTrigger(KeyInput.KEY_RSHIFT));
        inputManager.addMapping("LShift", new KeyTrigger(KeyInput.KEY_LSHIFT));
		
        inputManager.addListener(this, inputs);
    }

	public void unregisterInput(InputManager inputManager) {
		
	}
	
	public void updateMousePosition() {
		TerrainRenderer rend = this.app.getStateManager().getState(TerrainRenderer.class);
		this.terrainnode = rend.getTerrainNode();

		if (terrainnode != null) {
			
			CollisionResults results = new CollisionResults();		
			terrainnode.collideWith(MainCamera.getMouseRay(app.getInputManager().getCursorPosition()), results);

			if (results.size() > 0) {
				// The closest collision point is what was truly hit:
				CollisionResult closest = results.getClosestCollision();

				Vector3f contact = closest.getContactPoint();
				Vector3f normal = closest.getContactNormal();
				contact = contact.subtract(normal.mult(.001f));

				int x = Math.round(contact.getX());
				int y = Math.round(contact.getY());
				int z = Math.round(contact.getZ());
				MouseLocation.Set(x, y, z);
				hudText.setText("X: " + x + "  Y: " + y + "  Z: " + z);

				CursorBox.setLocalTranslation(new Vector3f(x, y, z));
				this.rootnode.attachChild(CursorBox);
			}
		}
	}

	public void setMode(CameraMode newMode) {
		if (CurrentMode != newMode) {
			CurrentMode = newMode;
			
			if (CurrentMode == CameraMode.SELECTING_VOLUME) {
				SelectionOrigin.copy(MouseLocation);
				SelectionPlane = new Plane(Vector3f.UNIT_Z, MouseLocation.Z);
				this.rootnode.detachChild(SelectionBox);	
			}
			
			if (CurrentMode == CameraMode.NORMAL) {
				SelectionPlane = null;
			}
		}
	}
	
	public void completeVolumeSelection() {

		Volume = new VolumeSelection();
		int maxX = Math.max(SelectionTerminus.X, SelectionOrigin.X);
		int maxY = Math.max(SelectionTerminus.Y, SelectionOrigin.Y);
		int maxZ = Math.max(SelectionTerminus.Z, SelectionOrigin.Z);

		int minX = Math.min(SelectionTerminus.X, SelectionOrigin.X);
		int minY = Math.min(SelectionTerminus.Y, SelectionOrigin.Y);
		int minZ = Math.min(SelectionTerminus.Z, SelectionOrigin.Z);

		Volume.OriginLocation.Set(minX, minY, minZ);
		Volume.TerminalLocation.Set(maxX, maxY, maxZ);

		Game game = state.getState(Game.class);
		JobManager jobs = game.getSettlment().getJobManager();
		ExcavateJob newJob = new ExcavateJob();
		newJob.addDesignations(Volume, new CubeShape(CubeShape.CUBE_BOTTOM_HEIGHT));
		jobs.addJob(newJob);

		setMode(CameraMode.SELECT_VOLUME);
	}

    @Override
	public void update(float tpf) {

	}

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled); 
    }
   
    @Override
    public void cleanup() {
        super.cleanup();

        unregisterInput(app.getInputManager());        
    }
}

