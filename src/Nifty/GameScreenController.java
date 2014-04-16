/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Nifty;

import com.jme3.app.Application;

import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;
import de.lessvoid.nifty.screen.KeyInputHandler;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.elements.Element;

import Game.Game;
import Interface.GameCameraState;
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
        System.out.println("onStartScreen");
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
		closePopup();
	}
	
	public void Pause() {
		Game Time = app.getStateManager().getState(Game.class);
		Time.Pause(!Time.isPaused());
	}

	public void SetSpeed(String NewSpeed) {
		int speed = Integer.parseInt(NewSpeed);
		Game Time = app.getStateManager().getState(Game.class);
		Time.Pause(false);
		Time.setTickRate(speed);
	}
	
	public void Dig() {
		GameCameraState Cam = app.getStateManager().getState(GameCameraState.class);
		Cam.setMode(GameCameraState.CameraMode.SELECT_VOLUME);
	}
}
