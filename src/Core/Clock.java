/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Core;

import com.jme3.system.Timer;
import com.jme3.system.lwjgl.LwjglTimer;
import java.io.Serializable;
/**
 *
 * @author Impaler
 */
public class Clock implements Serializable {

	long StartTime;
	long PausedTime;

    int SampleSize;
    int SampleIndex;

    long SamplingPause;

    double AverageTime;

    long[] AcumulationVector;

	boolean started;
	boolean paused;
	
	LwjglTimer CPUClock;
	

	public Clock(int Size) {
		//System.currentTimeMillis();
		
		CPUClock = new LwjglTimer();

		StartTime = 0;
		PausedTime = 0;
		started = false;
		paused = false;
		AverageTime = 0;
		SamplingPause = 0;
		
		SetSampleSize(Size);
	}

	public void Start() {
		StartTime = CPUClock.getTime();
		started = true;
		paused = false;
		SampleIndex = 0;
		PausedTime = 0;
		SamplingPause = 0;
	}

	public long Stop() {
		long Elapsed = getElapsed();

		started = false;
		paused = false;
		StartTime = 0;
		PausedTime = 0;
		SampleIndex = 0;

		return Elapsed;
	}

	public long getElapsed() {
		if (started) {
			if (paused) {
				return PausedTime;
			} else {
				return CPUClock.getTime() - StartTime;
			}
		}
		return 0;
	}

	public void Pause() {
		if ((started == true) && (paused == false)) {
			paused = true;
			AcumulationVector[SampleIndex] = CPUClock.getTime() - (StartTime + SamplingPause);
			PausedTime = CPUClock.getTime() - StartTime;

			SampleIndex++;
			if(SampleIndex == SampleSize) {
				doAverage();
			}
		}
	}

	public void Unpause() {
		if (paused == true) {
			paused = false;
			StartTime = CPUClock.getTime() - PausedTime;
			SamplingPause = PausedTime;
			PausedTime = 0;
		}
	}

	public void SetSampleSize(int Size) {
		SampleSize = Size;
		if (SampleSize == 0) {
			SampleSize = 1;
		}

		AcumulationVector = new long[SampleSize];

		SampleIndex = 0;
		SamplingPause = 0;
	}

	private void doAverage() {
		long Total = 0;
		for (short i = 0; i < SampleSize; i++) {
			Total += AcumulationVector[i];
		}
		SampleIndex = 0;
		AcumulationVector = new long[SampleSize];
		AverageTime = (double)Total / (double)SampleSize;
	}

	public double getAverage() {
		return AverageTime;
	}
}
