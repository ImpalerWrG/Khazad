/* Memory research
    http://dwarffortresswiki.net/index.php/User:Rick/memory.ini#A_table_of_available_settings
    http://dwarffortresswiki.net/index.php/User:Rick/Memory_research#Tile_Block
    http://dwarffortresswiki.net/index.php/User:AzureLightning/Memory_research
    http://dwarffortresswiki.net/index.php/User:Iluxan/Memory_research
 */

#ifndef EXTRACT_HEADER
#define EXTRACT_HEADER

#include <Singleton.h>
#include <stdafx.h>
#include <Process.h>
#include <DfMap.h>

class Extractor
{
DECLARE_SINGLETON_CLASS(Extractor)

protected:
    DfMap *df_map;   // DF extracted map structure
public:
    bool Init();
    ~Extractor();
    bool loadMap(string FileName);
    bool writeMap(string FileName);
    bool isMapLoaded();
    DfMap *getMap() {return df_map;};

    bool dumpMemory();
};


#define EXTRACT (Extractor::GetInstance())

#endif // EXTRACT_HEADER
