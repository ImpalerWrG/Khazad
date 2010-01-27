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

    bool Load(TiXmlElement* Entry, int32_t Index);   // This must be called to index the Data Title
    virtual bool PostProcessing() = 0;

protected:

    string Name;
};

class DataLibraryBase
{

public:

    virtual void LoadElement(TiXmlElement* Element) = 0;
    virtual void PostProcessDataClass() = 0;
};

class ColorData: public DataBase
{

public:

    ColorData();
    ~ColorData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing() {};

    Uint8 getRed()      { return Red; }
    Uint8 getGreen()    { return Green; }
    Uint8 getBlue()     { return Blue; }

protected:

    Uint8 Red;
    Uint8 Green;
    Uint8 Blue;
};

class ColorDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<ColorData*> DataEntries;
};

class TextureData: public DataBase
{

public:

    TextureData();
    ~TextureData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing() {};

    Path getPath() { return sPath; }

    void setDevILID(Uint32 ID)      { DevILID = ID; }
    Uint32 getDevILID()             { return DevILID; }

protected:

    Path sPath;
    Uint32 DevILID;
};

class TextureDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<TextureData*> DataEntries;
};

class TextureGridData: public DataBase
{

public:

    TextureGridData();
    ~TextureGridData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing() {};

    Path getPath() { return sPath; }

    void setDevILID(Uint32 ID)      { DevILID = ID; }
    Uint32 getDevILID()             { return DevILID; }

protected:

    Path sPath;
    Uint32 DevILID;
};

class TextureGridDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<TextureGridData*> DataEntries;
};

class ModelData: public DataBase
{

public:

    ModelData();
    ~ModelData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing() {};

    Path getPath()                  { return sPath; }
    float getScalar()               { return Scalar; }

protected:

    Path sPath;
    float Scalar;
};

class ModelDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<ModelData*> DataEntries;
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

class SurfaceTypeDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<SurfaceTypeData*> DataEntries;
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

class TileGroupDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<TileGroupData*> DataEntries;
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

class MaterialClassDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<MaterialClassData*> DataEntries;
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

class MaterialDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<MaterialData*> DataEntries;
};

class FontData: public DataBase
{

public:

    FontData();
    ~FontData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing() {};

    Path getPath()    { return sPath; }
    Uint16 getSize()  { return Size; }

protected:

    Path sPath;
    Uint16 Size;
};

class FontDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<FontData*> DataEntries;
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

class TileShapeDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<TileShapeData*> DataEntries;
};

class TreeData: public DataBase
{

public:
    TreeData();
    ~TreeData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getModelID()                     { return ModelID; }
    string getMatgloss()                    { return Matgloss; }

    Sint16 getTrunkMaterialID()             { return TrunkMaterialID; }
    Sint16 getLeavesMaterialID()            { return LeavesMaterialID; }

protected:

    string TrunkMaterialLabel;
    Sint16 TrunkMaterialID;

    string LeavesMaterialLabel;
    Sint16 LeavesMaterialID;

    string Matgloss;

    Sint16 ModelID;
    string ModelLabel;
};

class TreeDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<TreeData*> DataEntries;
};

class BuildingData: public DataBase
{

public:
    BuildingData();
    ~BuildingData();

    bool Load(TiXmlElement* Element, Uint32 Index);
    bool PostProcessing();

    Sint16 getModelID()                     { return ModelID; }
    Sint16 getTextureID()                   { return TextureID; }
    Uint16 getMatgloss()                    { return Matgloss; }

protected:

    Uint16 Matgloss;

    Sint16 ModelID;
    Sint16 TextureID;

    string ModelLabel;
    string TextureLabel;
};

class BuildingDataLibrary: public DataLibraryBase
{

public:

    void LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<BuildingData*> DataEntries;
};

#endif // DATATYPE__HEADER
