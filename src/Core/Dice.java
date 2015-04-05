/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Core;

import java.io.Serializable;
import java.util.Random;

/**
 *
 * @author Impaler
 */
public class Dice implements Serializable {

	private static final long serialVersionUID = 1;
	public Random Generator;

	public Dice() {
		Generator = new Random();
		seed(0);
	}

	public final void seed(long Seed) {
		Generator.setSeed(Seed);
	}

	public int roll(int Min, int Max) {
		return Generator.nextInt(Max - Min + 1) + Min;
	}

	public float roll(float Min, float Max) {
		return Generator.nextFloat() * (Max - Min) + Min;
	}
}
