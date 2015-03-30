/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Game.Game;
import Game.SaveGameHeader;
import Interface.GameCameraState;
import Job.JobManager;
import PathFinding.PathFinding;
import Renderer.MapRenderer;
import Renderer.PathingRenderer;
import Renderer.SelectionRenderer;
import Renderer.TerrainRenderer;
import com.jme3.app.Application;
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
import java.util.List;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileSystemView;

/**
 *
 * @author Dallas
 */
public class LoadGameScreenController implements ScreenController {

	private Application app;
	private Nifty nifty;
	private Screen screen;
	Element ErrorPopup = null;
	ListBox SaveGames;
	Label SaveDescription;
	SaveGameHeader SelectedSaveGameHeader = null;

	public LoadGameScreenController(Nifty nifty, Application app) {
		this.app = app;
		this.nifty = nifty;
	}

	public void bind(Nifty nifty, Screen screen) {
		this.screen = screen;
		this.SaveGames = screen.findNiftyControl("SaveGames", ListBox.class);
		this.SaveDescription = screen.findNiftyControl("SaveDescription", Label.class);
	}

	public void onStartScreen() {
		refreshSaveGameList();
	}

	private void refreshSaveGameList() {
		SaveGames.clear();
		SaveGames.addAllItems(getSaveGames());
	}

	public void onEndScreen() {
	}

	public void CancelLoadGame() {
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
			} catch (IOException e) {
				ShowError(e.toString());
				e.printStackTrace();
			} catch (ClassNotFoundException e) {
				ShowError(e.toString());
				e.printStackTrace();
			} finally {
				try {
					if (ois != null) {
						ois.close();
					}
				} catch (IOException e) {
					ShowError(e.toString());
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
			DisplaySaveGameHeader(SelectedSaveGameHeader);
		}
	}

	private void DisplaySaveGameHeader(SaveGameHeader saveGameHeader) {
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

	private void ShowError(String errorMessage) {
		if (ErrorPopup == null) {
			ErrorPopup = nifty.createPopup("ErrorPopup");
		}
		Label errorLabel = ErrorPopup.findNiftyControl("ErrorLabel", Label.class);
		if (errorLabel
				!= null) {
			errorLabel.setText(errorMessage);
		}

		nifty.showPopup(screen, this.ErrorPopup.getId(), null);
	}

	public void CloseError() {
		if (ErrorPopup != null) {
			nifty.closePopup(this.ErrorPopup.getId());
			ErrorPopup = null;
		}
	}

	public void DeleteGame() {
		if (SelectedSaveGameHeader == null) {
			ShowError("Please select a game to delete.");
			return;
		}
		// TODO are you sure?
		File saveFile = getSelectedSaveFile();
		if (!saveFile.exists()) {
			ShowError(SelectedSaveGameHeader.fileName + " save does not exist.");
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
			ShowError("Please select a game to load.");
			return;
		}
		ObjectInputStream ois = null;
		try {
			File saveFile = getSelectedSaveFile();
			if (!saveFile.exists()) {
				ShowError(SelectedSaveGameHeader.fileName + " save does not exist.");
				refreshSaveGameList();
				return;
			}

			// now read the save file
			ois = new ObjectInputStream(new FileInputStream(saveFile));
			// first thing should always be the version number, so that we can give helpful error messages.
			SaveGameHeader saveGameHeader = (SaveGameHeader)ois.readObject();
			if (!saveGameHeader.version.equals(Game.version)) {
				// TODO it might still be possible to load these saves
				ShowError("Game save is version " + saveGameHeader.version + ", current game version is " + Game.version + "." + System.lineSeparator()
						+ System.lineSeparator()
						+ "Unfortunately game saves from version " + saveGameHeader.version + " are no longer supported, please start a new game.");
				return;
			}
			Game game = (Game)ois.readObject();
			// copy a few things from the header into the game (since we don't keep the header)
			game.kingdomName = saveGameHeader.kingdomName;
			game.saveGameFileName = saveGameHeader.fileName;
			
			// initialize the game
			this.app.getStateManager().attach(game);
			this.app.getStateManager().getState(MapRenderer.class).attachToGame(game);
			this.app.getStateManager()
					.getState(TerrainRenderer.class).attachToGame(game);
			this.app.getStateManager()
					.attach(new SelectionRenderer());
			this.app.getStateManager()
					.getState(PathingRenderer.class).attachToGame(game);

			GameCameraState cam = new GameCameraState();

			this.app.getStateManager()
					.attach(cam);
			cam.SetViewSize(game.getMap().getHighestCell(), game.getMap().getLowestCell());
			cam.SetSlice(game.getMap().getHighestCell() - 2, game.getMap().getLowestCell() + 2);

			JobManager jobs = game.getSettlement().getJobManager();
			// PATHING
			PathFinding Pather = PathFinding.getSingleton();

			Pather.initialize(
					this.app.getStateManager(), this.app);
			Pather.CreateMapAbstraction(game.getMap());
			//Pather.AllocateThreadPool(ExecutionThreadpool);
			this.app.getStateManager()
					.attach(Pather);

			nifty.gotoScreen(
					"GameScreen");
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
}
