#ifndef DATATYPE__HEADER
#define DATATYPE__HEADER

#include <stdafx.h>

#include <tinyxml.h>

class DataBase
{

public:

    DataBase();
    virtual ~DataBase();
    bool Load(TiXmlElement* Entry, Uint32 Index);   // This must be called to index the Data Title
    virtual bool PostProcessing()              {};  // Empty for overRiding in derived data classes

protected:

    char Name[64];
};

class TextureData: public DataBase
{

public:

    TextureData();
    ~TextureData();
    bool Load(TiXmlElement* Element, Uint32 Index);

    char* getPath() { return Path; }

protected:

    char Path[256];
};


class MaterialData: public DataBase
{

public:

    MaterialData();
    ~MaterialData();
    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Uint16 getTexture() { return TextureID; }
    Uint16 getHardness() { return Hardness; }

    std::vector<Uint16> TileTypes;

protected:

    char TextureLabel[64];

    Uint32 TextureID;
    Uint16 Hardness;

};

#endif // DATATYPE__HEADER
