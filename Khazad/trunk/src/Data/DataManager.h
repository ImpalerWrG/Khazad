#ifndef DATA__HEADER
#define DATA__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <DataTypes.h>
#include <XMLManager.h>


struct ltstr
{
    bool operator()(string s1, string s2) const
    {
        return strcmp(s1.c_str(), s2.c_str()) < 0;
    }
};

class DataManager
{
	DECLARE_SINGLETON_CLASS(DataManager)

public:

    ~DataManager();
    bool Init();

    template <class T>
    bool LoadDataClass(std::vector<T*>* DataVector, const char* Path, const char* Entry)
    {
        cout << "Loading " << Path << endl;

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
                printf("Failed to Load Data object");
                NewData->~T();
            }
        }

        Document->~TiXmlDocument();  // Free the Document
        return true;
    }

	template <class T>
	bool PostProcessDataClass(std::vector<T*>* DataVector)
    {
        for(uint32_t i = 0; i < DataVector->size(); ++i)
        {
            DataVector->at(i)->PostProcessing();
        }
        return true;
    }

    ColorData* getColorData(Uint32 Index)
    {
        if (Index != -1)
            return Colors[Index];
        return Colors[0];
    }
    Uint32 getNumColors()                                    { return Colors.size(); }

    TextureData* getTextureData(Uint32 Index)                { return Textures[Index]; }
    Uint32 getNumTextures()                                  { return Textures.size(); }

    ModelData* getModelData(Uint32 Index)                    { return Models[Index]; }
    Uint32 getNumModels()                                    { return Models.size(); }

    FontData* getFontData(Uint32 Index)                      { return Fonts[Index]; }
    Uint32 getNumFonts()                                     { return Fonts.size(); }

    MaterialData* getMaterialData(Uint32 Index)              { return Materials[Index]; }
    Uint32 getNumMaterials()                                 { return Materials.size(); }

    TileGroupData* getTileGroupData(Uint32 Index)            { return TileGroups[Index]; }
    Uint32 getNumTileGroups()                                { return TileGroups.size(); }

    MaterialClassData* getMaterialClassData(Uint32 Index)    { return MaterialClasses[Index]; }
    Uint32 getNumMaterialClasses()                           { return MaterialClasses.size(); }

    SurfaceTypeData* getSurfaceTypeData(Uint32 Index)        { return SurfaceTypes[Index]; }
    Uint32 getNumSurfaceTypes()                              { return SurfaceTypes.size(); }

    TileShapeData* getTileShapeData(Uint32 Index)            { return TileShapes[Index]; }
    Uint32 getNumTileShape()                                 { return TileShapes.size(); }

    TreeData* getTreeData(Uint32 Index)                      { return Trees[Index]; }
    Uint32 getNumTrees()                                     { return Trees.size(); }

    BuildingData* getBuildingData(Uint32 Index)              { return Buildings[Index]; }
    Uint32 getNumBuildings()                                 { return Buildings.size(); }


    Sint32 getLabelIndex(string Label);
    void addLabel(string Label, Uint32 Index);

protected:

    std::map<string, Uint32, ltstr> GlobalLabelMap;

    // ADD New Data classes Here

    std::vector<ColorData*> Colors;
    std::vector<TextureData*> Textures;
    std::vector<ModelData*> Models;
    std::vector<FontData*> Fonts;
    std::vector<MaterialData*> Materials;
    std::vector<TileGroupData*> TileGroups;
    std::vector<MaterialClassData*> MaterialClasses;
    std::vector<SurfaceTypeData*> SurfaceTypes;
    std::vector<TileShapeData*> TileShapes;
    std::vector<TreeData*> Trees;
    std::vector<BuildingData*> Buildings;
};


#define DATA (DataManager::GetInstance())

#endif // DATA__HEADER
