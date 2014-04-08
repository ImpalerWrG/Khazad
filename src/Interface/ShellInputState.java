package Interface;


import com.jme3.app.state.AppStateManager;
import com.jme3.app.Application;
import com.jme3.app.state.AbstractAppState;
import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.AnalogListener;


public class ShellInputState extends AbstractAppState {

	protected InputManager input;
    protected Application app;
    protected AppStateManager stateManager;

	public ShellInputState() {

	}
	
	@Override
	public void initialize(AppStateManager stateManager, Application app) {
        this.app = app;
		this.input = app.getInputManager();
        this.stateManager = stateManager;        
		initialized = true;
		setEnabled(true);
    }

	@Override
    public void setEnabled(boolean enabled) {
      // Pause and unpause
		super.setEnabled(enabled);
		if(enabled){
			//input.addMapping(null, triggers);
			// init stuff that is in use while this state is RUNNING

			input.addListener(actionListener,"ExitGame");
		} else {
		  // take away everything not needed while this state is PAUSED
		}
    }

	private ActionListener actionListener = new ActionListener() {
		public void onAction(String name, boolean keyPressed, float tpf) {
			if (name.equals("ExitGame") && !keyPressed) {
			  app.stop();
			}
		}
	};
	
	private AnalogListener analogListener = new AnalogListener() {
		public void onAnalog(String name, float value, float tpf) {

		}
	};

	@Override
	public void update(float tpf) {
		
	}

	@Override
	public void cleanup() {
		
    }

}
