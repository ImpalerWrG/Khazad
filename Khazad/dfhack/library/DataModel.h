#ifndef DATAMODEL_H_INCLUDED
#define DATAMODEL_H_INCLUDED

class DfVector;

// let's go pure virtual.
class DataModel
{
    public:
    // read a string
    virtual const string readSTLString (uint32_t offset) = 0;
    // read a vector from memory
    virtual DfVector readVector (uint32_t offset, uint32_t item_size) = 0;
};

class DMWindows40d : public DataModel
{
    virtual const string readSTLString (uint32_t offset);
    // read a vector from memory
    virtual DfVector readVector (uint32_t offset, uint32_t item_size);
};

class DMLinux40d : public DataModel
{
    virtual const string readSTLString (uint32_t offset);
    // read a vector from memory
    virtual DfVector readVector (uint32_t offset, uint32_t item_size);
};

#endif // DATAMODEL_H_INCLUDED
