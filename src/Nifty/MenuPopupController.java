/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Core.Main;
import Core.Utils;
import Game.Game;
import Game.SaveGameHeader;
import Interface.GameCameraState;
import Renderer.MapRenderer;
import Renderer.SelectionRenderer;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.controls.Controller;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.KeyInputHandler;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.xml.xpp3.Attributes;
import java.io.File;
import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
import java.util.Date;
import java.util.HashSet;
import java.util.Properties;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileSystemView;

/**
 *
 * @author Dallas
 */
public class MenuPopupController implements Controller {

	Nifty nifty;
	Screen screen;
	Element popup;
	GameScreenController screenController;

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		this.nifty = nifty;
		this.screen = screen;
		this.popup = element;
		this.screenController = (GameScreenController)screen.getScreenController();
	}

	public void init(Properties parameter, Attributes controlDefinitionAttributes) {
	}

	public void onStartScreen() {
	}

	public void onFocus(boolean getFocus) {
	}

	public boolean inputEvent(NiftyInputEvent inputEvent) {
		return false;
	}
	
	public void Quit() {
		Main.app.stop();
	}

	public void Abandon() {
		// Destroy Game object
		SelectionRenderer selectionRenderer = Main.app.getStateManager().getState(SelectionRenderer.class);
		Main.app.getStateManager().detach(selectionRenderer);
		selectionRenderer.cleanup();
		
		GameCameraState camera = Main.app.getStateManager().getState(GameCameraState.class);
		Main.app.getStateManager().detach(camera);
		camera.cleanup();
		
		Game game = Main.app.getStateManager().getState(Game.class);
		Main.app.getStateManager().getState(MapRenderer.class).detachFromGame();
		Main.app.getStateManager().detach(game);
		game.cleanup();

		Main.app.getRootNode().detachAllChildren();

		screenController.CloseMenuPopup();
		nifty.gotoScreen("StartScreen");
	}

	public void SaveGame() {
		// TODO maybe a GUI to pick a save game slot
		// otherwise, lets just hard code World01.sav for now

		ObjectOutputStream oos = null;

		try {
			// first, create the my documents\my games\Khazad\Worlds folder, if it does not already exist.
			JFileChooser fr = new JFileChooser();
			FileSystemView fw = fr.getFileSystemView();

			String myDocumentsFolder = fw.getDefaultDirectory().toString();
			String saveGamesFolder = myDocumentsFolder + "/my games/Khazad/Worlds/";
			File saveGamesFolderFile = new File(saveGamesFolder);
			if (!saveGamesFolderFile.exists()) {
				saveGamesFolderFile.mkdirs();
			}

			Game game = Main.app.getStateManager().getState(Game.class);

			if (game.getSaveGameFileName() == null) {
				// find an unused filename
				HashSet<String> saveFileNames = getFilesInFolder(saveGamesFolderFile);
				game.setSaveGameFileName(findUniqueFileName(saveFileNames));
			}

			// now create the save file, if it does not already exist
			File saveFile = new File(saveGamesFolder + game.getSaveGameFileName());
			if (!saveFile.exists()) {
				saveFile.createNewFile();
			}

			// now write to the save file
			SaveGameHeader saveGameHeader = new SaveGameHeader();
			saveGameHeader.version = game.version;
			saveGameHeader.lastPlayed = new Date(); // current time
			saveGameHeader.kingdomName = game.getKingdomName();
			saveGameHeader.timeString = game.getTimeString();
			oos = new ObjectOutputStream(new FileOutputStream(saveFile));
			oos.writeObject(saveGameHeader);
			oos.writeObject(game);
			ShowSaveSuccess();
			screenController.CloseMenuPopup();
		} catch (Exception e) {
			ShowSaveError(e.toString());
			e.printStackTrace();
		} finally {
			try {
				if (oos != null) {
					oos.close();
				}
			} catch (Exception e) {
				ShowSaveError(e.toString());
				e.printStackTrace();
			}
		}
	}

	private HashSet<String> getFilesInFolder(File folder) {
		HashSet<String> fileNames = new HashSet<String>();
		// get all the files that end with .sav, and put in a HashSet
		for (final File fileEntry : folder.listFiles()) {
			if (!fileEntry.isDirectory()) {
				String fileEntryName = fileEntry.getName();
				if (fileEntryName.endsWith(".sav")) {
					fileNames.add(fileEntryName);
				}
			}
		}
		return fileNames;
	}

	private String findUniqueFileName(HashSet<String> saveFileNames) {
		long saveNumber = 1;
		while (true) {
			String saveNumberString = Utils.padLeadingZero(saveNumber);
			String fileName = "World" + saveNumberString + ".sav";
			if (!saveFileNames.contains(fileName)) {
				return fileName;
			}
			saveNumber++;
		}
	}

	private void ShowSaveError(String errorMessage) {
		ErrorPopupController.ShowErrorMessage(nifty, "Problem loading save", errorMessage);
	}

	private void ShowSaveSuccess() {
		ErrorPopupController.ShowErrorMessage(nifty, "Save successful", "");
	}
}
