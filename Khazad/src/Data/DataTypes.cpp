#include <DataTypes.h>
#include <XMLManager.h>


DataBase::DataBase()
{

}

DataBase::~DataBase()
{

}

bool DataBase::Load()
{

}

MaterialData::MaterialData()
{

}

MaterialData::~MaterialData()
{

}

bool MaterialData::Load(TiXmlElement* Entry)
{
    if(Entry)
    {
        XML->QueryUIntValue(Entry, "Texture", "Int", TextureID);
        XML->QueryUIntValue(Entry, "Hardness", "Int", Hardness);

        return true;
    }
    return false;
}
