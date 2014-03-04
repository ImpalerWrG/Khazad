package Core;

import Interface.GameCameraState;
import com.jme3.app.Application;
import com.jme3.app.StatsAppState;
import com.jme3.app.SimpleApplication;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;

import Nifty.*;
import Interface.*;
import Game.*;
import Renderer.*;
import Map.*;
import PathFinding.PathManager;
import Sound.Music;

import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.VertexBuffer;
import com.jme3.util.BufferUtils;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import javax.management.timer.Timer;

/**
 * test
 * @author Impaler
 */
public class Main extends SimpleApplication {
	
	TerrainRenderer render = null;

	static ScheduledThreadPoolExecutor ExecutionThreadpool;

    public static void main(String[] args) {	
		Main Game = new Main();
        Game.start();
    }

	public Main() {
		super(new ShellInputState(), new StatsAppState());
	}

    @Override
    public void simpleInitApp() {

		ExecutionThreadpool = new ScheduledThreadPoolExecutor(8);

		ImageManager Images = ImageManager.getImageManager();
		Images.Initialize(assetManager);
		
		// GAME
		Game game = new Game();
		game.InitializeGame((short) 10, (short) 10, "Khazad");
		this.stateManager.attach(game);
		
		// TEMPORAL
		this.stateManager.attach(TemporalManager.getSingleton());
		
		// PATHING
		PathManager Pather = PathManager.getSinglton();
		Pather.initialize(stateManager, this);
		Pather.CreateMapAbstraction(game.getMap());
		Pather.AllocateThreadPool(ExecutionThreadpool);
		this.stateManager.attach(Pather);
		
		// RENDER
		render = new TerrainRenderer();
		this.stateManager.attach(render);
		
		this.stateManager.attach(new GameCameraState());
		this.stateManager.attach(new Music());
		
		//createAxialMarker();
		
		GUI gui = GUI.getGUI();
		gui.InitializeGUI(this);

		for (int i = 0; i < 1000; i++) {
			game.SpawnPawn(Pather.Tester.getRandomPassableCoordinate());
		}
	}
	
	public void createAxialMarker() {
		Mesh AxialMarker = new Mesh();
		AxialMarker.setMode(Mesh.Mode.Lines);
		AxialMarker.setLineWidth(3);
		
		Vector3f [] vertices = new Vector3f[7];
		vertices[0] = new Vector3f(0, 0, 0);
		
		vertices[1] = new Vector3f(0,  0,  1);
		vertices[2] = new Vector3f(0,  0,  -1);
		vertices[3] = new Vector3f(0,  1,  0);
		vertices[4] = new Vector3f(0,  -1,  0);
		vertices[5] = new Vector3f(1,  0,  0);
		vertices[6] = new Vector3f(-1,  0,  0);
		
		int [] indexes = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, };
		
		AxialMarker.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
		AxialMarker.setBuffer(VertexBuffer.Type.Index,    3, BufferUtils.createIntBuffer(indexes));
		AxialMarker.updateBound();

		Geometry box = new Geometry("AxisMarker", AxialMarker);
		Material mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
		mat.setColor("Color", ColorRGBA.Green);
		box.setMaterial(mat);
		box.setLocalTranslation(9, 14, 0);
		rootNode.attachChild(box);
	}

    @Override
    public void simpleUpdate(float tpf) {
		
    }

    @Override
    public void simpleRender(RenderManager rm) {
        //TODO: add render code
    }
	
	@Override
    public void destroy() {
        super.destroy();
        ExecutionThreadpool.shutdown();
    }
}
