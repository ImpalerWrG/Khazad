/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.ArrayList;

/**
 *
 * @author Impaler
 */

public class Pool<T> {
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

	T ProvideObject() {
		if (ObjectCount == ObjectPool.size()) {
			T NewObject = ObjectFactory.provide();
			ObjectPool.add(NewObject);
			ObjectCount++;
			return NewObject;
		}
		return ObjectPool.get(ObjectCount++);
	}

	void Release() {
		InUse = false;
		ObjectCount = 0;
		
		if (ObjectPool.size() > 3000)
			ObjectPool.subList(0, 3000);
			
		ObjectFactory = null;
	}

	void Wipe() {
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

	/*
	ArrayList<Pool> PoolPool;

	CentralPool() {
		
	}

    CoordinatePool ProvidePool()  // Simple but effective
    {
        for (CoordinatePool TargetPool: PoolPool) {
            if (!TargetPool.isInUse()) {
                TargetPool.setInUse(true);
                return TargetPool;
            }
        }

        // No Pools available so Create a new one
        CoordinatePool NewPool = new CoordinatePool();
		NewPool.setInUse(true);
        PoolPool.add(NewPool);
        return NewPool;
    }
}
*/


