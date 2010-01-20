#ifndef EXTRACTOR__HEADER
#define EXTRACTOR__HEADER

#include <Singleton.h>
#include <stdafx.h>

///FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFHackAPI.h"
#include <string.h> // for memset

#include <Coordinates.h>

namespace DFHack
{
    class API;
}

struct DFHack::t_matglossPair;
class Column;
class Cell;
class Actor;
class Face;
class TreeManager;
union t_occupancy;
struct DFHack::t_construction;
struct DFHack::t_tree_desc;
struct DFHack::t_building;
struct Face;
class DFHackAPI;


class Extractor
{
DECLARE_SINGLETON_CLASS(Extractor)

public:

	~Extractor();
	bool Init();

    bool Attach();
    void Detach();

    DFHack::API* DFHack;

    bool isAttached()   { return Attached; }

    void InitilizeTilePicker(DFHack::API & DF);
    Sint16 PickMaterial(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial, DFHack::t_matglossPair constructionmaterial, DFHack::t_occupancy occupancy);
    Sint16 ResolveMatGlossPair(DFHack::t_matglossPair MatPair);

    void LoadCellData(DFHack::API & context,
                      vector< vector <uint16_t> >& layerassign,
                      Cell* TargetCell,
                      map<uint64_t, DFHack::t_construction> & constuctions,
                      map<uint64_t, DFHack::t_tree_desc> & vegetation,
                      map<uint64_t, DFHack::t_building> & buildings,
                      CellCoordinates NewCellCoordinates);

protected:

    Sint16 TileShapePicker[600];
    Sint16 TileSurfacePicker[600];
    Sint16 TileMaterialPicker[600];
    Sint16 TileMaterialClassPicker[600];

    vector <int16_t> StoneMatGloss;
    vector <int16_t> MetalMatGloss;
    vector <int16_t> WoodMatGloss;

    bool Initialized;
    bool Attached;
};

#define EXTRACTOR (Extractor::GetInstance())

#endif // EXTRACTOR__HEADER
