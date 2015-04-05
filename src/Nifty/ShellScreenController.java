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

import com.jme3.app.Application;

import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.elements.Element;

/**
 *
 * @author Impaler
 */
public class ShellScreenController implements ScreenController {

	private Application app;
	private Nifty nifty;
	Element TutorialPopup = null;
	Element ErrorPopup = null;

	public ShellScreenController(Nifty nifty, Application app) {
		this.app = app;
		this.nifty = nifty;
	}

	public void bind(Nifty nifty, Screen screen) {
	}

	public void onStartScreen() {
	}

	public void onEndScreen() {
	}

	public void quit() {
		app.stop();
	}

	public void gameSetup() {
		nifty.gotoScreen("SetupScreen");
	}

	public void beginTutorial() {
		if (TutorialPopup == null) {
			TutorialPopup = nifty.createPopup("TutorialPopup");
		}
		nifty.showPopup(nifty.getCurrentScreen(), this.TutorialPopup.getId(), null);
	}

	public void endTutorial() {
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
		return;
	}

	private void showError(String errorMessage) {
		if (ErrorPopup == null) {
			ErrorPopup = nifty.createPopup("ErrorPopup");
		}
		Label errorLabel = ErrorPopup.findNiftyControl("ErrorLabel", Label.class);
		if (errorLabel != null) {
			errorLabel.setText(errorMessage);
		}

		nifty.showPopup(nifty.getCurrentScreen(), this.ErrorPopup.getId(), null);
	}

	public void closeError() {
		if (ErrorPopup != null) {
			nifty.closePopup(this.ErrorPopup.getId());
			ErrorPopup = null;
		}
	}
}
