#ifndef DATATYPE__HEADER
#define DATATYPE__HEADER

#include <stdafx.h>

#include <tinyxml.h>

class DataBase
{

public:

    DataBase();
    virtual ~DataBase();
    virtual bool Load();

private:

    char* Name;
};

class MaterialData: public DataBase
{

public:

    MaterialData();
    ~MaterialData();
    bool Load(TiXmlElement* Element);

    Uint32 getTexture() { return TextureID; }
    Uint16 getHardness() { return Hardness; }

private:

    Uint32 TextureID;
    Uint16 Hardness;

};

#endif // DATATYPE__HEADER
