/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Nifty;

import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;

/**
 *
 * @author Impaler
 */
public class GameScreenController implements ScreenController {

	private Nifty nifty;

	public GameScreenController(Nifty Newnifty) {
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

    public void BeginDesignation() {
        //nifty.gotoScreen("end");
    }
}
