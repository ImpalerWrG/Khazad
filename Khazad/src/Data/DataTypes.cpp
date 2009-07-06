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
        XML->QueryTextValue(Entry, "Name", "Label", &Name);
        DATA->addLabel(Name, Index);
    }
}

ColorData::ColorData()
{}

ColorData::~ColorData()
{}

bool ColorData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryUIntValue(Entry, "Channels", "Red", Red);
        XML->QueryUIntValue(Entry, "Channels", "Green", Green);
        XML->QueryUIntValue(Entry, "Channels", "Blue", Blue);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

MaterialData::MaterialData()
{}

MaterialData::~MaterialData()
{}

bool MaterialData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "Texture", "label", &TextureLabel);
        XML->QueryTextValue(Entry, "PrimaryColor", "label", &PrimaryColorLabel);
        XML->QueryTextValue(Entry, "SecondaryColor", "label", &SecondaryColorLabel);

        XML->QueryUIntValue(Entry, "Hardness", "Int", Hardness);
        XML->QueryTextValue(Entry, "MatGloss", "label", &MatGloss);
        XML->QueryUIntArray(Entry, "TileValues", "Tile", "Int", &TileTypes);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool MaterialData::PostProcessing()
{
    TextureID = DATA->getLabelIndex(TextureLabel);
    PrimaryColorID = DATA->getLabelIndex(PrimaryColorLabel);
    SecondaryColorID = DATA->getLabelIndex(SecondaryColorLabel);

    return true;
}

FontData::FontData()
{}

FontData::~FontData()
{}

bool FontData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", &temp);
        XML->QueryUIntValue(Entry, "Size", "Int", Size);

        sPath = temp;
        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

TextureData::TextureData()
{}

TextureData::~TextureData()
{}

bool TextureData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", &temp);
        sPath = temp;
        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

