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

}

DataManager::~DataManager()
{

}

bool DataManager::Init()
{
    // Initial loading of all XML files
    LoadDataClass(&Textures, Path("Assets\\XML\\Textures.xml"), "Texture");
    LoadDataClass(&Materials, Path("Assets\\XML\\Materials.xml"), "Material");

    // Post process all data and dynamicly link references, not order dependent
	PostProcessDataClass(&Materials);

    return true;
}

Uint32 DataManager::getLabelIndex(string Label)
{
    std::map<string, Uint32, ltstr>::iterator iter;
    iter = GlobalLabelMap.find(Label);

	if(iter != GlobalLabelMap.end())
	{
		return iter->second;
	}
	else
	{
		cout << "Label is not in GlobalLabelMap" << endl;
		return 0;
	}
}

void DataManager::addLabel(string Label, Uint32 Index)
{
    GlobalLabelMap[Label] = Index;
}
