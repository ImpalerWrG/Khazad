#include <DataTypes.h>
#include <XMLManager.h>
#include <DataManager.h>


DataBase::DataBase()
{}

DataBase::~DataBase()
{}

bool DataBase::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "Name", "Label", Name);
        DATA->addLabel(Name, Index);
    }
}

MaterialData::MaterialData()
{}

MaterialData::~MaterialData()
{}

bool MaterialData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "Texture", "label", TextureLabel);
        XML->QueryUIntValue(Entry, "Hardness", "Int", Hardness);

        XML->QueryUIntArray(Entry, "TileValues", "Tile", "Int", &TileTypes);


        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool MaterialData::PostProcessing()
{
    TextureID = DATA->getLabelIndex(TextureLabel);

    return true;
}

TextureData::TextureData()
{}

TextureData::~TextureData()
{}

bool TextureData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "File", "Path", Path);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

