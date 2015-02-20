/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
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
import Interface.GameCameraState;
/**
 *
 * @author Impaler
 */
public class ShellScreenController implements ScreenController {
	
	private Application app;
	private Nifty nifty;

	public ShellScreenController(Nifty Newnifty, Application app) {
		this.app = app;
		this.nifty = Newnifty;
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
		game.InitializeGame((short) 10, (short) 10, Seed);
		this.app.getStateManager().attach(game);
		
		this.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);
		this.app.getStateManager().attach(new SelectionRenderer());
		this.app.getStateManager().getState(PathingRenderer.class).attachToGame(game);
		
		GameCameraState cam = new GameCameraState();
		this.app.getStateManager().attach(cam);
		cam.SetViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
		cam.SetSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

		JobManager jobs = game.getSettlment().getJobManager();
		
		// PATHING
		PathFinding Pather = PathFinding.getSinglton();
		Pather.initialize(this.app.getStateManager(), this.app);
		Pather.CreateMapAbstraction(game.getMap());
		//Pather.AllocateThreadPool(ExecutionThreadpool);
		this.app.getStateManager().attach(Pather);
		
		nifty.gotoScreen("GameScreen");

		for (int i = 0; i < 100; i++) {
			game.SpawnCitizen(Pather.Tester.getRandomPassableCoordinate());
		}
	}
}
