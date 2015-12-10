/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Interface;

import com.jme3.asset.AssetManager;
import com.jme3.math.Plane;
import com.jme3.math.Vector3f;
import com.jme3.post.SceneProcessor;
import com.jme3.renderer.RenderManager;
import com.jme3.renderer.ViewPort;
import com.jme3.renderer.queue.RenderQueue;
import com.jme3.texture.FrameBuffer;
import com.jme3.renderer.Camera;
/**
 *
 * @author Impaler
 */
public class TerrainSlicer implements SceneProcessor {

	protected AssetManager manager;
    protected RenderManager rm;
	protected ViewPort vp;
	protected GameCamera Cam;
	protected Plane SlicingPlane;
		
	public TerrainSlicer(AssetManager manager) {
		this.manager = manager;
		SlicingPlane = new Plane(Vector3f.UNIT_Z, 0);
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
		float SliceLevel = this.Cam.SliceTop;
		this.SlicingPlane.setConstant(SliceLevel + 0.499f);
		this.Cam.camera.setClipPlane(SlicingPlane, Plane.Side.Negative);		
    }

	public void postFrame(FrameBuffer out) {
    }

	public void reshape(ViewPort vp, int w, int h) {
	}

    public void cleanup() {
    }	
}
