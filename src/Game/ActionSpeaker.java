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

package Game;

import java.util.ArrayList;
import java.util.HashMap;


/**
 * Interface for a class that sends actions to an ActionListener
 * 
 * @author Impaler
 */
public interface ActionSpeaker {

	/**
	public static class Mapping {

        private final String name;
        private final ArrayList<ActionListener> listeners = new ArrayList<ActionListener>();

		public boolean matchesName(String name) {
			return (this.name.compareTo(name) == 0);
		}

        public Mapping(String name) {
            this.name = name;
        }
		
		public void addListener(ActionListener listener) {
			listeners.add(listener);
		}
		
		public void invokeAction(ActionData Data) {
			for (ActionListener listener : listeners) {
				listener.onAction(Data);
			}
		}
    } */
	
	public abstract class ActionData {};
	
    public final HashMap<String, ArrayList<ActionListener>> bindings = new HashMap<String, ArrayList<ActionListener>>();
	
	/**
     * Adds a new listener to receive events on the given mappings.
     *
     * <p>The given InputListener will be registered to receive events
     * on the specified mapping names. When a mapping raises an event, the
     * listener will have its appropriate method invoked, either
     * {@link ActionListener#onAction(java.lang.String, boolean, float) }
     * or {@link AnalogListener#onAnalog(java.lang.String, float, float) }
     * depending on which interface the <code>listener</code> implements.
     * If the listener implements both interfaces, then it will receive the
     * appropriate event for each method.
     *
     * @param listener The listener to register to receive input events.
     * @param Bindings The mapping names which the listener will receive
     * events from.
     *
     */
    public void addListener(ActionListener listener, String... Bindings);

	/**
     * Removes a listener from receiving events, will unregister the listener 
	 * from any mappings that it was previously registered with.
     *
     * @param listener The listener to unregister.
     */
    public abstract void removeListener(ActionListener listener);
	
	public void invokeActions(String Binding, ActionData Data);
	
	/**{
        ArrayList<Mapping> maps = bindings.get(bindinghash);
        if (maps == null) {
            return;
        }

        int size = maps.size();
        for (int i = size - 1; i >= 0; i--) {
            Mapping mapping = maps.get(i);
            ArrayList<InputListener> listeners = mapping.listeners;
            int listenerSize = listeners.size();
            for (int j = listenerSize - 1; j >= 0; j--) {
                InputListener listener = listeners.get(j);
                if (listener instanceof ActionListener) {
                    ((ActionListener) listener).onAction(mapping.name, Data);
                }
            }
        }
    }*/

}
