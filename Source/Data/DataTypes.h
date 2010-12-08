#ifndef DATATYPE__HEADER
#define DATATYPE__HEADER

#include <stdafx.h>
#include <Paths.h>

class TiXmlElement;

#define INVALID_INDEX -1

typedef unsigned short COLOR_INDEX;
typedef unsigned short TEXTURE_INDEX;
typedef unsigned short TEXTUREGRID_INDEX;
typedef unsigned short TEXTURESHEET_INDEX;
typedef unsigned short ANIMATION_TYPE_INDEX;
typedef unsigned short ANIMATION_GROUP_INDEX;
typedef unsigned short MODEL_INDEX;
typedef unsigned short SURFACE_INDEX;
typedef unsigned short TILEGROUP_INDEX;
typedef unsigned short MATERIALCLASS_INDEX;
typedef unsigned short MATERIAL_INDEX;
typedef unsigned short FONT_INDEX;
typedef unsigned short TILESHAPE_INDEX;
typedef unsigned short TREE_INDEX;
typedef unsigned short BUILDING_INDEX;



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

    virtual DataBase* LoadElement(TiXmlElement* Element) = 0;
    virtual void PostProcessDataClass() = 0;
};

class ColorData: public DataBase
{

public:

    ColorData();
    ~ColorData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {return true; };

    uint8_t getRed()      { return Red; }
    uint8_t getGreen()    { return Green; }
    uint8_t getBlue()     { return Blue; }

protected:

    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

class ColorDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<ColorData*> DataEntries;
};

class TextureData: public DataBase
{

public:

    TextureData();
    ~TextureData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {return true;};

    Path getPath()              { return sPath; }
    void setPath(Path newPath)  { sPath = newPath; }

    void setDevILID(uint32_t ID)      { DevILID = ID; }
    uint32_t getDevILID()             { return DevILID; }

    void setX(uint16_t X)    { x = X; }
    void setY(uint16_t Y)    { y = Y; }
    void setW(uint16_t W)    { w = W; }
    void setH(uint16_t H)    { h = H; }

    uint16_t getX()    { return x; }
    uint16_t getY()    { return y; }
    uint16_t getW()    { return w; }
    uint16_t getH()    { return h; }

    bool isLoneTexture()                { return LoneTexture; }
    bool setLoneTexture(bool NewValue)  { LoneTexture = NewValue; return true;}

protected:

    Path sPath;
    uint32_t DevILID;

    bool LoneTexture;     // Marks this texture as a whole file texture
    uint16_t x, y, h, w;  // Used for grid and sheet derived textures
};

class TextureDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<TextureData*> DataEntries;
};

class TextureGridData: public DataBase
{

public:

    TextureGridData();
    ~TextureGridData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {return true;};

    Path getPath() { return sPath; }

    void setDevILID(uint32_t ID)      { DevILID = ID; }
    uint32_t getDevILID()             { return DevILID; }

    std::vector<TextureData*> TextureList;

    uint16_t getTextureHeight()     { return TextureHeight; }
    uint16_t getTextureWidth()      { return TextureWidth; }

protected:

    Path sPath;
    uint32_t DevILID;

    uint16_t GridHeight, GridWidth;
    uint16_t TextureHeight, TextureWidth;
};

class TextureGridDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<TextureGridData*> DataEntries;
};

class TextureSheetData: public DataBase
{

public:

    TextureSheetData();
    ~TextureSheetData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {return true;};

    Path getPath() { return sPath; }

    void setDevILID(uint32_t ID)      { DevILID = ID; }
    uint32_t getDevILID()             { return DevILID; }

    std::vector<TextureData*> TextureList;

protected:

    Path sPath;
    uint32_t DevILID;
};

class TextureSheetDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<TextureSheetData*> DataEntries;
};

class AnimationTypeData: public DataBase
{

public:

    AnimationTypeData();
    ~AnimationTypeData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {};
};

class AnimationTypeDataLibrary: public DataLibraryBase
{
public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<AnimationTypeData*> DataEntries;
};

class AnimationGroupData: public DataBase
{

public:

    AnimationGroupData();
    ~AnimationGroupData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    int getAnimationStart(ANIMATION_TYPE_INDEX Type)    { return AnimationStartIndex[Type]; }
    int getAnimationLength(ANIMATION_TYPE_INDEX Type)   { return AnimationLength[Type]; }

    TEXTURE_INDEX getTexture(int AnimationIndex)        { return TextureList[AnimationIndex]; }
    int getTextureCount()                               { return TextureList.size(); }

protected:

    std::vector<string> TextureLabels;
    std::vector<TEXTURE_INDEX> TextureList;

    std::vector<string> AnimatinTypeLabels;
    std::vector<ANIMATION_TYPE_INDEX> AnimationTypeIndex;

    std::vector<uint16_t> AnimationStartIndex;
    std::vector<uint16_t> AnimationLength;
};

class AnimationGroupDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<AnimationGroupData*> DataEntries;
};

class ModelData: public DataBase
{

public:

    ModelData();
    ~ModelData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {return true;};

    Path getPath()                  { return sPath; }
    float getScalar()               { return Scalar; }

protected:

    Path sPath;
    float Scalar;
};

class ModelDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<ModelData*> DataEntries;
};

class SurfaceTypeData: public DataBase
{

public:

    SurfaceTypeData();
    ~SurfaceTypeData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    int16_t getTextureID()         { return TextureID; }

protected:

    string TextureLabel;
    int16_t TextureID;
};

class SurfaceTypeDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<SurfaceTypeData*> DataEntries;
};

class MaterialClassData: public DataBase
{

public:

    MaterialClassData();
    ~MaterialClassData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    int16_t getDefaultMaterial()     { return DefaultMaterialID; }

    int16_t getTexture(uint16_t SurfaceID)     { return TextureArray[SurfaceID]; }

protected:

    int16_t* TextureArray;

    string DefaultMaterialLabel;

    int16_t DefaultMaterialID;

    std::vector<string> SurfaceTypeLabels;
    std::vector<string> TextureLabels;
};

class MaterialClassDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<MaterialClassData*> DataEntries;
};

class MaterialData: public DataBase
{

public:

    MaterialData();
    ~MaterialData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    int16_t getMaterialClass()           { return MaterialClassID; }

    int16_t getPrimaryColorID()          { return PrimaryColorID; }
    int16_t getSecondaryColorID()        { return SecondaryColorID; }
    int16_t getBorderColorID()           { return BorderColorID; }
    string getColorMode()               { return ColorMode; }

    int16_t getTexture(uint16_t SurfaceID)     { return TextureArray[SurfaceID]; }

protected:

    int16_t* TextureArray;

    string PrimaryColorLabel;
    string SecondaryColorLabel;
    string BorderColorLabel;

    string MaterialClassLabel;

    string ColorMode;

    int16_t PrimaryColorID;
    int16_t SecondaryColorID;
    int16_t BorderColorID;

    int16_t MaterialClassID;

    std::vector<string> SurfaceTypeLabels;
    std::vector<string> TextureLabels;
};

class MaterialDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<MaterialData*> DataEntries;
};

class FontData: public DataBase
{

public:

    FontData();
    ~FontData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing() {return true;};

    Path getPath()    { return sPath; }
    uint16_t getSize()  { return Size; }

protected:

    Path sPath;
    uint16_t Size;
};

class FontDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass() {};

    std::vector<FontData*> DataEntries;
};

class TileShapeData: public DataBase
{

public:
    TileShapeData();
    ~TileShapeData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    bool isOpen()               { return Open; }

    int16_t getModelID()         { return ModelID; }

protected:

    bool Open;

    int16_t ModelID;
    string ModelLabel;
};

class TileShapeDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<TileShapeData*> DataEntries;
};

class TreeData: public DataBase
{

public:
    TreeData();
    ~TreeData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    int16_t getModelID()                     { return ModelID; }

    int16_t getTrunkMaterialID()             { return TrunkMaterialID; }
    int16_t getLeavesMaterialID()            { return LeavesMaterialID; }
    int16_t getTextureID()                   { return TextureID; }

protected:

    string TrunkMaterialLabel;
    int16_t TrunkMaterialID;

    string LeavesMaterialLabel;
    int16_t LeavesMaterialID;

    int16_t ModelID;
    string ModelLabel;

    int16_t TextureID;
    string TextureLabel;
};

class TreeDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<TreeData*> DataEntries;
};

class BuildingData: public DataBase
{

public:
    BuildingData();
    ~BuildingData();

    bool Load(TiXmlElement* Element, uint32_t Index);
    bool PostProcessing();

    int16_t getModelID()                     { return ModelID; }
    int16_t getTextureID()                   { return TextureID; }
    uint16_t getMatgloss()                    { return Matgloss; }

protected:

    uint16_t Matgloss;

    int16_t ModelID;
    int16_t TextureID;

    string ModelLabel;
    string TextureLabel;
};

class BuildingDataLibrary: public DataLibraryBase
{

public:

    DataBase* LoadElement(TiXmlElement* Element);
    void PostProcessDataClass();

    std::vector<BuildingData*> DataEntries;
};

#endif // DATATYPE__HEADER
