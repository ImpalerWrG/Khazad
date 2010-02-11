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

    ColorData* getColorData(Uint32 Index)                    { return Colors->DataEntries[Index]; }
    Uint32 getNumColors()                                    { return Colors->DataEntries.size(); }
    ColorDataLibrary* getColorDataLibrary()                  { return Colors; }

    TextureData* getTextureData(Uint32 Index)                { return Textures->DataEntries[Index]; }
    Uint32 getNumTextures()                                  { return Textures->DataEntries.size(); }
    TextureDataLibrary* getTextureDataLibrary()              { return Textures; }

    TextureGridData* getTextureGridData(Uint32 Index)        { return TextureGrids->DataEntries[Index]; }
    Uint32 getNumTextureGrids()                              { return TextureGrids->DataEntries.size(); }
    TextureGridDataLibrary* getTextureGridDataLibrary()      { return TextureGrids; }

    TextureSheetData* getTextureSheetData(Uint32 Index)      { return TextureSheets->DataEntries[Index]; }
    Uint32 getNumTextureSheets()                             { return TextureSheets->DataEntries.size(); }
    TextureSheetDataLibrary* getTextureSheetDataLibrary()    { return TextureSheets; }

    ModelData* getModelData(Uint32 Index)                    { return Models->DataEntries[Index]; }
    Uint32 getNumModels()                                    { return Models->DataEntries.size(); }
    ModelDataLibrary* getModelDataLibrary()                  { return Models; }

    FontData* getFontData(Uint32 Index)                      { return Fonts->DataEntries[Index]; }
    Uint32 getNumFonts()                                     { return Fonts->DataEntries.size(); }
    FontDataLibrary* getFontDataLibrary()                    { return Fonts; }

    MaterialData* getMaterialData(Uint32 Index)              { return Materials->DataEntries[Index]; }
    Uint32 getNumMaterials()                                 { return Materials->DataEntries.size(); }
    MaterialDataLibrary* getMaterialDataLibrary()            { return Materials; }

    TileGroupData* getTileGroupData(Uint32 Index)            { return TileGroups->DataEntries[Index]; }
    Uint32 getNumTileGroups()                                { return TileGroups->DataEntries.size(); }
    TileGroupDataLibrary* getTileGroupDataLibrary()          { return TileGroups; }

    MaterialClassData* getMaterialClassData(Uint32 Index)    { return MaterialClasses->DataEntries[Index]; }
    Uint32 getNumMaterialClasses()                           { return MaterialClasses->DataEntries.size(); }
    MaterialClassDataLibrary* getMaterialClassDataLibrary()  { return MaterialClasses; }

    SurfaceTypeData* getSurfaceTypeData(Uint32 Index)        { return SurfaceTypes->DataEntries[Index]; }
    Uint32 getNumSurfaceTypes()                              { return SurfaceTypes->DataEntries.size(); }
    SurfaceTypeDataLibrary* getSurfaceTypeDataLibrary()      { return SurfaceTypes; }

    TileShapeData* getTileShapeData(Uint32 Index)            { return TileShapes->DataEntries[Index]; }
    Uint32 getNumTileShape()                                 { return TileShapes->DataEntries.size(); }
    TileShapeDataLibrary* getTileShapeDataLibrary()          { return TileShapes; }

    TreeData* getTreeData(Uint32 Index)                      { return Trees->DataEntries[Index]; }
    Uint32 getNumTrees()                                     { return Trees->DataEntries.size(); }
    TreeDataLibrary* getTreeDataLibrary()                    { return Trees; }

    BuildingData* getBuildingData(Uint32 Index)              { return Buildings->DataEntries[Index]; }
    Uint32 getNumBuildings()                                 { return Buildings->DataEntries.size(); }
    BuildingDataLibrary* getBuildingDataLibrary()            { return Buildings; }

    // ADD New Data classes gets Here

    Sint32 getLabelIndex(string Label);
    void addLabel(string Label, Uint32 Index);

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
