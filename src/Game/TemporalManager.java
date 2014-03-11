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

package Game;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;

import java.util.PriorityQueue;

import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.font.BitmapFont;
import com.jme3.font.BitmapText;

import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;

import com.jme3.math.ColorRGBA;

/**
 * Holds and advances the games time ticks at 12 ticks per simulated second, calls the Wake function of all
 * Time sensing objects and remaps them for their next wakeup
 * @author Impaler
 */
public class TemporalManager extends AbstractAppState implements ActionListener{
	
	private static TemporalManager Singleton = null;
	private SimpleApplication app;
	private AppStateManager state;

	BitmapText hudText;

	boolean Pause;
	int TickRate;
	long CurrentGameTick;
	float TickRounding;

	int UniqueIDCounter;
	PriorityQueue<Temporal> TemporalQueue;
	
	private TemporalManager() {
		TickRate = 100;
		TickRounding = 0;
		CurrentGameTick = 0;
		UniqueIDCounter = 0;
		Pause = true;
		
		TemporalQueue = new PriorityQueue<Temporal>();
	}

	public static TemporalManager getSingleton() {
		if (Singleton == null)
			Singleton = new TemporalManager();
			
		return Singleton;
	}

	@Override
    public void initialize(AppStateManager stateManager, Application app) {
        super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.state = stateManager;
		
		BuildText();
		registerWithInput(app.getInputManager());
	}

	public void onAction(String name, boolean keyPressed, float tpf) {
        if (this.isEnabled()) {
			if (name.equals("Pause")) {
				if (keyPressed)
					Pause = !Pause;
			}
			if (name.equals("Faster")) {
				if (keyPressed) 
					TickRate *= 2f;
			}
			if (name.equals("Slower")) {
				if (keyPressed)
					TickRate /= 2f;
			}
		}
	}

	public void registerWithInput(InputManager inputManager) {
        String[] inputs = {"Pause", "Faster", "Slower"};

		inputManager.addMapping("Pause", new KeyTrigger(KeyInput.KEY_SPACE)); 
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_ADD)); 
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_EQUALS)); 
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_MINUS)); 
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_SUBTRACT)); 
        inputManager.addListener(this, inputs);
	}

	public long getCurrentTimeTick() {
		return CurrentGameTick;
	}

	public void UpdateTick() {
		CurrentGameTick++;   // Advance Tick count
		
		Temporal target;
		target = TemporalQueue.poll();
		do {	
			long RewakeTick = target.Wake(CurrentGameTick);
			if (RewakeTick != -1) {
				TemporalQueue.add(target);
			}
			target = TemporalQueue.poll();
		} while (target.WakeTick <= CurrentGameTick);	

		TemporalQueue.add(target);
	}
	
	public void changeTickRate(int RateChange) {
		TickRate += RateChange;

		if (TickRate < 1) {
			TickRate = 1;
		}
	}

	public void setTickRate(int NewRate) {
		TickRate = NewRate;
	}

	@Override
    public void update(float tpf) {
		if (!Pause) {
			float TargetTicks = TickRate * tpf;
			TargetTicks += TickRounding;
			int FullTicks = (int) TargetTicks;
			TickRounding = TargetTicks - FullTicks;

			for (int i = 0; i < FullTicks; i++) {
				UpdateTick();
			}
			long seconds = CurrentGameTick / 12;
			long minutes = seconds / 60;
			long hours = minutes / 60;
			long days = hours / 24;
			hudText.setText((hours %24) + ":" + (minutes % 60) + ":" + (seconds % 60));
		}
    }

	public void BuildText() {
		BitmapFont font = this.app.getAssetManager().loadFont("Interface/Fonts/Default.fnt");
		
		hudText = new BitmapText(font, false);          
		hudText.setSize(font.getCharSet().getRenderedSize());      // font size
		hudText.setColor(ColorRGBA.Blue);                             // font color
		hudText.setText("You can write any string here");             // the text
		hudText.setLocalTranslation(300, hudText.getLineHeight() * 2, 0); // position
		this.app.getGuiNode().attachChild(hudText);
	}

	boolean AddTemporal(Temporal NewTemporal, int NewCoolDown) {
		TemporalQueue.add(NewTemporal);	
		return true;
	}
}
