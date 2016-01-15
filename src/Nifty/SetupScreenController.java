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
import de.lessvoid.nifty.controls.TextField;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;

/**
 *
 * @author Dallas
 */
public class SetupScreenController implements ScreenController {

	private Nifty nifty;
	private Screen screen;
	Element TutorialPopup = null;
	Element ErrorPopup = null;
	Element MenuPopup = null;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
		this.screen = screen;
	}

	public void onStartScreen() {
		// set up a default kingdom name to make starting a new kingdom easier
		TextField kingdomNameTextField = nifty.getCurrentScreen().findNiftyControl("KingdomNameTextField", TextField.class);
		String kingdomName = kingdomNameTextField.getDisplayedText();
		if (kingdomName.length() == 0) {
			// TODO random kingdom name generator?
			kingdomNameTextField.setText("Lonely Mountain");
		}
		kingdomNameTextField.setCursorPosition(50);
	}

	public void onEndScreen() {
	}

	public void cancelSetup() {
		//nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).setText("");
		nifty.gotoScreen("StartScreen");
	}

	public void beginGame() {
		nifty.gotoScreen("ProgressBarScreen");

		try {
			String kingdomName = nifty.getCurrentScreen().findNiftyControl("KingdomNameTextField", TextField.class).getDisplayedText();
			if (kingdomName.length() == 0) {
				ErrorPopupController.ShowErrorMessage(nifty, "Problem starting game", "Please enter a kingdom name");
				return;
			}
			String Seed = nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).getDisplayedText();
			Main.createGame((short) 10, (short) 10, Seed, kingdomName);

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
