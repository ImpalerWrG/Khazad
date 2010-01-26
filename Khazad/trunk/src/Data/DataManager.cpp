#include <DataManager.h>
#include <Singleton.h>
#include <XMLManager.h>
#include <DataTypes.h>

#include <tinystr.h>
#include <tinyxml.h>
#include <Paths.h>


DECLARE_SINGLETON(DataManager)

DataManager::DataManager()
{
    Colors = new ColorDataLibrary();
    GlobalDataTypeMap["Color"] = Colors;

    Textures = new TextureDataLibrary();
    GlobalDataTypeMap["Texture"] = Textures;

    Models = new ModelDataLibrary();
    GlobalDataTypeMap["Model"] = Models;

    Fonts = new FontDataLibrary();
    GlobalDataTypeMap["Font"] = Fonts;

    Materials = new MaterialDataLibrary();
    GlobalDataTypeMap["Material"] = Materials;

    TileGroups = new TileGroupDataLibrary();
    GlobalDataTypeMap["TileGroup"] = TileGroups;

    MaterialClasses = new MaterialClassDataLibrary();
    GlobalDataTypeMap["MaterialClass"] = MaterialClasses;

    SurfaceTypes = new SurfaceTypeDataLibrary();
    GlobalDataTypeMap["SurfaceType"] = SurfaceTypes;

    TileShapes = new TileShapeDataLibrary();
    GlobalDataTypeMap["TileShape"] = TileShapes;

    Trees = new TreeDataLibrary();
    GlobalDataTypeMap["Tree"] = Trees;

    Buildings = new BuildingDataLibrary();
    GlobalDataTypeMap["Building"] = Buildings;
}

DataManager::~DataManager()
{

}

bool DataManager::Init()
{
    std::vector<Path> DataFiles;

    // master file reading from xml?
    DataFiles.push_back(Path("Assets\\XML\\Colors.xml"));
    DataFiles.push_back(Path("Assets\\XML\\Textures.xml"));
    DataFiles.push_back(Path("Assets\\XML\\Models.xml"));
    DataFiles.push_back(Path("Assets\\XML\\Fonts.xml"));
    DataFiles.push_back(Path("Assets\\XML\\Materials.xml"));
    DataFiles.push_back(Path("Assets\\XML\\TileGroups.xml"));
    DataFiles.push_back(Path("Assets\\XML\\MaterialClasses.xml"));
    DataFiles.push_back(Path("Assets\\XML\\SurfaceTypes.xml"));
    DataFiles.push_back(Path("Assets\\XML\\TileShapes.xml"));
    DataFiles.push_back(Path("Assets\\XML\\Trees.xml"));
    DataFiles.push_back(Path("Assets\\XML\\Buildings.xml"));

    std::vector<Path>::iterator FileIterator;
    for (FileIterator = DataFiles.begin(); FileIterator != DataFiles.end(); FileIterator++)
    {
        LoadDataFile(*FileIterator);
    }

    std::map<string, DataLibraryBase*, ltstr>::iterator TypeIterator;
    for (TypeIterator = GlobalDataTypeMap.begin(); TypeIterator != GlobalDataTypeMap.end(); TypeIterator++)
    {
        (*TypeIterator).second->PostProcessDataClass();
    }

    return true;
}

Sint32 DataManager::getLabelIndex(string Label)
{
    if(strcmp(Label.c_str(), "NONE") == 0)
    {
        return -1;
    }

    if(strcmp(Label.c_str(), "") == 0)
    {
        return -1;
    }

    std::map<string, Uint32, ltstr>::iterator it = GlobalLabelMap.find(Label);

    if(it != GlobalLabelMap.end())
    {
        return it->second;
    }
    else
    {
        cout << Label << " is not in GlobalLabelMap" << endl;
        return -1;
    }
}

DataLibraryBase* DataManager::getDataTypeGroup(string ElementType)
{
    std::map<string, DataLibraryBase*, ltstr>::iterator it = GlobalDataTypeMap.find(ElementType);

    if(it != GlobalDataTypeMap.end())
    {
        return it->second;
    }
    else
    {
        cout << ElementType << " is not in GlobalDataTypeMap" << endl;
        return NULL;
    }
}

void DataManager::addLabel(string Label, Uint32 Index)
{
    GlobalLabelMap[Label] = Index;
}
