#include <DataTypes.h>
#include <XMLManager.h>
#include <DataManager.h>
#include "../../TinyXML/tinyxml.h"

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
        XML->QueryTextValue(Entry, "MaterialClass", "label", MaterialClassLabel);
        XML->QueryTextValue(Entry, "Material", "label", MaterialLabel);

        XML->QueryUIntArray(Entry, "TileValues", "TileValue", "Int", TileValues);
        XML->QueryTextArray(Entry, "TileValues", "TileValue", "TileShape", &TileShapeLabels);
        XML->QueryTextArray(Entry, "TileValues", "TileValue", "SurfaceType", &SurfaceTypeLabels);

        DataBase::Load(Entry, Index);

        return true;
    }
    return false;
}

bool TileGroupData::PostProcessing()
{
    MaterialClassID = DATA->getLabelIndex(MaterialClassLabel);
    MaterialID = DATA->getLabelIndex(MaterialLabel);

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

        XML->QueryTextValue(Entry, "DefaultMaterial", "label", DefaultMaterialLabel);

        XML->QueryTextArray(Entry, "DefaultSurfaceTextures", "SurfaceTexture", "SurfaceType", &SurfaceTypeLabels);
        XML->QueryTextArray(Entry, "DefaultSurfaceTextures", "SurfaceTexture", "Texture", &TextureLabels);

        XML->QueryUIntValue(Entry, "MatGlossIndex", "int", MatGlossIndex);

        return true;
    }
    return false;
}

bool MaterialClassData::PostProcessing()
{
    DefaultMaterialID = DATA->getLabelIndex(DefaultMaterialLabel);

    int size = DATA->getNumSurfaceTypes();
    TextureArray = new Sint16[size];

    for (int i = 0; i < size; i++)
    {
        TextureArray[i] = -1;
    }

    for (int i = 0; i < SurfaceTypeLabels.size(); i++)
    {
        Sint16 SurfaceTypeID = DATA->getLabelIndex(SurfaceTypeLabels[i]);
        if(SurfaceTypeID != -1)
        {
            TextureArray[SurfaceTypeID] = DATA->getLabelIndex(TextureLabels[i]);
        }
    }

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
        XML->QueryTextValue(Entry, "MaterialClass", "label", MaterialClassLabel);

        XML->QueryTextValue(Entry, "PrimaryColor", "label", PrimaryColorLabel);
        XML->QueryTextValue(Entry, "SecondaryColor", "label", SecondaryColorLabel);
        XML->QueryTextValue(Entry, "BorderColor", "label", BorderColorLabel);

        XML->QueryTextValue(Entry, "MatGloss", "label", MatGloss);
        XML->QueryTextValue(Entry, "ColorMode", "mode", ColorMode);

        XML->QueryTextArray(Entry, "SurfaceTextures", "SurfaceTexture", "SurfaceType", &SurfaceTypeLabels);
        XML->QueryTextArray(Entry, "SurfaceTextures", "SurfaceTexture", "Texture", &TextureLabels);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool MaterialData::PostProcessing()
{
    PrimaryColorID = DATA->getLabelIndex(PrimaryColorLabel);
    SecondaryColorID = DATA->getLabelIndex(SecondaryColorLabel);
	BorderColorID = DATA->getLabelIndex(BorderColorLabel);

    MaterialClassID = DATA->getLabelIndex(MaterialClassLabel);

    int size = DATA->getNumSurfaceTypes();
    TextureArray = new Sint16[size];

    for (int i = 0; i < size; i++)
    {
        TextureArray[i] = -1;
    }

    for(int i = 0; i < SurfaceTypeLabels.size(); i++)
    {
        Sint16 SurfaceTypeID = DATA->getLabelIndex(SurfaceTypeLabels[i]);
        if(SurfaceTypeID != -1)
        {
            TextureArray[SurfaceTypeID] = DATA->getLabelIndex(TextureLabels[i]);
        }
    }

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

ModelData::ModelData()
{
    Scalar = 1.0;
}

ModelData::~ModelData()
{}

bool ModelData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", temp);
        XML->QueryFloatValue(Entry, "Scale", "value", Scalar);

        sPath = temp;
        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

TileShapeData::TileShapeData()
{
    Open = false;

    ModelID = -1;
    ModelLabel = "NONE";
}

TileShapeData::~TileShapeData()
{}

bool TileShapeData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryBoolValue(Entry, "Open", "bool", Open);
        XML->QueryTextValue(Entry, "Model", "label", ModelLabel);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool TileShapeData::PostProcessing()
{
    ModelID = DATA->getLabelIndex(ModelLabel);
}

TreeData::TreeData()
{}

TreeData::~TreeData()
{}

bool TreeData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "MatGloss", "value", Matgloss);
        XML->QueryTextValue(Entry, "Model", "label", ModelLabel);

        TiXmlElement *Variant = Entry->FirstChildElement("Variant");
        do
        {
            TreeVariant var;
            XML->QueryTextValue(Variant, "Trunk", "material", var.TrunkMaterial);
            XML->QueryTextValue(Variant, "Leaves", "material", var.LeavesMaterial);
            XML->QueryTextValue(Variant, "Snow", "material", var.SnowMaterial);
            variants.push_back(var);
            Variant= Variant->NextSiblingElement("Variant");
        }
        while(Variant);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool TreeData::PostProcessing()
{
    ModelID = DATA->getLabelIndex(ModelLabel);

    for(int i = 0; i < variants.size();i++)
    {
        variants[i].LeavesMaterialID = DATA->getLabelIndex(variants[i].LeavesMaterial);
        variants[i].TrunkMaterialID = DATA->getLabelIndex(variants[i].TrunkMaterial);
        variants[i].SnowMaterialID = DATA->getLabelIndex(variants[i].SnowMaterial);
    }
    return true;
}

BuildingData::BuildingData()
{
    ModelID = - 1;
    TextureID = - 1;
}

BuildingData::~BuildingData()
{}

bool BuildingData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryUIntValue(Entry, "MatGloss", "value", Matgloss);
        XML->QueryTextValue(Entry, "Model", "label", ModelLabel);
        XML->QueryTextValue(Entry, "Texture", "label", TextureLabel);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool BuildingData::PostProcessing()
{
    ModelID = DATA->getLabelIndex(ModelLabel);
    TextureID = DATA->getLabelIndex(TextureLabel);

    return true;
}
