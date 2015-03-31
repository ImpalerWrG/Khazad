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

package Nifty;

import Core.Main;

import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;
import de.lessvoid.nifty.screen.KeyInputHandler;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.controls.ScrollbarChangedEvent;
import de.lessvoid.nifty.controls.Scrollbar;
import de.lessvoid.nifty.NiftyEventSubscriber;

import Game.Game;
import Interface.GameCameraState;

/**
 *
 * @author Impaler
 */
public class GameScreenController implements ScreenController, KeyInputHandler {

	private Nifty nifty;
	Screen screen;
	Element MenuPopup;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
		this.screen = screen;
		System.out.println("bind( " + screen.getScreenId() + ")");
		screen.addKeyboardInputHandler(new KeyBoardMapping(), this);
		//screen.addPreKeyboardInputHandler(new KeyBoardMapping(), this);
	}

	public void onStartScreen() {
		System.out.println("GameScreen onStartScreen");
	}

	public void onEndScreen() {
		System.out.println("onEndScreen");
	}

	public void CloseMenuPopup() {
		if (MenuPopup != null) {
			nifty.closePopup(MenuPopup.getId());
			MenuPopup = null;
		}
	}

	public boolean keyEvent(NiftyInputEvent event) {
		if (event != null) {
			if (event == NiftyInputEvent.Escape) {
				if (MenuPopup != null) {
					CloseMenuPopup();
				} else {
					Menu();
				}
				return true;
			}
			//if (event == NiftyInputEvent.Activate)
		}
		return false;
	}

	public boolean inputEvent(NiftyInputEvent inputEvent) {
		return false;
	}

	public void onFocus(boolean getFocus) {
	}

	public void Menu() {
		MenuPopup = nifty.createPopup("MenuPopup");

		Game game = Main.app.getStateManager().getState(Game.class);
		game.Pause(true);

		nifty.showPopup(screen, MenuPopup.getId(), null);
	}

	public void Pause() {
		Game game = Main.app.getStateManager().getState(Game.class);
		game.Pause(!game.isPaused());
	}

	public void SetSpeed(String NewSpeed) {
		int speed = Integer.parseInt(NewSpeed);
		Game game = Main.app.getStateManager().getState(Game.class);
		game.Pause(false);
		game.setTickRate(speed);
	}

	public void Dig() {
		GameCameraState Cam = Main.app.getStateManager().getState(GameCameraState.class);
		Cam.setMode(GameCameraState.CameraMode.SELECT_VOLUME);
	}

	@NiftyEventSubscriber(id = "DepthSlider")
	public void DepthSliderChanged(final String id, final ScrollbarChangedEvent event) {

		Scrollbar bar = event.getScrollbar();
		Game game = Main.app.getStateManager().getState(Game.class);
		int High = game.getMap().getHighestCell();
		int Low = game.getMap().getLowestCell();
		bar.setWorldMax(High - Low);

		GameCameraState camera = Main.app.getStateManager().getState(GameCameraState.class);
		int top = camera.getSliceTop();
		int bottom = camera.getSliceBottom();

		int value = (int) event.getValue();
		int slice = camera.getSliceTop() - camera.getSliceBottom();
		camera.SetSlice(High - value, High - value - slice);
	}

	public void OpenPopulation() {
		Game game = Main.app.getStateManager().getState(Game.class);
		game.Pause(true);

		Element populationPopup = nifty.createPopup("PopulationPopup");
		nifty.showPopup(screen, populationPopup.getId(), null);
	}
}
