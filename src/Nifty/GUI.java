/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Nifty;

import com.jme3.app.Application;
import com.jme3.niftygui.NiftyJmeDisplay;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;


/**
 *
 * @author Impaler
 */
public class GUI implements ScreenController {
	
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
		nifty.fromXml("Interface/Nifty/KhazadNiftyGUI.xml", "start", this);

        app.getGuiViewPort().addProcessor(niftyDisplay);
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

    public void quit(){
        nifty.gotoScreen("end");
    }
	
}
