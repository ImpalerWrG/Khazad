/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Game.Game;
import Interface.GameCameraState;
import Job.JobManager;
import PathFinding.PathFinding;
import Renderer.MapRenderer;
import Renderer.PathingRenderer;
import Renderer.SelectionRenderer;
import Renderer.TerrainRenderer;
import com.jme3.app.Application;
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

	private Application app;
	private Nifty nifty;
	Element TutorialPopup = null;
	Element ErrorPopup = null;

	public SetupScreenController(Nifty nifty, Application app) {
		this.app = app;
		this.nifty = nifty;
	}

	public void bind(Nifty nifty, Screen screen) {
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
	
	public void CancelSetup() {
		//nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).setText("");
		nifty.gotoScreen("StartScreen");
	}


	public void BeginGame() {
		String kingdomName = nifty.getCurrentScreen().findNiftyControl("KingdomNameTextField", TextField.class).getDisplayedText();
		if (kingdomName.length() == 0) {
			ShowError("Please enter a kingdom name");
			return;
		}
		String Seed = nifty.getCurrentScreen().findNiftyControl("SeedTextField", TextField.class).getDisplayedText();

		Game game = new Game();
		this.app.getStateManager().attach(game);
		game.InitializeGame((short) 10, (short) 10, Seed, kingdomName);

		this.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
		this.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);
		this.app.getStateManager().attach(new SelectionRenderer());
		this.app.getStateManager().getState(PathingRenderer.class).attachToGame(game);

		GameCameraState cam = new GameCameraState();
		this.app.getStateManager().attach(cam);
		cam.SetViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
		cam.SetSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

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
