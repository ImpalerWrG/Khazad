/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Game.Citizen;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.controls.Button;
import de.lessvoid.nifty.controls.Controller;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.xml.xpp3.Attributes;
import java.util.Properties;

/**
 *
 * @author Dallas
 */
public class ChooseCitizenControlController implements Controller {

	Nifty nifty;
	Screen screen;
	Element control;
	Citizen citizen;
	GameScreenController screenController;

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		this.nifty = nifty;
		this.screen = screen;
		this.control = element;
		this.screenController = (GameScreenController)screen.getScreenController();
	}
	
	public void setCitizen(Citizen citizen) {
		this.citizen = citizen;
		Button citizenButton = control.findNiftyControl(control.getId() + "#CitizenButton", Button.class);
		if (citizenButton != null) {
			citizenButton.setText(citizen.getName());
		}
		Button jobButton = control.findNiftyControl(control.getId() + "#JobButton", Button.class);
		if (jobButton != null) {
			jobButton.setText(citizen.PrimaryJob.getName());
		}
		Button taskButton = control.findNiftyControl(control.getId() + "#TaskButton", Button.class);
		if (taskButton != null) {
			taskButton.setText(citizen.getTask().getName());
		}
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

	public void openCitizen() {
		System.out.println("Opening citizen " + citizen.getName());
		screenController.ClosePopulationPopup();
		screenController.spawnCitizenWindow(citizen);		
	}

	public void openJob() {
		System.out.println("Opening job " + citizen.PrimaryJob.getName());
		screenController.ClosePopulationPopup();
		screenController.spawnCitizenWindow(citizen);		
	}
	
	public void openTask() {
		System.out.println("Opening task " + citizen.getTask().getName());
		screenController.ClosePopulationPopup();
		screenController.spawnCitizenWindow(citizen);		
	}
}
