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

package Core;

import com.jme3.app.StatsAppState;
import com.jme3.app.SimpleApplication;
import com.jme3.system.AppSettings;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;

import Nifty.*;
import Renderer.*;
import Sound.Music;
import Data.DataManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;

import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.VertexBuffer;
import com.jme3.util.BufferUtils;

import com.jme3.app.state.ScreenshotAppState;
import com.jme3.asset.plugins.FileLocator;
import com.jme3.input.KeyInput;
import com.jme3.input.MouseInput;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.MouseAxisTrigger;
import com.jme3.input.controls.MouseButtonTrigger;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Simple Initialization of gamestates that run just abount everything else
 * A Threadpool is used for speeding up pathfinding and rendering
 *
 * @author Impaler
 */
public class Main extends SimpleApplication {

	private static ExecutorService pool;
	public static Main app;
	
	public boolean Focus;

	public static void main(String[] args) {
		app = new Main();

		Thread.currentThread().setPriority(Thread.MAX_PRIORITY);

		AppSettings setting = new AppSettings(true);
		setting.setTitle("Khazad");
		setting.setHeight(600);
		setting.setWidth(800);
		setting.setFrameRate(60);

		app.setSettings(setting);
		app.start();
	}

	public Main() {
		super(new StatsAppState());
	}

	@Override
	public void simpleInitApp() {

		try {
			File file = new File("Khazad Error.txt");
			FileOutputStream fos = new FileOutputStream(file);
			PrintStream ps = new PrintStream(fos);
			System.setErr(ps);
			//throw new Exception("Exception goes to err.txt too");
		} catch (Exception e) {
			e.printStackTrace();
		}
		app.assetManager.registerLocator("assets/", FileLocator.class);
		app.setPauseOnLostFocus(false);

		pool = Executors.newFixedThreadPool(8);

		DataManager Data = DataManager.getDataManager();
		Data.initialize();

		ImageManager Images = ImageManager.getImageManager();
		Images.initialize(assetManager);

		TextureManager Tex = TextureManager.getTextureManager();
		Tex.initialize(assetManager);

		initialiseKeyboardMappings();

		// RENDER
		this.stateManager.attach(new MapRenderer(pool));
		this.stateManager.attach(new TerrainRenderer(pool));
		this.stateManager.attach(new PathingRenderer());
		this.stateManager.attach(new ActorRenderer());

		this.stateManager.attach(new Music());
		this.stateManager.attach(new GUI(this));

		this.stateManager.attach(new ScreenshotAppState(new String()));
	}

	public void createAxialMarker() {
		Mesh AxialMarker = new Mesh();
		AxialMarker.setMode(Mesh.Mode.Lines);
		AxialMarker.setLineWidth(3);

		Vector3f[] vertices = new Vector3f[7];
		vertices[0] = new Vector3f(0, 0, 0);

		vertices[1] = new Vector3f(0, 0, 1);
		vertices[2] = new Vector3f(0, 0, -1);
		vertices[3] = new Vector3f(0, 1, 0);
		vertices[4] = new Vector3f(0, -1, 0);
		vertices[5] = new Vector3f(1, 0, 0);
		vertices[6] = new Vector3f(-1, 0, 0);

		int[] indexes = {0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6,};

		AxialMarker.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
		AxialMarker.setBuffer(VertexBuffer.Type.Index, 3, BufferUtils.createIntBuffer(indexes));
		AxialMarker.updateBound();

		Geometry box = new Geometry("AxisMarker", AxialMarker);
		Material mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
		mat.setColor("Color", ColorRGBA.Green);
		box.setMaterial(mat);
		box.setLocalTranslation(9, 14, 0);
		rootNode.attachChild(box);
	}

	public ExecutorService getThreadPool() {
		return pool;
	}

	@Override
	public void simpleUpdate(float tpf) {
	}

	@Override
	public void simpleRender(RenderManager rm) {
	}

	@Override
	public void gainFocus() {
		Focus = true;
		super.gainFocus();
	}

	@Override
	public void loseFocus() {
		Focus = false;
		super.loseFocus();
	}

	@Override
	public void destroy() {
		super.destroy();
		pool.shutdown();
	}
	
	public void initialiseKeyboardMappings() {
		// TODO move this to some key mappings UI

		// used in GameCameraState
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
		inputManager.addMapping("ArrowUp", new KeyTrigger(KeyInput.KEY_E));
		inputManager.addMapping("ArrowDown", new KeyTrigger(KeyInput.KEY_DOWN));
		inputManager.addMapping("ArrowDown", new KeyTrigger(KeyInput.KEY_Q));

		inputManager.addMapping("PanUp", new KeyTrigger(KeyInput.KEY_W));
		inputManager.addMapping("PanLeft", new KeyTrigger(KeyInput.KEY_A));
		inputManager.addMapping("PanDown", new KeyTrigger(KeyInput.KEY_S));
		inputManager.addMapping("PanRight", new KeyTrigger(KeyInput.KEY_D));

		inputManager.addMapping("RShift", new KeyTrigger(KeyInput.KEY_RSHIFT));
		inputManager.addMapping("LShift", new KeyTrigger(KeyInput.KEY_LSHIFT));

		// used in Game
		inputManager.addMapping("Pause", new KeyTrigger(KeyInput.KEY_SPACE));
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_ADD));
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_EQUALS));
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_MINUS));
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_SUBTRACT));

		// used in GameScreenController
		inputManager.addMapping("PathingRenderToggle", new KeyTrigger(KeyInput.KEY_P));
		inputManager.addMapping("TerrainRenderToggle", new KeyTrigger(KeyInput.KEY_T));
		inputManager.addMapping("SunnyRenderToggle", new KeyTrigger(KeyInput.KEY_L));
		inputManager.addMapping("ReduceDetailLevel", new KeyTrigger(KeyInput.KEY_Y));
		inputManager.addMapping("IncreeseDetailLevel", new KeyTrigger(KeyInput.KEY_U));	
	}
}
