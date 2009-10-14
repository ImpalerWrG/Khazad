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

/*
class TileClassData: public DataBase
{

public:

    TileClassData();
    ~TileClassData();
    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Uint16 getSurfaceType()     { return SurfaceTypeID; }
    Uint16 getTileShape()       { return TileShapeID; }

protected:

    Uint16 SurfaceTypeID;
    string SurfaceTypeLabel;

    Uint16 TileShapeID;
    string TileShapeLabel;
};
*/

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

    Sint16 getTextureID()       { return TextureID; }
    Sint16 getPrimaryColor()    { return PrimaryColorID; }
    Sint16 getSecondaryColor()  { return SecondaryColorID; }
    Sint16 getMaterialClass()   { return MaterialClassID; }

    std::vector<Uint16> TileValues;
    std::vector<Uint16> TileShapeID;
    std::vector<Uint16> SurfaceTypeID;

protected:

    string TextureLabel;
    string PrimaryColorLabel;
    string SecondaryColorLabel;
    string MaterialClassLabel;

    std::vector<string> TileShapeLabels;
    std::vector<string> SurfaceTypeLabels;

    Sint16 TextureID;
    Sint16 PrimaryColorID;
    Sint16 SecondaryColorID;
    Sint16 MaterialClassID;
};

class MaterialClassData: public DataBase
{

public:

    MaterialClassData();
    ~MaterialClassData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    bool isOverRidesMatGloss()    { return OverRidesMatGloss; }

    Sint16 getTextureID()       { return TextureID; }
    Sint16 getPrimaryColor()    { return PrimaryColorID; }
    Sint16 getSecondaryColor()  { return SecondaryColorID; }
    Sint16 getBorderColor()     { return BorderColorID; }

    Sint16 getMatGloosIndex()   { return MatGlossIndex; }

protected:

    string TextureLabel;
    string PrimaryColorLabel;
    string SecondaryColorLabel;
    string BorderColorLabel;

    bool OverRidesMatGloss;

    Sint16 TextureID;
    Sint16 PrimaryColorID;
    Sint16 SecondaryColorID;
    Sint16 BorderColorID;

    Sint16 MatGlossIndex;
};

class MaterialData: public DataBase
{

public:

    MaterialData();
    ~MaterialData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getTextureID()               { return TextureID; }
    Sint16 getPrimaryColorID()          { return PrimaryColorID; }
    Sint16 getSecondaryColorID()        { return SecondaryColorID; }
    Sint16 getBorderColorID()           { return BorderColorID; }

    string getMatGloss()                { return MatGloss; }
    string getColorMode()               { return ColorMode; }
    bool getBorder()                    { return Border; }

    std::vector<Uint16> TileTypes;

protected:

    string TextureLabel;
    string PrimaryColorLabel;
    string SecondaryColorLabel;
    string BorderColorLabel;

    string MaterialClassLabel;

    string MatGloss;
    string ColorMode;

    bool Border;

    Sint16 TextureID;
    Sint16 PrimaryColorID;
    Sint16 SecondaryColorID;
    Sint16 BorderColorID;

    Sint16 MaterialClassID;
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

    bool isOpen()               { return Open; }

protected:

    bool Open;

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
        TreeVariant getVariant(Uint32 idx)
        {
            return variants[idx];
        };
        string getModel() {return ModelFile;};
        string getMatgloss() {return Matgloss;};
    protected:
        vector <TreeVariant> variants;
        string ModelFile;
        string Matgloss;
};

#endif // DATATYPE__HEADER
