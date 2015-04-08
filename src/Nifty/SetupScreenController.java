/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Core.Main;
import Game.Game;
import Interface.GameCameraState;
import PathFinding.PathManager;
import Renderer.MapRenderer;
import Renderer.PathingRenderer;
import Renderer.SelectionRenderer;
import Renderer.TerrainRenderer;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.controls.Label;
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
	Element TutorialPopup = null;
	Element ErrorPopup = null;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
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
		try
		{
			String kingdomName = nifty.getCurrentScreen().findNiftyControl("KingdomNameTextField", TextField.class).getDisplayedText();
			if (kingdomName.length() == 0) {
				ErrorPopupController.ShowErrorMessage(nifty, "Problem starting game", "Please enter a kingdom name");
				return;
			}
			String Seed = nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).getDisplayedText();

			Game game = new Game();
			Main.app.getStateManager().attach(game);
			game.initializeGame((short) 10, (short) 10, Seed, kingdomName);

			Main.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
			Main.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);
			Main.app.getStateManager().attach(new SelectionRenderer());
			PathingRenderer pathingRenderer = Main.app.getStateManager().getState(PathingRenderer.class);
			pathingRenderer.attachToGame(game);

			GameCameraState cam = new GameCameraState();
			Main.app.getStateManager().attach(cam);
			cam.setViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
			cam.setSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

			// PATHING
			PathManager Pather = PathManager.getSingleton();
			Pather.initialize(Main.app.getStateManager(), Main.app);
			Pather.createMapAbstraction(game.getMap());
			//Pather.AllocateThreadPool(ExecutionThreadpool);
			Main.app.getStateManager().attach(Pather);


			short DwarfID = Data.DataManager.getDataManager().getLabelIndex("CREATURE_DWARF");
			for (int i = 0; i < 100; i++) {
				game.SpawnCitizen(DwarfID, Pather.Tester.getRandomPassableCoordinate());
			}
			nifty.gotoScreen("GameScreen");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
