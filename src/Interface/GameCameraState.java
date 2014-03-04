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
import com.jme3.collision.CollisionResult;
import com.jme3.font.BitmapFont;
import com.jme3.font.BitmapText;
import com.jme3.material.Material;
import com.jme3.scene.Geometry;
import com.jme3.math.ColorRGBA;

import com.jme3.scene.Node;
import com.jme3.math.Vector3f;
import com.jme3.scene.Mesh;
import com.jme3.scene.VertexBuffer;
import com.jme3.scene.shape.Box;
import com.jme3.texture.Texture;
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
 *  Manages the main game Isometric Camera 
 *
 *  @author    Impaler
 */
public class GameCameraState extends AbstractAppState implements ActionListener, AnalogListener {

	private Node rootnode;
	private Node terrainnode;

    private SimpleApplication app;
    private GameCamera MainCamera;
	private AppStateManager state; 

	BitmapText hudText;
	private Geometry box;
		
	private boolean LeftDown;
	private boolean RightDown;
	private boolean MiddleDown;

	private float OldMouseX;
	private float OldMouseY;
	private float XChange;
	private float YChange;

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
				Node LookNode = new Node();
				this.app.getRootNode().attachChild(LookNode);
				LookNode.setLocalTranslation(1, 2, 3);
				
				Box box1 = new Box(0.5f, 0.5f, 0.5f);
				Geometry blue = new Geometry("Box", box1);
				blue.setLocalTranslation(new Vector3f(0, 0, 0));
				Material mat1 = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
				mat1.setColor("Color", ColorRGBA.Blue);
				blue.setMaterial(mat1);
				LookNode.attachChild(blue);
				
                MainCamera = new GameCamera(app.getCamera(), LookNode);
            }
            //MainCamera.registerWithInput(app.getInputManager());            
        }
		registerWithInput(app.getInputManager());

		
		BuildBox();
		BuildText();
    }
	
	public void BuildBox() {
		
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
		
		box = new Geometry("Camera Mouse Box", WireBoxMesh);		
		box.setMaterial(mark_mat);
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
					//MoveBoxCursor();
				}
			}
			if (name.equals("RightClick")) {
				RightDown = keyPressed;
				if (RightDown) {
					//TargetNode.setCullHint(Spatial.CullHint.Never);					
				} else {
					//TargetNode.setCullHint(Spatial.CullHint.Always);										
				}
			}
			if (name.equals("MiddleClick")) {
				MiddleDown = keyPressed;
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
        }
    }

    public void onAnalog(String name, float value, float tpf) {
		
		ConvertMouseMovementToVector();
		updateMousePosition();
			
        if (name.equals("mouseLeft")) {
			if (LeftDown && RightDown) {
				MainCamera.RotateCamera(value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("mouseRight")) {
			if (LeftDown && RightDown) {
				MainCamera.RotateCamera(-value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("Up")) {
			if (LeftDown && RightDown) {
				MainCamera.PitchCamera(value);
			}else{
				if (RightDown)
					MainCamera.TranslateCamera(CreateTranslationVector());
			}
        } else if (name.equals("Down")) {
			if (LeftDown && RightDown) {
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
        String[] inputs = {"LeftClick", "RightClick", "MiddleClick", "Down", "Up", "mouseLeft", "mouseRight", "ZoomIn", "ZoomOut", "ArrowUp", "ArrowDown"};

        //this.inputManager = inputManager;

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
			hudText.setText("X: " + x + "  Y: " + y + "  Z: " + z);

			box.setLocalTranslation(new Vector3f(x, y, z));
			this.rootnode.attachChild(box);
        } else {
			this.rootnode.detachChild(box);
        }
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

