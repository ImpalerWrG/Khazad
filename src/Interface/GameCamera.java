/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Interface;

import com.jme3.export.JmeExporter;
import com.jme3.export.JmeImporter;
import com.jme3.input.InputManager;

import com.jme3.scene.control.CameraControl.ControlDirection;

import com.jme3.math.Ray;
import com.jme3.math.FastMath;
import com.jme3.math.Vector3f;
import com.jme3.math.Vector2f;
import com.jme3.math.Quaternion;
import com.jme3.renderer.Camera;
import com.jme3.scene.Node;
import com.jme3.scene.CameraNode;
import java.io.IOException;

/**
 * A camera 
 * @author Impaler
 */
public class GameCamera {
		
	protected Node TargetNode = null;
	protected Node RotationNode = null;
	protected Node PitchNode = null;

	protected CameraNode CamNode = null;
	
	protected float zoomFactor = 20;
	protected float TranslationFactor;

	protected float zoomSpeed = 0.1f;
	protected float rotationSpeed = 10.0f;
	protected float PitchSpeed = 200.0f;
	
	private Camera camera = null;
	private InputManager inputManager;

	private float PitchAngle;
	private boolean PitchLock;

	protected int SliceTop;
	protected int SliceBottom;
	protected int ViewLevels;
	
	protected int ViewMax, ViewMin;

	public GameCamera(Camera cam, final Node target) {

		this.camera = cam;

		float aspect = (float) camera.getWidth() / camera.getHeight();
		TranslationFactor = zoomFactor / camera.getWidth() * aspect * 2;

		camera.setParallelProjection(true);
		camera.setFrustum(-1000, 1000, -aspect * zoomFactor, aspect * zoomFactor, zoomFactor, -zoomFactor);

		this.TargetNode = target;
		this.camera.setAxes(Vector3f.UNIT_Y, Vector3f.UNIT_Z, Vector3f.UNIT_X);
		
		PitchLock = false;

		this.CamNode = new CameraNode("CamNode", this.camera);
		CamNode.setControlDir(ControlDirection.SpatialToCamera);
		this.RotationNode = new Node("RotateNode");
		this.PitchNode = new Node("PitchNode");
		
		TargetNode.attachChild(RotationNode);
		RotationNode.attachChild(PitchNode);
		PitchNode.attachChild(CamNode);
		
		CamNode.setLocalTranslation(0, 0, 10);
		PitchAngle = 45;
		PitchCamera(0);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);	
	}
	
	//rotate the camera around the target on the horizontal plane
	protected void PitchCamera(float value) {
		
		if (PitchAngle < 80 && value > 0) { // Allow Pitch to increese if not above maximum
			PitchAngle += value * PitchSpeed;
		} else if (PitchAngle > 1 && value < 0) {// Allow Pitch to decreese if not below minimum
			PitchAngle += value * PitchSpeed;
		}
			
		Quaternion pitch = new Quaternion(); 
		pitch.fromAngleAxis( FastMath.PI * PitchAngle / 180  , new Vector3f(1,0,0) ); 
		PitchNode.setLocalRotation(pitch);

		/*
		if (!PitchLock) {
			//float OriginalPitch = PitchNode.getLocalRotation().getY();	
			//Matrix3f OriginalPitch = PitchNode.getLocalRotation().toRotationMatrix();
			
			//float angle = 90 * FastMath.DEG_TO_RAD;

			//PitchNode.getLocalRotation().fromAngleAxis(value, Vector3f.ZERO);
	
			if (OriginalPitch > 0.5 && value > 0)  // Allow Pitch to increese if not above maximum
			{
				PitchNode.rotate(0, value * rotationSpeed, 0);
			}
			else if (OriginalPitch < 0.01 && value < 0) // Allow Pitch to decreese if not below minimum
			{
				PitchNode.rotate(0, value * rotationSpeed, 0);
			}
		}*/
		
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);	
	}

	//expand or contract frustrum for paralax zooming
	protected void zoomCamera(float value) {
		float change = (value * zoomSpeed) + 1;	
		if ((zoomFactor < 200 && change > 1) || (zoomFactor > 5 && change < 1))
		{
			float aspect = (float) camera.getWidth() / camera.getHeight();
			
			zoomFactor *= change;
			TranslationFactor = zoomFactor / camera.getWidth() * aspect * 2;

			float left = -zoomFactor * aspect;
			float right = zoomFactor * aspect;
			float top = zoomFactor;
			float bottom = -zoomFactor;

			camera.setFrustum(-1000, 1000, left, right, top, bottom);
		}
	}

	//rotate the camera around the target on the Horizonatal XY plane
	protected void RotateCamera(float value) {
		RotationNode.rotate(0, 0, value * rotationSpeed);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);		
	}

	protected void TranslateCamera(Vector3f Translation) {
		TargetNode.move(Translation);
	}

	float getShading(int Zlevel) {
		final float Minimum = 0.3f;

		if (Zlevel <= SliceTop)
		{
			float Depth = SliceTop - Zlevel;
			if (Depth < ViewLevels)
			{
				float Shading = 1.0f;
				if (Depth > 0) // Below look level
				{
					Shading -= Depth / (float) ViewLevels;

					Shading = ((1.0f - Minimum) * Shading) + (Minimum);
					return Shading;
				}
				return Shading;
			}
			return Minimum;
		}
		return Minimum;
	}

	public Ray getMouseRay(Vector2f click2d) {
		Vector3f click3d = camera.getWorldCoordinates(new Vector2f(click2d.x, click2d.y), 0f).clone();
		Vector3f dir = camera.getWorldCoordinates(new Vector2f(click2d.x, click2d.y), 1f).subtractLocal(click3d).normalizeLocal();
		return new Ray(click3d, dir);		
	}

	public void write(JmeExporter ex) throws IOException {

	}

	public void read(JmeImporter im) throws IOException {

	}
}
