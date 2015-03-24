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
import de.lessvoid.nifty.elements.Element;

/**
 *
 * @author Impaler
 */
public class ShellScreenController implements ScreenController {

    private Application app;
    private Nifty nifty;
    Element TutorialPopup = null;

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

        this.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
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
}
