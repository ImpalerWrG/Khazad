#ifndef DATA__HEADER
#define DATA__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <DataTypes.h>
#include <XMLManager.h>


struct ltstr
{
    bool operator()(string s1, string s2) const
    {
        return strcmp(s1.c_str(), s2.c_str()) < 0;
    }
};

class DataManager
{
	DECLARE_SINGLETON_CLASS(DataManager)

public:

    ~DataManager();
    bool Init();

    bool LoadDataFile(const char* Path)
    {
        cout << "Loading " << Path << endl;

        // Safety null checks on documents
        TiXmlDocument* Document = XML->loadFile(Path);
        TiXmlElement* Parent = Document->RootElement();
        TiXmlElement* Iterator = Parent->FirstChildElement();

        for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement())
        {
            DataLibraryBase* TargetLibrary = getDataTypeGroup(Iterator->Value());
            if (TargetLibrary != NULL)
            {
                TargetLibrary->LoadElement(Iterator);
            }
        }

        Document->~TiXmlDocument();  // Free the Document
        return true;
    }

    ColorData* getColorData(uint32_t Index)                    { return Colors->DataEntries[Index]; }
    uint32_t getNumColors()                                    { return Colors->DataEntries.size(); }
    ColorDataLibrary* getColorDataLibrary()                  { return Colors; }

    TextureData* getTextureData(uint32_t Index)                { return Textures->DataEntries[Index]; }
    uint32_t getNumTextures()                                  { return Textures->DataEntries.size(); }
    TextureDataLibrary* getTextureDataLibrary()              { return Textures; }

    TextureGridData* getTextureGridData(uint32_t Index)        { return TextureGrids->DataEntries[Index]; }
    uint32_t getNumTextureGrids()                              { return TextureGrids->DataEntries.size(); }
    TextureGridDataLibrary* getTextureGridDataLibrary()      { return TextureGrids; }

    TextureSheetData* getTextureSheetData(uint32_t Index)      { return TextureSheets->DataEntries[Index]; }
    uint32_t getNumTextureSheets()                             { return TextureSheets->DataEntries.size(); }
    TextureSheetDataLibrary* getTextureSheetDataLibrary()    { return TextureSheets; }

    ModelData* getModelData(uint32_t Index)                    { return Models->DataEntries[Index]; }
    uint32_t getNumModels()                                    { return Models->DataEntries.size(); }
    ModelDataLibrary* getModelDataLibrary()                  { return Models; }

    FontData* getFontData(uint32_t Index)                      { return Fonts->DataEntries[Index]; }
    uint32_t getNumFonts()                                     { return Fonts->DataEntries.size(); }
    FontDataLibrary* getFontDataLibrary()                    { return Fonts; }

    MaterialData* getMaterialData(uint32_t Index)              { return Materials->DataEntries[Index]; }
    uint32_t getNumMaterials()                                 { return Materials->DataEntries.size(); }
    MaterialDataLibrary* getMaterialDataLibrary()            { return Materials; }

    TileGroupData* getTileGroupData(uint32_t Index)            { return TileGroups->DataEntries[Index]; }
    uint32_t getNumTileGroups()                                { return TileGroups->DataEntries.size(); }
    TileGroupDataLibrary* getTileGroupDataLibrary()          { return TileGroups; }

    MaterialClassData* getMaterialClassData(uint32_t Index)    { return MaterialClasses->DataEntries[Index]; }
    uint32_t getNumMaterialClasses()                           { return MaterialClasses->DataEntries.size(); }
    MaterialClassDataLibrary* getMaterialClassDataLibrary()  { return MaterialClasses; }

    SurfaceTypeData* getSurfaceTypeData(uint32_t Index)        { return SurfaceTypes->DataEntries[Index]; }
    uint32_t getNumSurfaceTypes()                              { return SurfaceTypes->DataEntries.size(); }
    SurfaceTypeDataLibrary* getSurfaceTypeDataLibrary()      { return SurfaceTypes; }

    TileShapeData* getTileShapeData(uint32_t Index)            { return TileShapes->DataEntries[Index]; }
    uint32_t getNumTileShape()                                 { return TileShapes->DataEntries.size(); }
    TileShapeDataLibrary* getTileShapeDataLibrary()          { return TileShapes; }

    TreeData* getTreeData(uint32_t Index)                      { return Trees->DataEntries[Index]; }
    uint32_t getNumTrees()                                     { return Trees->DataEntries.size(); }
    TreeDataLibrary* getTreeDataLibrary()                    { return Trees; }

    BuildingData* getBuildingData(uint32_t Index)              { return Buildings->DataEntries[Index]; }
    uint32_t getNumBuildings()                                 { return Buildings->DataEntries.size(); }
    BuildingDataLibrary* getBuildingDataLibrary()            { return Buildings; }

    // ADD New Data classes gets Here

    int32_t getLabelIndex(string Label);
    void addLabel(string Label, uint32_t Index);

    DataLibraryBase* getDataTypeGroup(string ElementType);

protected:

    std::map<string, uint32_t, ltstr> GlobalLabelMap;
    std::map<string, DataLibraryBase*, ltstr> GlobalDataTypeMap;

    // ADD New Data classes Libraries Here

    ColorDataLibrary* Colors;
    TextureDataLibrary* Textures;
    TextureGridDataLibrary* TextureGrids;
    TextureSheetDataLibrary* TextureSheets;
    ModelDataLibrary* Models;
    FontDataLibrary* Fonts;
    MaterialDataLibrary* Materials;
    TileGroupDataLibrary* TileGroups;
    MaterialClassDataLibrary* MaterialClasses;
    SurfaceTypeDataLibrary* SurfaceTypes;
    TileShapeDataLibrary* TileShapes;
    TreeDataLibrary* Trees;
    BuildingDataLibrary* Buildings;
};


#define DATA (DataManager::GetInstance())

#endif // DATA__HEADER
