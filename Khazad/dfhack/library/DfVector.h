#ifndef DFVECTOR_H_INCLUDED
#define DFVECTOR_H_INCLUDED

class DfVector
{
private:
    // starting offset
    uint32_t start;
    // vector size
    uint32_t size;
    // vector item size
    uint32_t item_size;
    
public:
    DfVector(uint32_t _start, uint32_t _size, uint32_t _item_size):
        start(_start),size(_size),item_size(_item_size) {};
    DfVector(){};
    // get offset of the specified index
    inline uint32_t operator[](uint32_t index)
    {
        assert(index < size);
        return start + index*item_size;
    };
    // get vector size
    inline uint32_t getSize()
    {
        return size;
    };
    // read item_size bytes from the right offset
    inline void read (uint32_t index, uint8_t *target)
    {
        assert(index < size);
        Mread (start + index*item_size, item_size, target);
    };
};

#endif // DFVECTOR_H_INCLUDED
