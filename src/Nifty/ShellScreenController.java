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
import de.lessvoid.nifty.elements.Element;

/**
 *
 * @author Impaler
 */
public class ShellScreenController implements ScreenController {

	private Nifty nifty;
	Element TutorialPopup = null;
	Element ErrorPopup = null;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
	}

	public void onStartScreen() {
	}

	public void onEndScreen() {
	}

	public void quit() {
		Main.app.stop();
	}

	public void gameSetup() {
		nifty.gotoScreen("SetupScreen");
	}

	public void openTutorialPopup() {
		if (TutorialPopup == null) {
			TutorialPopup = nifty.createPopup("TutorialPopup");
		}
		nifty.showPopup(nifty.getCurrentScreen(), this.TutorialPopup.getId(), null);
	}

	public void closeTutorialPopup(){
		if (TutorialPopup != null) {
			nifty.closePopup(this.TutorialPopup.getId());
		}
	}

	public void beginCredits() {
	}

	public void endCredtis() {
	}

	public void loadGame() {
		nifty.gotoScreen("LoadGameScreen");
	}
}
