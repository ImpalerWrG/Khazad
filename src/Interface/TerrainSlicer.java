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

package Interface;

import com.jme3.asset.AssetManager;
import com.jme3.math.Plane;
import com.jme3.math.Vector3f;
import com.jme3.post.SceneProcessor;
import com.jme3.renderer.RenderManager;
import com.jme3.renderer.ViewPort;
import com.jme3.renderer.queue.RenderQueue;
import com.jme3.texture.FrameBuffer;

/**
 *
 * @author Impaler
 */
public class TerrainSlicer implements SceneProcessor {

	protected AssetManager manager;
	protected RenderManager rm;
	protected ViewPort vp;
	protected GameCamera Cam;
	protected Plane SlicingTopPlane, SlicingBottomPlane;

	public TerrainSlicer(AssetManager manager) {
		this.manager = manager;
		SlicingTopPlane = new Plane(Vector3f.UNIT_Z, 0);
		SlicingBottomPlane = new Plane(Vector3f.UNIT_Z, 0);
	}

	public void initialize(RenderManager rm, ViewPort vp) {
		this.rm = rm;
		this.vp = vp;
	}

	public void setCamera(GameCamera Camera) {
		this.Cam = Camera;
	}

	public boolean isInitialized() {
		return rm != null;
	}

	public void postQueue(RenderQueue rq) {
	}

	public void preFrame(float tpf) {
		// Render actors first?
		this.Cam.setClipPlane();
	}

	public void postFrame(FrameBuffer out) {
	}

	public void reshape(ViewPort vp, int w, int h) {
	}

	public void cleanup() {
	}
}
