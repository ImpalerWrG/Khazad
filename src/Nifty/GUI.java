/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Nifty;

import com.jme3.app.Application;
import com.jme3.niftygui.NiftyJmeDisplay;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.screen.Screen;


/**
 *
 * @author Impaler
 */
public class GUI {
	
	private Nifty nifty;
	private static GUI instance = null;
	
	protected GUI() {

	}

	public static GUI getGUI() {
	   if(instance == null) {
		  instance = new GUI();
	   }
	   return instance;
	}

	public void InitializeGUI(Application app) {
		
		NiftyJmeDisplay niftyDisplay = new NiftyJmeDisplay(app.getAssetManager(), app.getInputManager(), app.getAudioRenderer(), app.getGuiViewPort());
        nifty = niftyDisplay.getNifty();

		nifty.registerScreenController(new GameScreenController(nifty));
		nifty.addXml("Interface/Nifty/GameScreenNifty.xml");

		nifty.registerScreenController(new StartScreenController(nifty));
		nifty.addXml("Interface/Nifty/StartScreenNifty.xml");

		nifty.gotoScreen("StartScreen");
		
        app.getGuiViewPort().addProcessor(niftyDisplay);
	}
}
