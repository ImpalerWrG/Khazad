/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Nifty;

import com.jme3.app.Application;
import com.jme3.niftygui.NiftyJmeDisplay;
import de.lessvoid.nifty.Nifty;

import com.jme3.app.state.AbstractAppState;

/**
 *
 * @author Impaler
 */
public class GUI extends AbstractAppState {

	private Nifty nifty;
	private Application app;

	public GUI(Application app) {
		this.app = app;

		NiftyJmeDisplay niftyDisplay = new NiftyJmeDisplay(app.getAssetManager(), app.getInputManager(), app.getAudioRenderer(), app.getGuiViewPort());
        nifty = niftyDisplay.getNifty();

		//nifty.loadStyleFile("Dwarven.xml");

		nifty.registerScreenController(new GameScreenController(nifty, this.app));
		nifty.registerScreenController(new ShellScreenController(nifty, this.app));
		nifty.registerScreenController(new GameTopBarController(nifty, this.app));

		nifty.addXml("Interface/Nifty/GameScreenNifty.xml");
		nifty.addXml("Interface/Nifty/StartScreenNifty.xml");
		nifty.addXml("Interface/Nifty/SetupScreenNifty.xml");
		
		nifty.gotoScreen("StartScreen");
		
        app.getGuiViewPort().addProcessor(niftyDisplay);
	}
}
