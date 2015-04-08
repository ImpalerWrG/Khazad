/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Core.Main;
import Game.Game;
import Game.SaveGameHeader;
import Interface.GameCameraState;
import Job.JobManager;
import PathFinding.PathManager;
import Renderer.MapRenderer;
import Renderer.PathingRenderer;
import Renderer.SelectionRenderer;
import Renderer.TerrainRenderer;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.NiftyEventSubscriber;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.controls.ListBox;
import de.lessvoid.nifty.controls.ListBoxSelectionChangedEvent;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileSystemView;

/**
 *
 * @author Dallas
 */
public class LoadGameScreenController implements ScreenController {

	private Nifty nifty;
	private Screen screen;
	Element ErrorPopup = null;
	ListBox SaveGames;
	Label SaveDescription;
	SaveGameHeader SelectedSaveGameHeader = null;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
		this.screen = screen;
		this.SaveGames = screen.findNiftyControl("SaveGames", ListBox.class);
		this.SaveDescription = screen.findNiftyControl("SaveDescription", Label.class);
	}

	public void onStartScreen() {
		refreshSaveGameList();
	}

	private void refreshSaveGameList() {
		SaveGames.clear();
		ArrayList<SaveGameHeader> saveGames = getSaveGames();
		Collections.sort(saveGames);
		SaveGames.addAllItems(saveGames);
	}

	public void onEndScreen() {
	}

	public void cancelLoadGame() {
		nifty.gotoScreen("StartScreen");
	}

	private ArrayList<SaveGameHeader> getSaveGames() {
		ArrayList<SaveGameHeader> saveGameHeaders = new ArrayList<SaveGameHeader>();

		// find the save games header
		JFileChooser fr = new JFileChooser();
		FileSystemView fw = fr.getFileSystemView();
		String myDocumentsFolder = fw.getDefaultDirectory().toString();
		String saveGamesFolder = myDocumentsFolder + "\\my games\\Khazad\\Worlds\\";
		File saveGamesFolderFile = new File(saveGamesFolder);
		if (!saveGamesFolderFile.exists()) {
			// if folder does not exist then there cannot be any saves
			return saveGameHeaders;
		}

		// check all the files that end with .sav, and extract the save game headers
		for (final File saveFile : saveGamesFolderFile.listFiles()) {
			ObjectInputStream ois = null;
			try {
				if (!saveFile.isDirectory()) {
					String saveFileName = saveFile.getName();
					if (saveFileName.endsWith(".sav")) {
						// read the save file header
						ois = new ObjectInputStream(new FileInputStream(saveFile));
						SaveGameHeader saveGameHeader = (SaveGameHeader) ois.readObject();
						saveGameHeader.fileName = saveFileName;
						saveGameHeaders.add(saveGameHeader);
					}
				}
			} catch (Exception e) {
				ErrorPopupController.ShowErrorMessage(nifty, "Problem displaying save games", e.toString());
				e.printStackTrace();
			} finally {
				try {
					if (ois != null) {
						ois.close();
					}
				} catch (Exception e) {
					ErrorPopupController.ShowErrorMessage(nifty, "Problem displaying save games", e.toString());
					e.printStackTrace();
				}
			}
		}
		return saveGameHeaders;
	}

	@NiftyEventSubscriber(id = "SaveGames")
	public void onSaveGamesSelectionChanged(final String id, final ListBoxSelectionChangedEvent<SaveGameHeader> event) {
		List<SaveGameHeader> selection = event.getSelection();
		if (selection.size() == 1) {
			SelectedSaveGameHeader = selection.get(0);
			displaySaveGameHeader(SelectedSaveGameHeader);
		}
	}

	private void displaySaveGameHeader(SaveGameHeader saveGameHeader) {
		DateFormat dateFormatter = DateFormat.getDateInstance(DateFormat.LONG);
		SimpleDateFormat timeFormatter = new SimpleDateFormat("h:mm a");
		String lastPlayed = dateFormatter.format(saveGameHeader.lastPlayed) + " " + timeFormatter.format(saveGameHeader.lastPlayed);

		SaveDescription.setText(saveGameHeader.kingdomName + System.lineSeparator()
				+ System.lineSeparator()
				+ saveGameHeader.timeString + System.lineSeparator()
				+ "Last Played: " + lastPlayed + System.lineSeparator()
				+ System.lineSeparator()
				+ "Version: " + saveGameHeader.version);
	}


	public void DeleteGame() {
		if (SelectedSaveGameHeader == null) {
			ErrorPopupController.ShowErrorMessage(nifty, "Problem deleting game", "Please select a game to delete.");
			return;
		}
		// TODO are you sure?
		File saveFile = getSelectedSaveFile();
		if (!saveFile.exists()) {
			ErrorPopupController.ShowErrorMessage(nifty, "Problem deleting game", SelectedSaveGameHeader.fileName + " save does not exist.");
			refreshSaveGameList();
			return;
		}
		saveFile.delete();
		refreshSaveGameList();
	}

	private File getSelectedSaveFile() {
		// look in the my documents\my games\Khazad\Worlds folder for the savegame
		JFileChooser fr = new JFileChooser();
		FileSystemView fw = fr.getFileSystemView();

		String myDocumentsFolder = fw.getDefaultDirectory().toString();
		String saveGamesFolder = myDocumentsFolder + "\\my games\\Khazad\\Worlds\\";
		return new File(saveGamesFolder + SelectedSaveGameHeader.fileName);
	}

	public void LoadGame() {
		if (SelectedSaveGameHeader == null) {
			ErrorPopupController.ShowErrorMessage(nifty, "Problem loading game", "Please select a game to load.");
			return;
		}
		ObjectInputStream ois = null;
		try {
			File saveFile = getSelectedSaveFile();
			if (!saveFile.exists()) {
				ErrorPopupController.ShowErrorMessage(nifty, "Problem loading game", SelectedSaveGameHeader.fileName + " save does not exist.");
				refreshSaveGameList();
				return;
			}

			// now read the save file
			ois = new ObjectInputStream(new FileInputStream(saveFile));
			// first thing should always be the version number, so that we can give helpful error messages.
			SaveGameHeader saveGameHeader = (SaveGameHeader) ois.readObject();
			if (!saveGameHeader.version.equals(Game.version)) {
				// TODO it might still be possible to load these saves
				ErrorPopupController.ShowErrorMessage(nifty, "Problem loading game",
						"Game save is version " + saveGameHeader.version + ", current game version is " + Game.version + "." + System.lineSeparator()
						+ System.lineSeparator()
						+ "Unfortunately game saves from version " + saveGameHeader.version + " are no longer supported, please start a new game.");
				return;
			}
			Game game = (Game) ois.readObject();
			// copy a few things from the header into the game (since we don't keep the header)
			game.setKingdomName(saveGameHeader.kingdomName);
			game.setSaveGameFileName(SelectedSaveGameHeader.fileName);
			nifty.gotoScreen("GameScreen");

			// initialize the game
			Main.app.getStateManager().attach(game);
			Main.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
			Main.app.getStateManager().getState(TerrainRenderer.class).attachToGame(game);
			Main.app.getStateManager().attach(new SelectionRenderer());
			Main.app.getStateManager().getState(PathingRenderer.class).attachToGame(game);

			GameCameraState cam = new GameCameraState();

			Main.app.getStateManager().attach(cam);
			cam.setViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
			cam.setSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

			JobManager jobs = game.getSettlement().getJobManager();
			// PATHING
			PathManager Pather = PathManager.getSingleton();

			Pather.initialize(Main.app.getStateManager(), Main.app);
			Pather.createMapAbstraction(game.getMap());
			//Pather.AllocateThreadPool(ExecutionThreadpool);
			Main.app.getStateManager().attach(Pather);
		} catch (Exception e) {
			// TODO show a better message to the user
			ErrorPopupController.ShowErrorMessage(nifty, "Problem loading game", e.toString());
			e.printStackTrace();
		} finally {
			try {
				if (ois != null) {
					ois.close();
				}
			} catch (IOException e) {
				// TODO show a better message to the user
				ErrorPopupController.ShowErrorMessage(nifty, "Problem loading game", e.toString());
				e.printStackTrace();
			}
		}
	}
}
