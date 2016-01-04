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

import com.jme3.export.JmeExporter;
import com.jme3.export.JmeImporter;

import com.jme3.scene.control.CameraControl.ControlDirection;
import com.jme3.bounding.BoundingBox;

import com.jme3.math.Plane;
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
 *
 * @author Impaler
 */
public class GameCamera {

	// Nodes used to rotate and move the Camera
	protected Node TargetNode = null;
	protected Node RotationNode = null;
	protected Node PitchNode = null;
	protected CameraNode CamNode = null;
	// Factor values determining speed of most Camera changes
	protected final float flatzoom = 400;
	protected final float maxzoom = 400;
	protected float zoomFactor = 20;
	protected final float minzoom = 5;
	protected float TranslationFactor;
	protected final float zoomSpeed = 0.1f;
	protected final float rotationSpeed = 10.0f;
	protected final float PitchSpeed = 200.0f;
	// The JMonkey Camera object
	protected Camera camera = null;
	// Current Angles and values for slicing

	// Pitch, the angle of the Camera relative to the XY plane
	private float UserPitchAngle;
	private float ModifiedPitchAngle;
	private boolean PitchLock;
	private float MaxPitch = 80;
	private float MinPitch = .1f;

	private int SliceTop;
	private int SliceBottom;
	private int ViewLevels;
	protected int ViewMax, ViewMin;

	protected final int FrustumNear = -500;
	protected final int FrustumFar = 1000;

	public GameCamera(Camera cam, int SliceLevel, final Node target) {

		this.camera = cam;

		float aspect = (float) camera.getWidth() / camera.getHeight();
		TranslationFactor = zoomFactor / camera.getWidth() * aspect * 2;

		camera.setParallelProjection(true);
		camera.setFrustum(FrustumNear * zoomFactor, FrustumFar * zoomFactor, -aspect * zoomFactor, aspect * zoomFactor, zoomFactor, -zoomFactor);
		this.SliceTop = SliceLevel;

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
		UserPitchAngle = 45;
		pitchCamera(0);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
	}

	//rotate the camera around the target on the horizontal plane
	protected void pitchCamera(float value) {
		if (UserPitchAngle < MaxPitch && value > 0) { // Allow Pitch to increese if not above maximum
			UserPitchAngle += value * PitchSpeed;
		} else if (UserPitchAngle > MinPitch && value < 0) {// Allow Pitch to decreese if not below minimum
			UserPitchAngle += value * PitchSpeed;
		}
		updatePitch();
	}
	
	protected void updatePitch() {
		float flatzoomfraction = (flatzoom - zoomFactor) / flatzoom;
		this.ModifiedPitchAngle = UserPitchAngle * flatzoomfraction;

		Quaternion pitch = new Quaternion();
		pitch.fromAngleAxis(FastMath.PI * this.ModifiedPitchAngle / 180, new Vector3f(1, 0, 0));
		PitchNode.setLocalRotation(pitch);

		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
		//setClipPlane();
	}

	//expand or contract frustrum for paralax zooming
	protected void zoomCamera(float value) {
		float change = (value * zoomSpeed) + 1;
		if ((zoomFactor < maxzoom && change > 1) || (zoomFactor > minzoom && change < 1)) {

			if (change > 0) {
				zoomFactor *= change;
				zoomFactor = Math.min(zoomFactor, maxzoom);
				zoomFactor = Math.max(zoomFactor, minzoom);
				
				frustrumReset();
				updatePitch();
				//setClipPlane();
			}
		}
	}

	protected void frustrumReset() {
		float aspect = (float) camera.getWidth() / camera.getHeight();
		TranslationFactor = zoomFactor / camera.getWidth() * aspect * 2;

		float left = -zoomFactor * aspect;
		float right = zoomFactor * aspect;
		float top = zoomFactor;
		float bottom = -zoomFactor;

		camera.setFrustum(FrustumNear * zoomFactor, FrustumFar * zoomFactor, left, right, top, bottom);	
	}

	protected void setClipPlane() {
		float SliceTopLevel = SliceTop;

		Plane SlicingTopPlane = new Plane(Vector3f.UNIT_Z, 0);
		SlicingTopPlane.setConstant(SliceTopLevel + 0.499f);
		camera.setClipPlane(SlicingTopPlane, Plane.Side.Negative);	
	}

	//rotate the camera around the target on the Horizonatal XY plane
	protected void rotateCamera(float value) {
		RotationNode.rotate(0, 0, value * rotationSpeed);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
	}

	protected void translateCamera(Vector3f Translation) {
		TargetNode.move(Translation);
	}

	protected void pointCameraAt(Vector3f target) {
		TargetNode.setLocalTranslation(target);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
		this.camera.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
	}

	float getShading(int Zlevel) {
		final float Minimum = 0.3f;

		if (Zlevel <= SliceTop) {
			float Depth = SliceTop - Zlevel;
			if (Depth < ViewLevels) {
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

	public void setSlice(int Top, int Bottom) {
		SliceTop = Top;
		SliceBottom = Bottom;
		Vector3f NewLocation = TargetNode.getLocalTranslation();
		NewLocation.z = Top;
		TargetNode.setLocalTranslation(NewLocation);
	}

	public Ray getMouseRay(Vector2f click2d) {
		Vector3f click3d = camera.getWorldCoordinates(new Vector2f(click2d.x, click2d.y), 0f).clone();
		Vector3f dir = camera.getWorldCoordinates(new Vector2f(click2d.x, click2d.y), 1f).subtractLocal(click3d).normalizeLocal();
		return new Ray(click3d, dir);
	}

	public boolean contains(BoundingBox Box) {
		camera.setPlaneState(0);
		Camera.FrustumIntersect results = camera.contains(Box);
		if (results == Camera.FrustumIntersect.Outside)
			return false;
		else {
			return true;
		}
	}

	public void write(JmeExporter ex) throws IOException {
	}

	public void read(JmeImporter im) throws IOException {
	}
}
