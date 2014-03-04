/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

/**
 *
 * @author Impaler
 */
public abstract class Temporal {
	
	int ID;

	Temporal() {
		TemporalManager.getSingleton().AddTemporal(this, 1);
	}

	void ResetCooldown(int NewCoolDown) {
		// GEt the actor and move to appropriate carosel
	}

	void Retire() {
		//GAME->getTemporal()->RetireTemporal(this);
	}

	public int getID() {
		return ID;
	}

	abstract int Wake();
}
