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

package PathFinding;

import java.io.Serializable;
import java.util.ArrayList;

/**
 * Container class for storage of generic Nodes that are used in PathFinding
 *
 * @author Impaler
 */
public class Pool<T> implements Serializable {

	private static final long serialVersionUID = 1;
	ArrayList<T> ObjectPool;
	PathAlgorithm<T> ObjectFactory;
	public boolean InUse;
	int ObjectCount;

	Pool() {
		ObjectPool = new ArrayList<T>();
		InUse = false;
		ObjectCount = 0;
	}

	void setFactory(PathAlgorithm Factory) {
		InUse = true;
		//Wipe();
		// TODO change ObjectPool if T class is different
		ObjectFactory = Factory;
	}

	T provide() {
		if (ObjectCount == ObjectPool.size()) {
			T NewObject = ObjectFactory.provide();
			ObjectPool.add(NewObject);
			ObjectCount++;
			return NewObject;
		}
		return ObjectPool.get(ObjectCount++);
	}

	void release() {
		InUse = false;
		ObjectCount = 0;

		if (ObjectPool.size() > 3000)
			ObjectPool.subList(0, 3000);

		ObjectFactory = null;
	}

	void wipe() {
		ObjectCount = 0;
		ObjectPool = null;
	}

	boolean isInUse() {
		return InUse;
	}

	void setInUse(boolean NewValue) {
		InUse = NewValue;
	}
}