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

bool ColorData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryUIntValue(Entry, "Channels", "Red", Red);
        XML->QueryUIntValue(Entry, "Channels", "Green", Green);
        XML->QueryUIntValue(Entry, "Channels", "Blue", Blue);

        string HexString;

        if(XML->QueryTextValue(Entry, "HexValue", "value", HexString))
        {
            int red, green, blue;
            const char* ConvertedHexString = HexString.c_str();

            sscanf(ConvertedHexString,"%2x%2x%2x", &red, &green, &blue);
            Red = red; Green = green; Blue = blue;
        }

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

SurfaceTypeData::SurfaceTypeData()
{}

SurfaceTypeData::~SurfaceTypeData()
{}

bool SurfaceTypeData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "TextureUsed", "label", TextureLabel);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool SurfaceTypeData::PostProcessing()
{
    TextureID = DATA->getLabelIndex(TextureLabel);

    return true;
}

TileGroupData::TileGroupData()
{}

TileGroupData::~TileGroupData()
{}

bool TileGroupData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "Texture", "label", TextureLabel);
        XML->QueryTextValue(Entry, "PrimaryColor", "label", PrimaryColorLabel);
        XML->QueryTextValue(Entry, "SecondaryColor", "label", SecondaryColorLabel);
        XML->QueryTextValue(Entry, "MaterialClass", "label", MaterialClassLabel);

        XML->QueryTextArray(Entry, "TileValues", "TileValue", "TileShape", &TileShapeLabels);
        XML->QueryTextArray(Entry, "TileValues", "TileValue", "SurfaceType", &SurfaceTypeLabels);

        XML->QueryUIntArray(Entry, "TileValues", "TileValue", "Int", TileValues);

        DataBase::Load(Entry, Index);

        return true;
    }
    return false;
}

bool TileGroupData::PostProcessing()
{
    TextureID = DATA->getLabelIndex(TextureLabel);
    PrimaryColorID = DATA->getLabelIndex(PrimaryColorLabel);
    SecondaryColorID = DATA->getLabelIndex(SecondaryColorLabel);
    MaterialClassID = DATA->getLabelIndex(MaterialClassLabel);

    for(int i = 0; i < TileShapeLabels.size(); i++)
    {
        TileShapeID.push_back(DATA->getLabelIndex(TileShapeLabels[i]));
    }
    for(int i = 0; i < SurfaceTypeLabels.size(); i++)
    {
        SurfaceTypeID.push_back(DATA->getLabelIndex(SurfaceTypeLabels[i]));
    }

    return true;
}

MaterialClassData::MaterialClassData()
{}

MaterialClassData::~MaterialClassData()
{}

bool MaterialClassData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        DataBase::Load(Entry, Index);

        XML->QueryBoolValue(Entry, "OverRidesMatGloss", "bool", OverRidesMatGloss);

        return true;
    }
    return false;
}

bool MaterialClassData::PostProcessing()
{
    return true;
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

        XML->QueryTextValue(Entry, "PrimaryColor", "label", PrimaryColorLabel);
        XML->QueryTextValue(Entry, "SecondaryColor", "label", SecondaryColorLabel);
        XML->QueryTextValue(Entry, "BorderColor", "label", BorderColorLabel);

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
	BorderColorID = DATA->getLabelIndex(BorderColorLabel);

    MaterialClassID = DATA->getLabelIndex(MaterialClassLabel);

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

TileShapeData::TileShapeData()
{}

TileShapeData::~TileShapeData()
{}

bool TileShapeData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryBoolValue(Entry, "Open", "bool", Open);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}
