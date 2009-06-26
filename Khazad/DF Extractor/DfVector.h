#ifndef DFVECTOR_H_INCLUDED
#define DFVECTOR_H_INCLUDED

#include <Process.h>

/// TODO: dangerous, there's no sync between our vectors and the in-game ones. do not reuse.
class DfVector
{
private:
    // starting offset
    Uint32 start;
    // vector size
    Uint32 size;
    // vector item size
    Uint32 item_size;
public:
    DfVector(Uint32 _start, Uint32 _size, Uint32 _item_size):
        start(_start),size(_size),item_size(_item_size) {};
    // get offset of the specified index
    Uint32 operator[](Uint32 index)
    {
        if(index < size)
            return start + index*item_size;
        return 0; // return zero in case of out-of-bounds query
    }
    // get vector size
    Uint32 getSize()
    {
        return size;
    }
    // read item_size bytes from the right offset
    bool read (Uint32 index, Uint8 *target)
    {
        if(index < size)
            if(PROCESS->isValid())
                return PROCESS -> read (start + index*item_size, item_size, target);
        return false;
    }
};

#endif // DFVECTOR_H_INCLUDED
