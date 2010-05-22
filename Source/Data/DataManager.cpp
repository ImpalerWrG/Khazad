#include <DataManager.h>
//#include <Singleton.h>
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

    TextureGrids = new TextureGridDataLibrary();
    GlobalDataTypeMap["TextureGrid"] = TextureGrids;

    TextureSheets = new TextureSheetDataLibrary();
    GlobalDataTypeMap["TextureSheet"] = TextureSheets;

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

    // Safety null checks on documents
    TiXmlDocument* Document = XML->loadFile("..\\Media\\XML\\MasterFileList.xml");
    TiXmlElement* Parent = Document->RootElement();
    TiXmlElement* Iterator = Parent->FirstChildElement();

    for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement())
    {
        DataFiles.push_back(Path(Iterator->Attribute("Path")));
    }
    Document->~TiXmlDocument();  // Free the Document

    std::vector<Path>::iterator FileIterator;
    for (FileIterator = DataFiles.begin(); FileIterator != DataFiles.end(); FileIterator++)
    {
        LoadDataFile(*FileIterator);
    }

    std::map<string, DataLibraryBase*, ltstr>::iterator TypeIterator;
    for (TypeIterator = GlobalDataTypeMap.begin(); TypeIterator != GlobalDataTypeMap.end(); TypeIterator++)
    {
        TypeIterator->second->PostProcessDataClass();
    }

    return true;
}

int32_t DataManager::getLabelIndex(string Label)
{
    if(strcmp(Label.c_str(), "NONE") == 0)
    {
        return -1;
    }

    if(strcmp(Label.c_str(), "") == 0)
    {
        return -1;
    }

    std::map<string, uint32_t, ltstr>::iterator it = GlobalLabelMap.find(Label);

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

void DataManager::addLabel(string Label, uint32_t Index)
{
    GlobalLabelMap[Label] = Index;
}
