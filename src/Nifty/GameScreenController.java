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
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;
import de.lessvoid.nifty.screen.KeyInputHandler;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.controls.ScrollbarChangedEvent;
import de.lessvoid.nifty.controls.Scrollbar;
import de.lessvoid.nifty.NiftyEventSubscriber;

import Game.Game;
import Interface.GameCameraState;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileSystemView;

/**
 *
 * @author Impaler
 */
public class GameScreenController implements ScreenController, KeyInputHandler {

    private Application app;
    private Nifty nifty;
    boolean MenuUp = false;
    Element MenuPopup = null;

    public GameScreenController(Nifty Newnifty, Application app) {
        this.app = app;
        this.nifty = Newnifty;
    }

    public void bind(Nifty nifty, Screen screen) {
        System.out.println("bind( " + screen.getScreenId() + ")");
        screen.addKeyboardInputHandler(new KeyBoardMapping(), this);
        //screen.addPreKeyboardInputHandler(new KeyBoardMapping(), this);
    }

    public void onStartScreen() {
        System.out.println("GameScreen onStartScreen");
    }

    public void onEndScreen() {
        System.out.println("onEndScreen");
    }

    public boolean keyEvent(NiftyInputEvent event) {
        if (event != null) {
            if (event == NiftyInputEvent.Escape) {
                if (MenuUp) {
                    closePopup();
                    return true;
                } else {
                    Menu();
                    return true;
                }
            }
            //if (event == NiftyInputEvent.Activate)
        }
        return false;
    }

    public void Menu() {
        if (MenuPopup == null) {
            MenuPopup = nifty.createPopup("MenuPopup");
        }

        Game Time = app.getStateManager().getState(Game.class);
        Time.Pause(true);

        nifty.showPopup(nifty.getCurrentScreen(), this.MenuPopup.getId(), null);
        MenuUp = true;
    }

    public void closePopup() {
        if (MenuPopup != null) {
            nifty.closePopup(this.MenuPopup.getId());
            MenuUp = false;
        }
    }

    public void Quit() {
        this.app.stop();
    }

    public void Abandon() {
        closePopup();

        // Destroy Game object ?
        nifty.gotoScreen("StartScreen");
    }

    public void SaveGame() {
        // TODO maybe a GUI to pick a save game slot
        // otherwise, lets just hard code World01.sav for now

        String fileName = "World01.sav";
        ObjectOutputStream  oos = null;

        try {
            // first, create the my documents\my games\Khazad folder, if it does not already exist.
            JFileChooser fr = new JFileChooser();
            FileSystemView fw = fr.getFileSystemView();
            
            String myDocumentsFolder = fw.getDefaultDirectory().toString();
            String saveGamesFolder = myDocumentsFolder + "\\my games\\Khazad\\";
            File saveGamesFolderFile = new File(saveGamesFolder);
            if (!saveGamesFolderFile.exists())
            {
                saveGamesFolderFile.mkdirs();
            }
            
            // now create the save file, if it does not already exist
            File saveFile = new File(saveGamesFolder + fileName);
            if (!saveFile.exists()) {
                saveFile.createNewFile();
            }

            // now write to the save file
            oos = new ObjectOutputStream(new FileOutputStream(saveFile));
            // get the content in bytes
            Game game = app.getStateManager().getState(Game.class);
            game.Save(oos);
            System.out.println("Done");
        } catch (IOException e) {
            // TODO show a message to the user
            e.printStackTrace();
        } finally {
            try {
                if (oos != null) {
                    oos.close();
                }
            } catch (IOException e) {
                // TODO show a message to the user
                e.printStackTrace();
            }
        }        
        //Game game = app.getStateManager().getState(Game.class);
        //game.Save(stream)
        closePopup();
    }

    public void Pause() {
        Game game = app.getStateManager().getState(Game.class);
        game.Pause(!game.isPaused());
    }

    public void SetSpeed(String NewSpeed) {
        int speed = Integer.parseInt(NewSpeed);
        Game game = app.getStateManager().getState(Game.class);
        game.Pause(false);
        game.setTickRate(speed);
    }

    public void Dig() {
        GameCameraState Cam = app.getStateManager().getState(GameCameraState.class);
        Cam.setMode(GameCameraState.CameraMode.SELECT_VOLUME);
    }

    @NiftyEventSubscriber(id = "DepthSlider")
    public void DepthSliderChanged(final String id, final ScrollbarChangedEvent event) {

        Scrollbar bar = event.getScrollbar();
        Game game = app.getStateManager().getState(Game.class);
        int High = game.getMap().getHighestCell();
        int Low = game.getMap().getLowestCell();
        bar.setWorldMax(High - Low);

        GameCameraState camera = app.getStateManager().getState(GameCameraState.class);
        int top = camera.getSliceTop();
        int bottom = camera.getSliceBottom();

        int value = (int) event.getValue();
        int slice = camera.getSliceTop() - camera.getSliceBottom();
        camera.SetSlice(High - value, High - value - slice);
    }
}
