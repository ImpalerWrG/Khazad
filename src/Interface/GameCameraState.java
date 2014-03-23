/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Interface;


import Map.MapCoordinate;
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
import com.jme3.math.Vector2f;

/**
 *  Manages the main games parrelel projection Camera 
 *
 *  @author    Impaler
 */
public class GameCameraState extends AbstractAppState implements ActionListener, AnalogListener {

	private Node rootnode;
	private Node terrainnode;
	private Node LookNode;

    private SimpleApplication app;
    private GameCamera MainCamera;
	private AppStateManager state; 

	BitmapText hudText;
	private Geometry CursorBox;
	private Geometry SelectionBox;
		
	private boolean LeftDown;
	private boolean RightDown;
	private boolean MiddleDown;

	private float OldMouseX;
	private float OldMouseY;
	private float XChange;
	private float YChange;

	private boolean SelectingVolume = false;
	private boolean SelectingSurface = false;

	private MapCoordinate MouseLocation = new MapCoordinate();
	private MapCoordinate SelectionOrigin = new MapCoordinate();

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
		
		int maxX = Math.max(MouseLocation.X, SelectionOrigin.X);
		int maxY = Math.max(MouseLocation.Y, SelectionOrigin.Y);
		int maxZ = Math.max(MouseLocation.Z, SelectionOrigin.Z);

		int minX = Math.min(MouseLocation.X, SelectionOrigin.X);
		int minY = Math.min(MouseLocation.Y, SelectionOrigin.Y);
		int minZ = Math.min(MouseLocation.Z, SelectionOrigin.Z);

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
				if (!RightDown && keyPressed) {
				}
			}
			if (name.equals("RightClick")) {
				RightDown = keyPressed;
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
			
			if (name.equals("SelectionStart") && keyPressed) {
				beginVolumeSelection();
			}
			
			if (name.equals("SelectionEnd") && keyPressed) {
				endVolumeSelection();
			}
        }
    }

    public void onAnalog(String name, float value, float tpf) {
		
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
        } else if (name.equals("Up")) {
			if (MiddleDown) {
				MainCamera.PitchCamera(value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("Down")) {
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

    public void registerWithInput(InputManager inputManager) {
        String[] inputs = {"LeftClick", "RightClick", "MiddleClick", "Down", "Up", "mouseLeft", "mouseRight", "ZoomIn", "ZoomOut", "ArrowUp", "ArrowDown", "SelectionStart", "SelectionEnd"};

        inputManager.addMapping("Down", new MouseAxisTrigger(1, true));
        inputManager.addMapping("Up", new MouseAxisTrigger(1, false));
        inputManager.addMapping("ZoomIn", new MouseAxisTrigger(2, true));
        inputManager.addMapping("ZoomOut", new MouseAxisTrigger(2, false));
        inputManager.addMapping("mouseLeft", new MouseAxisTrigger(0, true));
        inputManager.addMapping("mouseRight", new MouseAxisTrigger(0, false));
        
		inputManager.addMapping("LeftClick", new MouseButtonTrigger(MouseInput.BUTTON_LEFT));
        inputManager.addMapping("RightClick", new MouseButtonTrigger(MouseInput.BUTTON_RIGHT));
        inputManager.addMapping("MiddleClick", new MouseButtonTrigger(MouseInput.BUTTON_MIDDLE));

        inputManager.addMapping("ArrowUp", new KeyTrigger(KeyInput.KEY_UP));
        inputManager.addMapping("ArrowDown", new KeyTrigger(KeyInput.KEY_DOWN));

		inputManager.addMapping("SelectionStart", new KeyTrigger(KeyInput.KEY_S));
		inputManager.addMapping("SelectionEnd", new KeyTrigger(KeyInput.KEY_E));

        inputManager.addListener(this, inputs);
    }

	public void unregisterInput(InputManager inputManager) {
		
	}
	
	public void updateMousePosition() {
		CollisionResults results = MainCamera.CursorCollision(terrainnode, app.getInputManager().getCursorPosition());
		
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
			
			if (SelectingVolume) {
				this.rootnode.detachChild(CursorBox);
				this.rootnode.detachChild(SelectionBox);					
				BuildSelectionBox();

				int minX = Math.min(MouseLocation.X, SelectionOrigin.X);
				int minY = Math.min(MouseLocation.Y, SelectionOrigin.Y);
				int minZ = Math.min(MouseLocation.Z, SelectionOrigin.Z);

				SelectionBox.setLocalTranslation(new Vector3f(minX, minY, minZ));
				this.rootnode.attachChild(SelectionBox);
			} else {
				this.rootnode.detachChild(SelectionBox);	
			}
        } else {
			this.rootnode.detachChild(CursorBox);
        }
	}
	
	public void beginVolumeSelection() {
		SelectingVolume = true;
		SelectionOrigin.copy(MouseLocation);		
	}
	
	public void endVolumeSelection() {
		SelectingVolume = false;

		Volume = new VolumeSelection();
		int maxX = Math.max(MouseLocation.X, SelectionOrigin.X);
		int maxY = Math.max(MouseLocation.Y, SelectionOrigin.Y);
		int maxZ = Math.max(MouseLocation.Z, SelectionOrigin.Z);

		int minX = Math.min(MouseLocation.X, SelectionOrigin.X);
		int minY = Math.min(MouseLocation.Y, SelectionOrigin.Y);
		int minZ = Math.min(MouseLocation.Z, SelectionOrigin.Z);

		Volume.OriginLocation.Set(minX, minY, minZ);
		Volume.TerminalLocation.Set(maxX, maxY, maxZ);
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

