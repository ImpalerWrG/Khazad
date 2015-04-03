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
import Game.Citizen;

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
import de.lessvoid.nifty.NiftyIdCreator;
import de.lessvoid.nifty.controls.dynamic.CustomControlCreator;

/**
 *
 * @author Impaler
 */
public class GameScreenController implements ScreenController, KeyInputHandler {

	private Nifty nifty;
	Screen screen;
	Element MenuPopup;
	Element PopulationPopup;
	Element windows;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
		this.screen = screen;
		System.out.println("bind( " + screen.getScreenId() + ")");
		screen.addKeyboardInputHandler(new KeyBoardMapping(), this);
		windows = screen.findElementByName("windows");
		//screen.addPreKeyboardInputHandler(new KeyBoardMapping(), this);
	}

	public void onStartScreen() {
		System.out.println("GameScreen onStartScreen");
	}

	public void onEndScreen() {
		MenuPopup = null;
		PopulationPopup = null;
		// close any open windows
		for(Element tempElement : windows.getElements()){
			nifty.removeElement(screen, tempElement);
		}
		System.out.println("onEndScreen");
	}

	public boolean keyEvent(NiftyInputEvent event) {
		if (event != null) {
			if (event == NiftyInputEvent.Escape) {
				// if a popup is open, close it, otherwise open the menu
				if (MenuPopup != null) {
					CloseMenuPopup();
				} else if (PopulationPopup != null) {
					ClosePopulationPopup();
				} else {
					OpenMenuPopup();
				}
				return true;
			}
			//if (event == NiftyInputEvent.Activate)
		}
		return false;
	}

	public void OpenMenuPopup() {
		CloseMenuPopup();
		Game game = Main.app.getStateManager().getState(Game.class);
		game.Pause(true);

		MenuPopup = nifty.createPopup("MenuPopup");
		nifty.showPopup(screen, MenuPopup.getId(), null);
	}

	public void CloseMenuPopup() {
		if (MenuPopup != null) {
			nifty.closePopup(MenuPopup.getId());
			MenuPopup = null;
		}
	}

	public void OpenPopulationPopup() {
		ClosePopulationPopup();
		Game game = Main.app.getStateManager().getState(Game.class);
		game.Pause(true);
		// prevent mouse wheel interfering with scrolling a menu
		// TODO there may be a better way of doing this, e.g. nifty not passing the mousewheel event to the game.
		disableMouseWheel();

		PopulationPopup = nifty.createPopup("PopulationPopup");
		nifty.showPopup(screen, PopulationPopup.getId(), null);
	}

	public void ClosePopulationPopup() {
		if (PopulationPopup != null) {
			nifty.closePopup(PopulationPopup.getId());
			PopulationPopup = null;
		}
		enableMouseWheel();
	}

	public boolean inputEvent(NiftyInputEvent inputEvent) {
		return false;
	}

	public void onFocus(boolean getFocus) {
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
	
	private void disableMouseWheel() {
		GameCameraState camera = Main.app.getStateManager().getState(GameCameraState.class);
		camera.setMouseWheelEnabled(false);
	}
	
	private void enableMouseWheel() {
		GameCameraState camera = Main.app.getStateManager().getState(GameCameraState.class);
		camera.setMouseWheelEnabled(true);
	}
	
	public void spawnCitizenWindow(Citizen citizen) {
		String windowId = NiftyIdCreator.generate();
		CustomControlCreator citizenWindowCreator = new CustomControlCreator(windowId, "CitizenWindow");
		Element citizenWindow = citizenWindowCreator.create(nifty, screen, windows);
		// the controller needs to be set on the panel rather than the control, maybe due to the hidden window-content panel
		// there is something bugged about these ID's
		Element citizenWindowPanel = citizenWindow.findElementByName(windowId + "#CitizenWindow#window-content#CitizenWindow#window-content#CitizenWindowPanel");
		CitizenWindowController controller = citizenWindowPanel.getControl(CitizenWindowController.class);
		controller.setCitizen(citizen);
	}

}
