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
    virtual bool PostProcessing()              {return false;};  // Empty for overRiding in derived data classes

protected:

    string Name;
};

class ColorData: public DataBase
{

public:

    ColorData();
    ~ColorData();

    bool Load(TiXmlElement* Element, Uint32 Index);

    Uint8 getRed()      { return Red; }
    Uint8 getGreen()    { return Green; }
    Uint8 getBlue()     { return Blue; }

protected:

    Uint8 Red;
    Uint8 Green;
    Uint8 Blue;
};

class TextureData: public DataBase
{

public:

    TextureData();
    ~TextureData();
    bool Load(TiXmlElement* Element, Uint32 Index);

    Path getPath() { return sPath; }

    void setDevILID(Uint32 ID)      { DevILID = ID; }
    Uint32 getDevILID()             { return DevILID; }

protected:

    Path sPath;
    Uint32 DevILID;
};

class ModelData: public DataBase
{

public:

    ModelData();
    ~ModelData();
    bool Load(TiXmlElement* Element, Uint32 Index);

    Path getPath()                  { return sPath; }

protected:

    Path sPath;
};

class SurfaceTypeData: public DataBase
{

public:

    SurfaceTypeData();
    ~SurfaceTypeData();
    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getTextureID()         { return TextureID; }

protected:

    string TextureLabel;
    Sint16 TextureID;
};

class TileGroupData: public DataBase
{

public:

    TileGroupData();
    ~TileGroupData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getMaterialClassID()     { return MaterialClassID; }
    Sint16 getMaterialID()          { return MaterialID; }

    std::vector<Uint16> TileValues;
    std::vector<Uint16> TileShapeID;
    std::vector<Uint16> SurfaceTypeID;

protected:

    string MaterialClassLabel;
    string MaterialLabel;

    std::vector<string> TileShapeLabels;
    std::vector<string> SurfaceTypeLabels;

    Sint16 MaterialClassID;
    Sint16 MaterialID;
};

class MaterialClassData: public DataBase
{

public:

    MaterialClassData();
    ~MaterialClassData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getDefaultMaterial()     { return DefaultMaterialID; }
    Uint16 getMatGlossIndex()       { return MatGlossIndex; }

    Sint16 getTexture(Uint16 SurfaceID)     { return TextureArray[SurfaceID]; }

protected:

    Sint16* TextureArray;

    string DefaultMaterialLabel;

    Sint16 DefaultMaterialID;
    Uint16 MatGlossIndex;

    std::vector<string> SurfaceTypeLabels;
    std::vector<string> TextureLabels;
};

class MaterialData: public DataBase
{

public:

    MaterialData();
    ~MaterialData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getMaterialClass()           { return MaterialClassID; }

    Sint16 getPrimaryColorID()          { return PrimaryColorID; }
    Sint16 getSecondaryColorID()        { return SecondaryColorID; }
    Sint16 getBorderColorID()           { return BorderColorID; }
    string getColorMode()               { return ColorMode; }

    string getMatGloss()                { return MatGloss; }

    Sint16 getTexture(Uint16 SurfaceID)     { return TextureArray[SurfaceID]; }

protected:

    Sint16* TextureArray;

    string PrimaryColorLabel;
    string SecondaryColorLabel;
    string BorderColorLabel;

    string MaterialClassLabel;

    string MatGloss;
    string ColorMode;

    Sint16 PrimaryColorID;
    Sint16 SecondaryColorID;
    Sint16 BorderColorID;

    Sint16 MaterialClassID;

    std::vector<string> SurfaceTypeLabels;
    std::vector<string> TextureLabels;
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

class TileShapeData: public DataBase
{

public:
    TileShapeData();
    ~TileShapeData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    bool isOpen()               { return Open; }

    Sint16 getModelID()         { return ModelID; }

protected:

    bool Open;

    Sint16 ModelID;
    string ModelLabel;
};

struct TreeVariant
{
    string TrunkMaterial;
    string LeavesMaterial;
    string SnowMaterial;
    Sint16 TrunkMaterialID;
    Sint16 LeavesMaterialID;
    Sint16 SnowMaterialID;
};

class TreeData: public DataBase
{

public:
    TreeData();
    ~TreeData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    TreeVariant getVariant(Uint32 idx)          { return variants[idx]; }

    Sint16 getModelID()                     { return ModelID; }
    string getMatgloss()                    { return Matgloss; }

protected:

    vector <TreeVariant> variants;

    string Matgloss;

    Sint16 ModelID;
    string ModelLabel;
};

#endif // DATATYPE__HEADER
