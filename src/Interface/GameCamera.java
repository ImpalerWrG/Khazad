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
	protected float maxzoom = 400;
	protected float zoomFactor = 20;
	protected float minzoom = 5;
	protected float TranslationFactor;
	protected float zoomSpeed = 0.1f;
	protected float rotationSpeed = 10.0f;
	protected float PitchSpeed = 200.0f;
	// The JMonkey Camera object
	private Camera camera = null;
	// Current Angles and values for slicing
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
		pitchCamera(0);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
	}

	//rotate the camera around the target on the horizontal plane
	protected void pitchCamera(float value) {

		if (PitchAngle < 80 && value > 0) { // Allow Pitch to increese if not above maximum
			PitchAngle += value * PitchSpeed;
		} else if (PitchAngle > 1 && value < 0) {// Allow Pitch to decreese if not below minimum
			PitchAngle += value * PitchSpeed;
		}

		Quaternion pitch = new Quaternion();
		pitch.fromAngleAxis(FastMath.PI * PitchAngle / 180, new Vector3f(1, 0, 0));
		PitchNode.setLocalRotation(pitch);

		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
	}

	//expand or contract frustrum for paralax zooming
	protected void zoomCamera(float value) {
		float change = (value * zoomSpeed) + 1;
		if ((zoomFactor < maxzoom && change > 1) || (zoomFactor > minzoom && change < 1)) {
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
	protected void rotateCamera(float value) {
		RotationNode.rotate(0, 0, value * rotationSpeed);
		CamNode.lookAt(TargetNode.getWorldTranslation(), Vector3f.UNIT_Z);
	}

	protected void translateCamera(Vector3f Translation) {
		TargetNode.move(Translation);
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
