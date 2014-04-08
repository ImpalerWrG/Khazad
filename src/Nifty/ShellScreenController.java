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
import Job.WanderJob;
import PathFinding.PathManager;
import Renderer.TerrainRenderer;
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

		nifty.gotoScreen("GameScreen");

		Game game = new Game();
		game.InitializeGame((short) 10, (short) 10, Seed);
		this.app.getStateManager().attach(game);
		
		this.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);

		JobManager jobs = game.getSettlment().getJobManager();
		WanderJob newJob = new WanderJob();
		jobs.addJob(newJob);
		
		// PATHING
		PathManager Pather = PathManager.getSinglton();
		Pather.initialize(this.app.getStateManager(), this.app);
		Pather.CreateMapAbstraction(game.getMap());
		//Pather.AllocateThreadPool(ExecutionThreadpool);
		this.app.getStateManager().attach(Pather);
		
		for (int i = 0; i < 100; i++) {
			game.SpawnCitizen(Pather.Tester.getRandomPassableCoordinate());
		}
	}
}
