#include <stdafx.h>

#include <Extractor.h>
#include <Map.h>
#include <Game.h>
#include <Cell.h>
#include <Building.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>

///FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFHackAPI.h"


#include <string.h> // for memset

using namespace DFHack;

DECLARE_SINGLETON(Extractor)

Extractor::Extractor()
{
    DFHack = NULL;
    Initialized = false;
}

Extractor::~Extractor()
{

}

bool Extractor::Init()
{
    Path path_to_xml("Assets/XML/Memory.xml");
    DFHack = new DFHack::API(path_to_xml);

    if (DFHack != NULL)
    {
        Initialized = true;
        return true;
    }
    return false;
}

bool Extractor::Attach()
{
    if(!DFHack->Attach())
    {
        Attached = false;
        return false;
    }
    if(!DFHack->InitMap())
    {
        Attached = false;
        return false;
    }

    InitilizeTilePicker(*DFHack);

    Attached = true;
    return Attached;
}

void Extractor::Detach()
{
    DFHack->ForceResume(); // just to be sure we really do resume on windows
    DFHack->Detach();
    Attached = false;
}

void Extractor::LoadCellData(DFHack::API & DF,
                       vector< vector <uint16_t> >& layerassign,
                       Cell* TargetCell,
                       map<uint64_t, DFHack::t_construction> & constructions,
                       map<uint64_t, DFHack::t_tree_desc> & vegetation,
                       map<uint64_t, DFHack::t_building> & buildings, // FIXME: this is wrong for buildings. they can overlap
                       CellCoordinates NewCellCoordinates)
{
    uint16_t tiletypes[16][16];
    DFHack::t_designation designations[16][16];
    DFHack::t_occupancy occupancies[16][16];
    uint8_t regionoffsets[16];
    int16_t basemat [16][16];
    int16_t veinmat [16][16];
    DFHack::t_matglossPair constmat [16][16];
    vector <t_vein> veins;

    DF.ReadTileTypes(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, (uint16_t *) tiletypes);
    DF.ReadDesignations(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, (uint32_t *) designations);
    DF.ReadOccupancy(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, (uint32_t *) occupancies);
    DF.ReadRegionOffsets(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, regionoffsets);

    memset(basemat, -1, sizeof(basemat));
    memset(veinmat, -1, sizeof(veinmat));
    memset(constmat, -1, sizeof(constmat));
    veins.clear();
    DF.ReadVeins(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, veins);

    // get the materials, buildings, trees
    for(uint32_t xx = 0; xx < 16; xx++)
    {
        for (uint32_t yy = 0; yy < 16; yy++)
        {
            // base rock layers
            basemat[xx][yy] = layerassign[regionoffsets[designations[xx][yy].bits.biome]][designations[xx][yy].bits.geolayer_index];

            // veins
            for(int i = 0; i < veins.size();i++)
            {
                // and the bit array with a one-bit mask, check if the bit is set
                bool set = ((1 << xx) & veins[i].assignment[yy]) >> xx;
                if(set)
                {
                    // store matgloss
                    veinmat[xx][yy] = veins[i].type;
                }
            }

            // constructions
            uint64_t coord =  (uint64_t)NewCellCoordinates.Z + ((uint64_t)(NewCellCoordinates.Y *16 + yy) << 16) + ((uint64_t)(NewCellCoordinates.X *16 + xx) << 32);
            if(constructions.count(coord))
            {
                // store matgloss
                constmat[xx][yy] = constructions[coord].material;
            }

            // plants
            if(vegetation.count(coord))
            {
                DFHack::t_tree_desc t = vegetation[coord];
                DFHack::TileClass Type = (DFHack::TileClass) DFHack::getVegetationType(tiletypes[xx][yy]);

                if (t.material.type == Mat_Wood)
                {
                    Sint16 TreeTypeID = ResolveMatGlossPair(t.material);

                    bool isAlive = false;

                    switch(Type)
                    {
                        case DFHack::TREE_DEAD:
                        case DFHack::SAPLING_DEAD:
                        case DFHack::SHRUB_DEAD:
                            isAlive = false;
                            break;

                        case DFHack::TREE_OK:
                        case DFHack::SAPLING_OK:
                        case DFHack::SHRUB_OK:
                            isAlive = true;
                            break;
                    }

                    GAME->SpawnTree(MapCoordinates(t.x, t.y, t.z), TreeTypeID, isAlive);
                }
            }

            // buildings, FIXME: doesn't work with overlapping buildings
            if(buildings.count(coord))
            {
                DFHack::t_building b = buildings[coord];

                for(Uint32 i = 0; i < DATA->getNumBuildings(); i++)
                {
                    if (DATA->getBuildingData(i)->getMatgloss() == b.type)
                    {
                        Sint16 MaterialID = ResolveMatGlossPair(b.material);
                        Building* NewBuilding = new Building(MapCoordinates(b.x1, b.y1, b.z), b.x2 - b.x1, b.y2 - b.y1, MaterialID, i);
                        TargetCell->addBuilding(NewBuilding);
                    }
                }
            }
        }
    }

    CubeCoordinates Coordinates;

    for (Coordinates.X = 0; Coordinates.X < CELLEDGESIZE; Coordinates.X += 1)
    {
        for (Coordinates.Y = 0; Coordinates.Y < CELLEDGESIZE; Coordinates.Y += 1)
        {
            DFHack::t_designation Designations = designations[Coordinates.X][Coordinates.Y];
            Uint16 TileType = tiletypes[Coordinates.X][Coordinates.Y];
            DFHack::t_occupancy Ocupancies = occupancies[Coordinates.X][Coordinates.Y];

            Sint16 TileShapeID = TileShapePicker[TileType];
            Sint16 TileSurfaceID = TileSurfacePicker[TileType];
            Sint16 TileMaterialID = PickMaterial(TileType, basemat[Coordinates.X][Coordinates.Y], veinmat[Coordinates.X][Coordinates.Y], constmat[Coordinates.X][Coordinates.Y], Ocupancies);

            TargetCell->setCubeShape(Coordinates, TileShapeID);
            TargetCell->setCubeSurface(Coordinates, TileSurfaceID);
            TargetCell->setCubeMaterial(Coordinates, TileMaterialID);

            TargetCell->setCubeHidden(Coordinates, Designations.bits.hidden);
            TargetCell->setCubeSubTerranean(Coordinates, Designations.bits.subterranean);
            TargetCell->setCubeSkyView(Coordinates, Designations.bits.skyview);
            TargetCell->setCubeSunLit(Coordinates, Designations.bits.light);

            if(Designations.bits.flow_size)
            {
                TargetCell->setLiquid(Coordinates, Designations.bits.liquid_type, Designations.bits.flow_size);
            }
            //TargetCube->setVisible(true);
        }
    }
}

void Extractor::InitilizeTilePicker(DFHack::API & DF)
{
    for(int i = 0; i < 600; ++i)  // Exact number of possible DF tile types isn't know
    {
        TileShapePicker[i] = -1;
        TileSurfacePicker[i] = -1;
        TileMaterialPicker[i] = -1;
        TileMaterialClassPicker[i] = -1;
    }

    // Index the Shape, Surface and Material values of each tile
    for(Uint32 i = 0; i < DATA->getNumTileGroups(); ++i)
    {
        for(Uint32 j = 0; j < DATA->getTileGroupData(i)->TileValues.size(); ++j)
        {
            int Tile = DATA->getTileGroupData(i)->TileValues[j];

            TileShapePicker[Tile] = DATA->getTileGroupData(i)->TileShapeID[j];
            TileSurfacePicker[Tile] = DATA->getTileGroupData(i)->SurfaceTypeID[j];

            TileMaterialPicker[Tile] = DATA->getTileGroupData(i)->getMaterialID();
            TileMaterialClassPicker[Tile] = DATA->getTileGroupData(i)->getMaterialClassID();
        }
    }

    // FIXME: move to .h, so that it can be saved/loaded
    vector<t_matgloss> stonetypes;
    vector<t_matgloss> metaltypes;
    vector<t_matgloss> woodtypes;
    vector<t_matgloss> planttypes;

    DF.ReadStoneMatgloss(stonetypes);
    DF.ReadPlantMatgloss(metaltypes);  // HACK, DF ReadPlantMatgloss and ReadMetalMatgloss are reversed
    DF.ReadWoodMatgloss(woodtypes);
    DF.ReadMetalMatgloss(planttypes);

    Sint16 uninitialized = -1;

    Uint32 NumStoneMats = stonetypes.size();
    for(Uint32 i = 0; i < NumStoneMats; i++)
    {
        bool Matchfound = false;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialClassData(DATA->getMaterialData(j)->getMaterialClass())->getMatGlossIndex() == Mat_Stone)
            {
                if(DATA->getMaterialData(j)->getMatGloss() == stonetypes[i].id)
                {
                    StoneMatGloss.push_back(j);
                    Matchfound = true;
                    break;
                }
            }
        }
        if(!Matchfound)
        {
            StoneMatGloss.push_back(uninitialized);
        }
    }

    Uint32 NumMetalMats = metaltypes.size();
    for(Uint32 i = 0; i < NumMetalMats; i++)
    {
        bool Matchfound = false;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialClassData(DATA->getMaterialData(j)->getMaterialClass())->getMatGlossIndex() == Mat_Metal)
            {
                if(DATA->getMaterialData(j)->getMatGloss() == metaltypes[i].id)
                {
                    MetalMatGloss.push_back(j);
                    Matchfound = true;
                    break;
                }
            }
        }
        if(!Matchfound)
        {
            MetalMatGloss.push_back(uninitialized);
        }
    }


    Uint32 NumTreeMats = woodtypes.size();
    for(Uint32 i = 0; i < NumTreeMats; i++)
    {
        /*
        bool Matchfound = false;
        int tree_end = DATA->getNumTrees();
        for(Uint32 j = 0; j < tree_end; ++j)
        {
            TreeData * td = DATA->getTreeData(j);
            if(td)
            if(td->getMatgloss() == woodtypes[i].id)
            {
                WoodMatGloss.push_back(j);
                Matchfound = true;
                break;
            }
        }
        if(!Matchfound)
        {*/
            WoodMatGloss.push_back(uninitialized);
        //}
    }
}

//FIXME: the ugly hack
Sint16 Extractor::PickMaterial(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial, DFHack::t_matglossPair constructionmaterial, DFHack::t_occupancy occupancy)
{
    static Uint16 Unknown = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    static Uint16 LayerStone = DATA->getLabelIndex("MATERIALCLASS_LAYER_STONE");
    static Uint16 Soil = DATA->getLabelIndex("MATERIALCLASS_SOIL");
    static Uint16 VeinStone = DATA->getLabelIndex("MATERIALCLASS_VEIN_STONE");

    //return Unknown;

    Sint16 TileMaterial = TileMaterialPicker[TileType];
    if (TileMaterial != -1 && TileMaterial < DATA->getNumMaterials())
    {
        return TileMaterial;
    }

    Sint16 TileMaterialClass = TileMaterialClassPicker[TileType];
    Sint16 DefaultMaterial = -1;
    if(TileMaterialClass != -1)
    {
        DefaultMaterial = DATA->getMaterialClassData(TileMaterialClass)->getDefaultMaterial();
    }

    if ( TileMaterialClass == LayerStone || TileMaterialClass == Soil )
    {
        return StoneMatGloss[basematerial];
    }
    else if (TileMaterialClass == VeinStone)
    {
        if (veinmaterial >= 0 && veinmaterial < StoneMatGloss.size() && StoneMatGloss[veinmaterial] != -1)
        {
            return StoneMatGloss[veinmaterial];
        }
        else // Probably a Modded material
        {
            cerr << "bad or unknown vein matgloss:" << veinmaterial << endl;
            if(DefaultMaterial != -1 && DefaultMaterial < DATA->getNumMaterials())
            {
                return DefaultMaterial;
            }
        }
    }

    Sint16 ConstructionMaterial = ResolveMatGlossPair(constructionmaterial);
    if(ConstructionMaterial != -1)
    {
        return ConstructionMaterial;
    }

    if (DefaultMaterial != -1 && DefaultMaterial < DATA->getNumMaterials())
    {
        return DefaultMaterial;
    }

    return Unknown;
}

Sint16 Extractor::ResolveMatGlossPair(t_matglossPair MatPair)
{
    static Uint16 Unknown = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");

    if (MatPair.type != -1 && MatPair.index != -1)
    {
        Sint16 PotentialMaterial = -1;

        if (MatPair.type == Mat_Stone)
        {
            if (MatPair.index >= 0 && MatPair.index < StoneMatGloss.size())
            {
                PotentialMaterial = StoneMatGloss[MatPair.index];
            }
        }
        else if (MatPair.type == Mat_Metal)
        {
            if (MatPair.index >= 0 && MatPair.index < MetalMatGloss.size())
            {
                PotentialMaterial = MetalMatGloss[MatPair.index];
            }
        }
        else if (MatPair.type == Mat_Wood)
        {
            if (MatPair.index >= 0 && MatPair.index < WoodMatGloss.size())
            {
                PotentialMaterial = WoodMatGloss[MatPair.index];
            }
        }

        if(PotentialMaterial != -1)
        {
            return PotentialMaterial;
        }
        else
        {
            cerr << "material not in Materials.xml: "<< MatPair.type << "::" << MatPair.index << endl;
        }

        // Try to find a MateralClass default material
        for (Sint16 MaterialClass = 0; MaterialClass < DATA->getNumMaterialClasses(); MaterialClass++)
        {
            if (MatPair.type == DATA->getMaterialClassData(MaterialClass)->getMatGlossIndex())
            {
                PotentialMaterial = DATA->getMaterialClassData(MaterialClass)->getDefaultMaterial();
                break;
            }
        }

        if(PotentialMaterial != -1)
        {
            return PotentialMaterial;
        }
        else
        {
            cerr << "material not in Material Classes: "<< MatPair.type << "::" << MatPair.index << endl;
        }
    }

    return Unknown;
}

