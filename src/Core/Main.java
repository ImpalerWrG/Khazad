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

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Simple Initialization of gamestates that run just around everything else
 * A Threadpool is used for speeding up pathfinding and rendering
 * @author Impaler
 */
public class Main extends SimpleApplication {

	static ExecutorService pool;

    public static void main(String[] args) {	
		Main Game = new Main();

		AppSettings setting= new AppSettings(true);
		setting.setTitle("Khazad");
		setting.setHeight(600);
		setting.setWidth(800);
		//Game.setShowSettings(false); 
		Game.setSettings(setting);

        Game.start();
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
		pool = Executors.newFixedThreadPool(8);

		DataManager Data = DataManager.getDataManager();
		Data.Initialize();

		ImageManager Images = ImageManager.getImageManager();
		Images.Initialize(assetManager);

		//TextureManager Tex = TextureManager.getTextureManager();
		//Tex.Initialize(assetManager);

		// RENDER
		this.stateManager.attach(new MapRenderer());
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

	public ExecutorService getThreadPool() {
		return pool;
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
        pool.shutdown();
    }
}
