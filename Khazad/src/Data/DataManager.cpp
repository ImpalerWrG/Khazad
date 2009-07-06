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
    Path color_xml = "Assets\\XML\\Colors.xml";
    Path texture_xml = "Assets\\XML\\Textures.xml";
    Path font_xml = "Assets\\XML\\Fonts.xml";
    Path material_xml = "Assets\\XML\\Materials.xml";

    // Initial loading of all XML files
    LoadDataClass(&Colors, color_xml, "Color");
    LoadDataClass(&Textures, texture_xml, "Texture");
    LoadDataClass(&Fonts, font_xml, "Font");
    LoadDataClass(&Materials, material_xml, "Material");

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
