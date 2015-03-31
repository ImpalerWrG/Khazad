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
import de.lessvoid.nifty.controls.TextField;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;

import Game.Game;
import Job.JobManager;

import PathFinding.PathFinding;
import Renderer.TerrainRenderer;
import Renderer.SelectionRenderer;
import Renderer.PathingRenderer;
import Renderer.MapRenderer;
import Interface.GameCameraState;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.elements.Element;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileSystemView;

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

	public void Quit() {
		app.stop();
	}

	public void GameSetup() {
		nifty.gotoScreen("SetupScreen");
	}

	public void BeginTutorial() {
		if (TutorialPopup == null) {
			TutorialPopup = nifty.createPopup("TutorialPopup");
		}
		nifty.showPopup(nifty.getCurrentScreen(), this.TutorialPopup.getId(), null);
	}

	public void EndTutorial() {
		if (TutorialPopup != null) {
			nifty.closePopup(this.TutorialPopup.getId());
		}
	}

	public void BeginCredtis() {
	}

	public void EndCredtis() {
	}

	public void LoadGame() {
		nifty.gotoScreen("LoadGameScreen");
		return;
	}

	private void ShowError(String errorMessage) {
		if (ErrorPopup == null) {
			ErrorPopup = nifty.createPopup("ErrorPopup");
		}
		Label errorLabel = ErrorPopup.findNiftyControl("ErrorLabel", Label.class);
		if (errorLabel != null) {
			errorLabel.setText(errorMessage);
		}

		nifty.showPopup(nifty.getCurrentScreen(), this.ErrorPopup.getId(), null);
	}

	public void CloseError() {
		if (ErrorPopup != null) {
			nifty.closePopup(this.ErrorPopup.getId());
			ErrorPopup = null;
		}
	}
}
