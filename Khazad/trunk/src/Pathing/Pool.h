#ifndef POOL_HEADER
#define POOL_HEADER

#include <vector.h>

template<class T>
class Pool
{
public:

    Pool()
    {
        InUse = false;
        ObjectCount = 0;
    }

    ~Pool()
    {
        for (int i = 0; i < ObjectPool.size(); i++)
        {
            delete ObjectPool[i];
        }
    }

    T* ProvideObject()
    {
        if (ObjectCount == ObjectPool.size())
        {
            T* NewObject = new T();
            ObjectPool.push_back(NewObject);
            ObjectCount++;
            return NewObject;
        }
        return ObjectPool[ObjectCount++];
    }

    void Release()
    {
        InUse = false;
        ObjectCount = 0;
    }

    void Wipe()
    {
        ObjectCount = 0;
    }

    bool isInUse()                  { return InUse; }
    void setInUse(bool NewValue)    { InUse = NewValue; }

private:

    std::vector<T*> ObjectPool;

    bool InUse;
    int ObjectCount;
};

template<class T>
class CentralPool
{
public:

    Pool<T>* ProvidePool()  // Simple but effective
    {
        int i = 0;
        for (; i < PoolPool.size(); i++)
        {
            if (!PoolPool[i]->isInUse())
            {
                PoolPool[i]->setInUse(true);
                return PoolPool[i];
            }
        }

        // No Pools available so Create a new one
        Pool<T>* NewPool = new Pool<T>();
        PoolPool.push_back(NewPool);
        PoolPool[i]->setInUse(true);
        return NewPool;
    }

    ~CentralPool()
    {
        for (int i = 0; i < PoolPool.size(); i++)
        {
            delete PoolPool[i];
        }
    }

private:

    std::vector<Pool<T>*> PoolPool;
};

#endif // POOL_HEADER
