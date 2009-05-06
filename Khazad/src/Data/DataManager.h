#ifndef DATA__HEADER
#define DATA__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <DataTypes.h>
#include <XMLManager.h>

struct ltstr
{
    bool operator()(const char* s1, const char* s2) const
    {
        return strcmp(s1, s2) < 0;
    }
};

class DataManager
{
	DECLARE_SINGLETON_CLASS(DataManager)

public:

	~DataManager();
	bool Init();

	template <class T>
	bool LoadDataClass(std::vector<T*>* DataVector, char* Path, char* Entry)
    {
        TiXmlDocument* Document = XML->loadFile(Path);
        TiXmlElement* Parent = Document->RootElement();
        TiXmlElement* Iterator = Parent->FirstChildElement(Entry);

        for(; Iterator != NULL; Iterator = Iterator->NextSiblingElement(Entry))
        {
            //TODO allow the type of Element to fork to different loading types so mixed files are possible
            T* NewData = new T;
            if(NewData->Load(Iterator, DataVector->size()))
            {
                DataVector->push_back(NewData);
            }
            else  // Error durring loading delete the Data object
            {
                NewData->~T();
            }
        }

        Document->~TiXmlDocument();  // Free the Document
        return true;
    }

	template <class T>
	bool PostProcessDataClass(std::vector<T*>* DataVector)
    {
        for(int i = 0; i < DataVector->size(); ++i)
        {
            DataVector->at(i)->PostProcessing();
        }
        return true;
    }

    TextureData* getTextureData(Uint32 Index)          { return Textures[Index]; }
    Uint32 getNumTextures()                         { return Textures.size(); }

    MaterialData* getMaterialData(Uint32 Index)        { return Materials[Index]; }
    Uint32 getNumMaterials()                            { return Materials.size(); }

    Uint32 getLabelIndex(char* Label);
    void addLabel(char* Label, Uint32 Index);

protected:

    std::map<char*, Uint32, ltstr> GlobalLabelMap;

    // ADD New Data classes Here

    std::vector<TextureData*> Textures;
    std::vector<MaterialData*> Materials;

};


#define DATA (DataManager::GetInstance())

#endif // DATA__HEADER
