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
        return true;
    }
    return false;
}

ColorData::ColorData()
{}

ColorData::~ColorData()
{}

void ColorData::setValue (string & value)
{
    const char * cvalue = value.c_str();
    int red;
    int green;
    int blue;
    if(sscanf(cvalue,"#%2x%2x%2x", &red, &green, &blue ))
    {
        Red = red;
        Green = green;
        Blue = blue;
    }
    else if(sscanf(cvalue,"rgb ( %d , %d , %d )", &red, &green, &blue ))
    {
        Red = red;
        Green = green;
        Blue = blue;
    }
    // FIXME: silently fail? really?
}

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
{
    Border = true;
    PrimaryColorIsLabel = false;
    SecondaryColorIsLabel = false;
}

MaterialData::~MaterialData()
{}

bool MaterialData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "Texture", "label", TextureLabel);
        // first we check for by-label colors
        if(XML->QueryTextValue(Entry, "PrimaryColor", "label", PrimaryColorLabel))
            PrimaryColorIsLabel = true;
        if(XML->QueryTextValue(Entry, "SecondaryColor", "label", SecondaryColorLabel))
            SecondaryColorIsLabel = true;
        // then we check if we have by-value colors
        string PrimaryColorValue;
        if(XML->QueryTextValue(Entry, "PrimaryColor", "value", PrimaryColorValue))
        {
            MyPrimary.setValue(PrimaryColorValue);
        }
        string SecondaryColorValue;
        if(XML->QueryTextValue(Entry, "PrimaryColor", "value", SecondaryColorValue))
        {
            MySecondary.setValue(SecondaryColorValue);
        }
        XML->QueryUIntValue(Entry, "Hardness", "Int", Hardness);
        Border = XML->QueryExists(Entry, "Border");
        XML->QueryTextValue(Entry, "MatGloss", "label", MatGloss);
        XML->QueryTextValue(Entry, "ColorMode", "mode", ColorMode);
        XML->QueryUIntArray(Entry, "TileValues", "Tile", "Int", TileTypes);

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
ColorData MaterialData::getPrimaryColor()
{
    if(PrimaryColorIsLabel == true)
        return *DATA->getColorData(PrimaryColorID);
    return MyPrimary;
};
ColorData MaterialData::getSecondaryColor()
{
    if(SecondaryColorIsLabel == true)
        return *DATA->getColorData(SecondaryColorID);
    return MySecondary;
};
FontData::FontData()
{}

FontData::~FontData()
{}

bool FontData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", temp);
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
        XML->QueryTextValue(Entry, "File", "Path", temp);
        sPath = temp;
        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

