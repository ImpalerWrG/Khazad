#include <DataManager.h>
#include <Singleton.h>
#include <XMLManager.h>
#include <DataTypes.h>

#include <tinystr.h>
#include <tinyxml.h>


DECLARE_SINGLETON(DataManager)

DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

bool DataManager::Init()
{
    //Try to grab all XML files
    LoadDataClass("Assets\\XML\\Materials.xml", "Material");


    //Post process all data and dynamicly link references
    return true;
}

bool DataManager::LoadDataClass(char* Path, char* Entry)
{
    // Templatize this ?
	TiXmlDocument* Document = XML->loadFile(Path);
    TiXmlElement* Parent = Document->RootElement();
    TiXmlElement* Iterator = Parent->FirstChildElement( Entry );

	for(int i = 0; Iterator != NULL; Iterator = Iterator->NextSiblingElement(Entry))
	{
	    //TODO allow the type of Element to fork to different loading types so mixed files are possible
        MaterialData* NewMaterial = new MaterialData;
        NewMaterial->Load(Iterator);
        Materials.push_back(NewMaterial);
	}

    return true;
}
