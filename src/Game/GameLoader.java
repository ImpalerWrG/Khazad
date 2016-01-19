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

package Game;

import Core.Main;
import static Core.Main.app;
import static Core.Main.setProgress;

import Nifty.GUI;
import PathFinding.PathManager;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.concurrent.Callable;

/**
 *
 * @author Impaler
 */
public class GameLoader implements Callable<Object>{

	ObjectInputStream ois = null;
	Game game = null;

	public Object call() {
		try {
			setProgress(.2f, "Loading Game File");

			game = (Game) ois.readObject();
			Main.app.getStateManager().attach(game);

			// PATHING
			setProgress(.7f, "Creating Pathfinding Abstraction");
			PathManager Pather = PathManager.getSingleton();
			Pather.initialize(Main.app.getStateManager(), Main.app);
			Pather.createMapAbstraction(game.getMap());
			Main.app.getStateManager().attach(Pather);

			Main.attachRenderers(game);
			GUI nifty = app.getStateManager().getState(GUI.class);
			nifty.gotoScreen("GameScreen");

		} catch (Exception e) {
			// TODO show a better message to the user
			Main.showErrorPopup("Problem loading game", e.toString());
			e.printStackTrace();
		} finally {
			try {
				if (ois != null) {
					ois.close();
				}
			} catch (IOException e) {
				// TODO show a better message to the user
				Main.showErrorPopup("Problem loading game", e.toString());
				e.printStackTrace();
			}
		}

		return null;
	}
	
	public void setParameters(ObjectInputStream stream, Game game) {
		this.ois = stream;
		this.game = game;
	}
}
