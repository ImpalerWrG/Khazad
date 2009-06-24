#ifndef DATATYPE__HEADER
#define DATATYPE__HEADER

#include <stdafx.h>

#include <tinyxml.h>
#include <Paths.h>

class DataBase
{

public:

    DataBase();
    virtual ~DataBase();
    bool Load(TiXmlElement* Entry, Uint32 Index);   // This must be called to index the Data Title
    virtual bool PostProcessing()              {};  // Empty for overRiding in derived data classes

protected:

    string Name;
};

class FontData: public DataBase
{

public:

    FontData();
    ~FontData();
    bool Load(TiXmlElement* Element, Uint32 Index);

    Path getPath()    { return sPath; }
    Uint16 getSize()  { return Size; }

protected:

    Path sPath;
    Uint16 Size;
};

class TextureData: public DataBase
{

public:

    TextureData();
    ~TextureData();
    bool Load(TiXmlElement* Element, Uint32 Index);

    Path getPath() { return sPath; }

protected:

    Path sPath;
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

    string TextureLabel;

    Uint32 TextureID;
    Uint16 Hardness;

};

#endif // DATATYPE__HEADER
