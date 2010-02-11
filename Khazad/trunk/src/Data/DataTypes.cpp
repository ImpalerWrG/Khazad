#include <DataTypes.h>
#include <XMLManager.h>
#include <DataManager.h>
#include "../../TinyXML/tinyxml.h"


DataBase::DataBase(){}

DataBase::~DataBase(){}

bool DataBase::Load(TiXmlElement* Entry, int32_t Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "Name", "Label", Name);
        DATA->addLabel(Name, Index);
        return true;
    }
    return false;
}


// COLOR

ColorData::ColorData(){}

ColorData::~ColorData(){}

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

DataBase* ColorDataLibrary::LoadElement(TiXmlElement* Element)
{
    ColorData* NewData = new ColorData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

// TEXTURE

TextureData::TextureData()
{
    LoneTexture = true;

    x = y = w = h = 0;
}

TextureData::~TextureData(){}

bool TextureData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", temp);
        sPath = temp;

        XML->QueryUIntValue(Entry, "GridLocation", "X", x);
        XML->QueryUIntValue(Entry, "GridLocation", "Y", y);

        XML->QueryUIntValue(Entry, "SheetLocation", "X", x);
        XML->QueryUIntValue(Entry, "SheetLocation", "Y", y);

        XML->QueryUIntValue(Entry, "SheetLocation", "W", w);
        XML->QueryUIntValue(Entry, "SheetLocation", "H", h);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

DataBase* TextureDataLibrary::LoadElement(TiXmlElement* Element)
{
    TextureData* NewData = new TextureData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

// TEXTURE GRID

TextureGridData::TextureGridData(){}

TextureGridData::~TextureGridData(){}

bool TextureGridData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", temp);
        sPath = temp;

        XML->QueryUIntValue(Entry, "TextureSize", "Height", TextureHeight);
        XML->QueryUIntValue(Entry, "TextureSize", "Width", TextureWidth);

        XML->QueryUIntValue(Entry, "GridSize", "Height", GridHeight);
        XML->QueryUIntValue(Entry, "GridSize", "Width", GridWidth);


        TiXmlElement* Iterator = Entry->FirstChildElement("Textures");
        Iterator = Iterator->FirstChildElement("Texture");
        for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement())
        {
            TextureData* NewTexture = static_cast<TextureData*> (DATA->getTextureDataLibrary()->LoadElement(Iterator));

            NewTexture->setLoneTexture(false);

            // Convert Grid position to pixels
            NewTexture->setX(NewTexture->getX() * TextureWidth);
            NewTexture->setY(NewTexture->getY() * TextureHeight);

            TextureList.push_back(NewTexture);
        }

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

DataBase* TextureGridDataLibrary::LoadElement(TiXmlElement* Element)
{
    TextureGridData* NewData = new TextureGridData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

// TEXTURE SHEET

TextureSheetData::TextureSheetData(){}

TextureSheetData::~TextureSheetData(){}

bool TextureSheetData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        string temp;
        XML->QueryTextValue(Entry, "File", "Path", temp);
        sPath = temp;

        TiXmlElement* Iterator = Entry->FirstChildElement("Textures");
        Iterator = Iterator->FirstChildElement("Texture");
        for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement())
        {
            TextureData* NewTexture = static_cast<TextureData*> (DATA->getTextureDataLibrary()->LoadElement(Iterator));

            NewTexture->setLoneTexture(false);
            TextureList.push_back(NewTexture);
        }

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

DataBase* TextureSheetDataLibrary::LoadElement(TiXmlElement* Element)
{
    TextureSheetData* NewData = new TextureSheetData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

// MODEL

ModelData::ModelData()
{
    Scalar = 1.0;
}

ModelData::~ModelData(){}

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

DataBase* ModelDataLibrary::LoadElement(TiXmlElement* Element)
{
    ModelData* NewData = new ModelData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

// SURFACE TYPE

SurfaceTypeData::SurfaceTypeData(){}

SurfaceTypeData::~SurfaceTypeData(){}

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

DataBase* SurfaceTypeDataLibrary::LoadElement(TiXmlElement* Element)
{
    SurfaceTypeData* NewData = new SurfaceTypeData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void SurfaceTypeDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}

// TILEGROUP

TileGroupData::TileGroupData(){}

TileGroupData::~TileGroupData(){}

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

DataBase* TileGroupDataLibrary::LoadElement(TiXmlElement* Element)
{
    TileGroupData* NewData = new TileGroupData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void TileGroupDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}

// MATERIAL CLASS

MaterialClassData::MaterialClassData()
{
    DefaultMaterialLabel = "NONE";
}

MaterialClassData::~MaterialClassData(){}

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

DataBase* MaterialClassDataLibrary::LoadElement(TiXmlElement* Element)
{
    MaterialClassData* NewData = new MaterialClassData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void MaterialClassDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}

// MATERIAL

MaterialData::MaterialData()
{
    PrimaryColorLabel = "NONE";
    SecondaryColorLabel = "NONE";
    BorderColorLabel = "NONE";
}

MaterialData::~MaterialData(){}

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

DataBase* MaterialDataLibrary::LoadElement(TiXmlElement* Element)
{
    MaterialData* NewData = new MaterialData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void MaterialDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}

// FONT

FontData::FontData(){}

FontData::~FontData(){}

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

DataBase* FontDataLibrary::LoadElement(TiXmlElement* Element)
{
    FontData* NewData = new FontData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

// TILE SHAPE

TileShapeData::TileShapeData()
{
    Open = false;

    ModelID = -1;
    ModelLabel = "NONE";
}

TileShapeData::~TileShapeData(){}

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

DataBase* TileShapeDataLibrary::LoadElement(TiXmlElement* Element)
{
    TileShapeData* NewData = new TileShapeData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void TileShapeDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}

// TREE

TreeData::TreeData(){}

TreeData::~TreeData(){}

bool TreeData::Load(TiXmlElement* Entry, Uint32 Index)
{
    if(Entry)
    {
        XML->QueryTextValue(Entry, "MatGloss", "value", Matgloss);
        XML->QueryTextValue(Entry, "Model", "label", ModelLabel);

        XML->QueryTextValue(Entry, "Trunk", "material", TrunkMaterialLabel);
        XML->QueryTextValue(Entry, "Leaves", "material", LeavesMaterialLabel);

        DataBase::Load(Entry, Index);
        return true;
    }
    return false;
}

bool TreeData::PostProcessing()
{
    ModelID = DATA->getLabelIndex(ModelLabel);

    TrunkMaterialID = DATA->getLabelIndex(TrunkMaterialLabel);
    LeavesMaterialID = DATA->getLabelIndex(LeavesMaterialLabel);

    return true;
}

DataBase* TreeDataLibrary::LoadElement(TiXmlElement* Element)
{
    TreeData* NewData = new TreeData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void TreeDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}

// BUILDING

BuildingData::BuildingData()
{
    ModelLabel = "NONE";
    TextureLabel = "NONE";

    ModelID = -1;
    TextureID = -1;
}

BuildingData::~BuildingData(){}

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

DataBase* BuildingDataLibrary::LoadElement(TiXmlElement* Element)
{
    BuildingData* NewData = new BuildingData();
    if (NewData->Load(Element, DataEntries.size()))
    {
        DataEntries.push_back(NewData);
        return NewData;
    }
    else  // Error durring loading delete the Data object
    {
        printf("Failed to Load Data object");
        delete NewData;
        return NULL;
    }
}

void BuildingDataLibrary::PostProcessDataClass()
{
    for(int i = 0; i < DataEntries.size(); i++)
    {
        DataEntries[i]->PostProcessing();
    }
}
