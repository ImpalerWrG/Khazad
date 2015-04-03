/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Game.Citizen;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.NiftyEventSubscriber;
import de.lessvoid.nifty.controls.Controller;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.controls.WindowClosedEvent;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.xml.xpp3.Attributes;
import java.util.Properties;

/**
 *
 * @author Dallas
 */
public class CitizenWindowController implements Controller, Updatable {

	Nifty nifty;
	Screen screen;
	Element control;
	GameScreenController screenController;
	Citizen citizen;
	Label nameLabel;
	Label jobLabel;
	Label taskLabel;

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		this.nifty = nifty;
		this.screen = screen;
		this.control = element;
		this.screenController = (GameScreenController) screen.getScreenController();
		// there is definitely some sort of bug with these ID's
		nameLabel = control.findNiftyControl(control.getId() + "#CitizenWindow#window-content#CitizenWindowPanel#NameLabel", Label.class);
		jobLabel = control.findNiftyControl(control.getId() + "#CitizenWindow#window-content#CitizenWindowPanel#JobLabel", Label.class);
		taskLabel = control.findNiftyControl(control.getId() + "#CitizenWindow#window-content#CitizenWindowPanel#TaskLabel", Label.class);
	}

	public void setCitizen(Citizen citizen) {
		this.citizen = citizen;
		update();
	}

	public void update() {
		nameLabel.setText("Name: " + citizen.getName());
		jobLabel.setText("Current Job: " + citizen.PrimaryJob.getName());
		taskLabel.setText("Current Task: " + citizen.getTask().getName());
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

	public void OpenPopulationPopup() {
		screenController.OpenPopulationPopup();
	}
}
