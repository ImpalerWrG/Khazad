/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import com.jme3.light.DirectionalLight;
import com.jme3.math.ColorRGBA;
import com.jme3.scene.Node;
import com.jme3.math.Vector3f;
import com.jme3.math.FastMath;
import com.jme3.math.Quaternion;
/**
 *
 * @author Impaler
 */
public class Weather extends Temporal {
	
	DirectionalLight Sun;
	Quaternion Rotation;
	Vector3f SunVec;
	Node SunNode;
	ColorRGBA Suncolor = ColorRGBA.White;

	public Weather() {
		Rotation = new Quaternion();
		Rotation.fromAngleAxis(1 * FastMath.PI / 180f, new Vector3f(0, 1, 0));
		
		SunNode = new Node();
		SunVec = new Vector3f();
		
		Sun = new DirectionalLight();
		Sun.setColor(Suncolor.mult(0.6f));
	}
	
	public void AttatchSun(Node TerrainNode) {
		SunVec.set(0, 0, -1);
		SunVec.normalizeLocal();
		Sun.setDirection(SunVec);

		TerrainNode.addLight(Sun);
	}
	
	public long Wake(long CurrentTick) {		
		if (CurrentTick >= WakeTick) {
			
			// Day Rotation
			SunVec = Rotation.mult(SunVec);
			Sun.setDirection(SunVec);
			float Z = SunVec.z;
			if (Z > 0) {
				Sun.setColor(Suncolor.mult(0.0f));
			} else {
				Sun.setColor(Suncolor.mult((0.6f) * (Z * -1)));				
			}
			
			WakeTick = CurrentTick + 720;
		}
		return WakeTick;		
	}
}
