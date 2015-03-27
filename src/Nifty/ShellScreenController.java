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
		System.out.println("bind( " + screen.getScreenId() + ")");
	}

	public void onStartScreen() {
		System.out.println("onStartScreen");
	}

	public void onEndScreen() {
		System.out.println("onEndScreen");
	}

	public void Quit() {
		app.stop();
	}

	public void CancelSetup() {
		//nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).setText("");
		nifty.gotoScreen("StartScreen");
	}

	public void GameSetup() {
		nifty.gotoScreen("SetupScreen");
	}

	public void BeginGame() {
		String Seed = nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).getDisplayedText();

		Game game = new Game();
		this.app.getStateManager().attach(game);
		game.InitializeGame((short) 10, (short) 10, Seed);

		this.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
		this.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);
		this.app.getStateManager().attach(new SelectionRenderer());
		this.app.getStateManager().getState(PathingRenderer.class).attachToGame(game);

		GameCameraState cam = new GameCameraState();
		this.app.getStateManager().attach(cam);
		cam.SetViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
		cam.SetSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

		JobManager jobs = game.getSettlement().getJobManager();

		// PATHING
		PathFinding Pather = PathFinding.getSingleton();
		Pather.initialize(this.app.getStateManager(), this.app);
		Pather.CreateMapAbstraction(game.getMap());
		//Pather.AllocateThreadPool(ExecutionThreadpool);
		this.app.getStateManager().attach(Pather);

		nifty.gotoScreen("GameScreen");

		for (int i = 0; i < 100; i++) {
			game.SpawnCitizen(Pather.Tester.getRandomPassableCoordinate());
		}
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
		// TODO maybe a GUI to pick a load game slot
		// otherwise, lets just hard code World01.sav for now

		String fileName = "World01.sav";
		ObjectInputStream ois = null;

		try {
			// look in the my documents\my games\Khazad folder for the savegame
			JFileChooser fr = new JFileChooser();
			FileSystemView fw = fr.getFileSystemView();

			String myDocumentsFolder = fw.getDefaultDirectory().toString();
			String saveGamesFolder = myDocumentsFolder + "\\my games\\Khazad\\";
			File saveFile = new File(saveGamesFolder + fileName);
			if (!saveFile.exists()) {
				ShowError(fileName + " save does not exist.");
				return;
			}

			// now read the save file
			ois = new ObjectInputStream(new FileInputStream(saveFile));
			// first thing should always be the version number, so that we can give helpful error messages.
			String gameVersion = (String)ois.readObject();
			if (!gameVersion.equals(Game.version)) {
				// TODO it might still be possible to load these saves
				ShowError("Game save is version " + gameVersion + ", current game version is " + Game.version + "." + System.lineSeparator()
						+ System.lineSeparator()
						+ "Unfortunately game saves from version " + gameVersion + " are no longer supported, please start a new game.");
				return;
			}
			Game game = (Game)ois.readObject();
			this.app.getStateManager().attach(game);

			this.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
			this.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);
			this.app.getStateManager().attach(new SelectionRenderer());
			this.app.getStateManager().getState(PathingRenderer.class).attachToGame(game);

			GameCameraState cam = new GameCameraState();
			this.app.getStateManager().attach(cam);
			cam.SetViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
			cam.SetSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

			JobManager jobs = game.getSettlement().getJobManager();

			// PATHING
			PathFinding Pather = PathFinding.getSingleton();
			Pather.initialize(this.app.getStateManager(), this.app);
			Pather.CreateMapAbstraction(game.getMap());
			//Pather.AllocateThreadPool(ExecutionThreadpool);
			this.app.getStateManager().attach(Pather);

			nifty.gotoScreen("GameScreen");
		} catch (IOException e) {
			// TODO show a better message to the user
			ShowError(e.toString());
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO show a better message to the user
			ShowError(e.toString());
			e.printStackTrace();
		} finally {
			try {
				if (ois != null) {
					ois.close();
				}
			} catch (IOException e) {
				// TODO show a better message to the user
				ShowError(e.toString());
				e.printStackTrace();
			}
		}
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
		}
	}
}
