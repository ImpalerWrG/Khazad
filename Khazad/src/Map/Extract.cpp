#include <windows.h>
#include <stdio.h>

#include "Extract.h"
//#include "scene.h"
#include <stdafx.h>


DECLARE_SINGLETON(Extractor)

Extractor::Extractor()
{

}

bool Extractor::Init()
{
    return true;
}

Extractor::~Extractor()
{

}

int Extractor::dumpMemory( /*scene *thescene*/)
{
    HWND DFProcess;
    HANDLE DFHandle;
    DWORD DFProcessID;

    toggle_isloaded = 0;

	char process_name[] = "Dwarf Fortress";
    int buffer, map_loc;

	int temp1, temp2, temp3;
    int temp_loc, temp_locx, temp_locy, temp_locz;
    int temp_tilex, temp_tiley;
    unsigned int current_mem = 0;
    int pe_found;
    short int temp_tile;
    z_active_levels = 0;

    FILE *file_out;
    DWORD bytesRead = 0;



    meminfo.push_back(memory_info());

    sprintf(meminfo[0].version,"v0.28.181.40c");

    meminfo[0].pe_timestamp        = (0x48AD802C);
    meminfo[0].pe_timestamp_offset = (0x004000F8);
    meminfo[0].map_offset          = (0x015C3D60);
    meminfo[0].x_count_offset      = (0x015C3D78);
    meminfo[0].y_count_offset      = (0x015C3D7C);
    meminfo[0].z_count_offset      = (0x015C3D80);
    meminfo[0].tile_type_offset    = (0x00000062);
    meminfo[0].designation_offset  = (0x00000264);
    meminfo[0].occupancy_offset    = (0x00000664);

    int pe_offset = meminfo[0].pe_timestamp_offset;
    int pe_timestamp = meminfo[0].pe_timestamp;
    int map_offset = meminfo[0].map_offset;
    int x_count_offset = meminfo[0].x_count_offset;
    int y_count_offset = meminfo[0].y_count_offset;
    int z_count_offset = meminfo[0].z_count_offset;
    int tile_type_offset = meminfo[0].tile_type_offset;


    for ( unsigned int i=1; i < meminfo.size(); i++ )
    {
        printf("ver: %s\n",meminfo[i].version);
    }

	DFProcess = FindWindow(NULL,process_name);
	if(!DFProcess)
	{
	    printf("Cannot find window with name \"%s\"\n", process_name);
		return 3;
    }

	GetWindowThreadProcessId(DFProcess, &DFProcessID);
	printf("Window Thread Process ID [%u] \n", (unsigned int)DFProcessID);
	DFHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, DFProcessID);

	if(!DFHandle)
	{
	    printf("Could not get application handle to hook. \n");
		return 2;
    }

    //pe_timestamp
    pe_found = 0;
    ReadProcessMemory( DFHandle, (int*)(pe_offset), &buffer, sizeof(int), &bytesRead );
    printf("pe_timestamp : 0x%.8X [0x%.8X]\n",buffer,pe_timestamp);

    if ( pe_timestamp != buffer )
    {
        readMemoryFile();
        for (current_mem++; current_mem < meminfo.size() && !pe_found; current_mem++ )
        {
            printf("PE timestamps do not match for %s.  Trying next... ", meminfo[current_mem-1].version);
            ReadProcessMemory( DFHandle, (int*)(meminfo[current_mem].pe_timestamp_offset), &buffer, sizeof(int), &bytesRead );

            if ( meminfo[current_mem].pe_timestamp == buffer )
            {
                pe_found = true;
                printf("found[%d].\nUsing version %s.\n", current_mem, meminfo[current_mem].version);
            }
            else
            {
                printf("no match.\n");
            }
        }

        if ( !pe_found )
        {
            return 1;
        }
        else
        {
            current_mem--;
            pe_offset = meminfo[current_mem].pe_timestamp_offset;
            pe_timestamp = meminfo[current_mem].pe_timestamp;
            map_offset = meminfo[current_mem].map_offset;
            x_count_offset = meminfo[current_mem].x_count_offset;
            y_count_offset = meminfo[current_mem].y_count_offset;
            z_count_offset = meminfo[current_mem].z_count_offset;
            tile_type_offset = meminfo[current_mem].tile_type_offset;
        }
    }

    //map_loc
    ReadProcessMemory( DFHandle, (int*)(map_offset), &map_loc, sizeof(int), &bytesRead );
    printf("map data : 0x%.8X\n",map_loc);

    if ( !map_loc )
    {
        printf("Could not find DF map information in memory...\n");
        return 1;
    }
    else
    {
        // x_blocks count
        ReadProcessMemory( DFHandle, (int*)(x_count_offset), &x_blocks, sizeof(int), &bytesRead );
        printf("x_blocks tiles: %u (%d)\n",16*x_blocks,x_blocks);

        // y_blocks count
        ReadProcessMemory( DFHandle, (int*)(y_count_offset), &y_blocks, sizeof(int), &bytesRead );
        printf("y_blocks tiles: %u (%d)\n",16*y_blocks,y_blocks);

        // z_levels count
        ReadProcessMemory( DFHandle, (int*)(z_count_offset), &z_levels, sizeof(int), &bytesRead );
        printf("z_levels height: %u\n\n",z_levels);
        z_level_active = (short int*) malloc( sizeof(short int) * z_levels );
        memset(z_level_active, 0, sizeof(short int)*z_levels );


        //map_blocks = allocateArray3i(x_blocks, y_blocks, z_levels, 0, 0);
        map_blocks.resize(boost::extents[x_blocks][y_blocks][z_levels]);
        for ( temp1=0; temp1 < x_blocks; temp1++ )
        {
            for ( temp2=0; temp2 < y_blocks; temp2++ )
            {
                for ( temp3=0; temp3 < z_levels; temp3++ )
                {
                    map_blocks[temp1][temp2][temp3] = 0;
                }
            }
        }

        cube_sides.resize(boost::extents[x_blocks*16+4][y_blocks*16+4][z_levels+4]);
        cube_textures.resize(boost::extents[x_blocks*16+4][y_blocks*16+4][z_levels+4]);
        tile_types.resize(boost::extents[x_blocks*16+4][y_blocks*16+4][z_levels+4]);

        for ( temp1=0; temp1 < x_blocks*16+4; temp1++ )
        {
            for ( temp2=0; temp2 < y_blocks*16+4; temp2++ )
            {
                for ( temp3=0; temp3 < z_levels+4; temp3++ )
                {
                    tile_types[temp1][temp2][temp3] = 32;
                    cube_sides[temp1][temp2][temp3] = 0;
                    cube_textures[temp1][temp2][temp3] = 0;
                }
            }
        }

        //starts at bottom of map
        //in upper left corner

        //initialize all the pointers to the map blocks
        for ( temp1=0; temp1 < x_blocks; temp1++ )
        {
            temp_locx = map_loc + ( 4 * temp1 );
            ReadProcessMemory( DFHandle, (int*)(temp_locx), &temp_locy, sizeof(int), &bytesRead );
            for ( temp2=0; temp2 < y_blocks; temp2++ )
            {
                ReadProcessMemory( DFHandle, (int*)(temp_locy), &temp_locz, sizeof(int), &bytesRead );
                for ( temp3=0; temp3 < z_levels; temp3++ )
                {
                    ReadProcessMemory( DFHandle, (int*)(temp_locz), &temp_loc, sizeof(int), &bytesRead );
                    map_blocks[temp1][temp2][temp3] = temp_loc;
                    temp_locz += 4;
                }
                temp_locy += 4;
            }
            //thescene->ce_progress_bar->setProgress(((float)temp1+1.0)/((float)x_blocks*4.0));
            //thescene->render();
        }

        //read the memory from the map blocks
        for ( temp1=0; temp1 < x_blocks; temp1++ )
        {
            for ( temp2=0; temp2 < y_blocks; temp2++ )
            {
                for ( temp3=0; temp3 < z_levels; temp3++ )
                {
                    for ( temp_tiley=0; temp_tiley < 16; temp_tiley++ )
                    {
                        for ( temp_tilex=0; temp_tilex < 16; temp_tilex++ )
                        {
                            if ( map_blocks[temp1][temp2][temp3] )
                            {
                                z_level_active[temp3] = 1; //so we know levels to export

                                ReadProcessMemory( DFHandle, (int*)(map_blocks[temp1][temp2][temp3]+tile_type_offset+(2*temp_tiley+(temp_tilex*16*2))), &temp_tile, sizeof(short int), &bytesRead );

                                tile_types[temp1*16+temp_tilex+2][temp2*16+temp_tiley+2][temp3+2] = temp_tile;

                                //points.push_back(new point(temp1*16+temp_tilex,temp2*16+temp_tiley,temp3));
                            }
                            else
                            {
                                tile_types[temp1*16+temp_tilex+2][temp2*16+temp_tiley+2][temp3+2]=-1;
                            }
                        }
                    }
                }
            }
            //thescene->ce_progress_bar->setProgress(((float)temp1+1.0)/((float)x_blocks*1.33)+.5);
            //thescene->render();
        }

        //thescene->ce_progress_bar->setProgress(1.0);
        //thescene->render();
        //glfwSleep(.5);

        printf("Stuff on levels: ");
        for ( temp1=0; temp1 < z_levels; temp1++ )
        {
            if ( z_level_active[temp1] )
            {
                printf("%d ",temp1);
                z_active_levels++;
            }

        }
        printf("\n");


        file_out = fopen("3dwarf.map","wb");
        if  (file_out == NULL) { printf("Can't open \"3dwarf.map\" for write.\n"); }
        else {
            //print the debug file
            fwrite(&x_blocks, sizeof(x_blocks),1,file_out);
            fwrite(&y_blocks, sizeof(y_blocks),1,file_out);
            fwrite(&z_active_levels, sizeof(z_active_levels),1,file_out);

            for ( temp1=0; temp1 < z_levels; temp1++ )
            {
                if ( z_level_active[temp1] )
                {
                    for ( temp2=0; temp2 < y_blocks*16; temp2++ )
                    {
                        for ( temp3=0; temp3 < x_blocks*16; temp3++ )
                        {
                            fwrite(&tile_types[temp3+2][temp2+2][temp1+2],sizeof(short int), 1, file_out);
                        }
                    }
                }
            }
        }
        fclose(file_out);

        createTerrain();

        toggle_isloaded = 1;
    }  //end of major map_loc check


    return 0;
}

int Extractor::loadMap(char* infile/*, scene* myself*/)
{
    FILE *file_in;
    int temp1, temp2, temp3;
    double elapsed_time;

    file_in = fopen("3dwarf.map", "rb");
    if  (file_in == NULL)
    {
        printf("Can't open \"3dwarf.map\" for read.\n");
        return 1;
    }
    else
    {
        if  ( toggle_isloaded )
        {
            printf("freeing tile memory... ");
            tile_types.resize(boost::extents[0][0][0]);
            printf("done.\n");
        }

        toggle_isloaded = 0;

        fread(&x_blocks, sizeof(int), 1, file_in);
        fread(&y_blocks, sizeof(int), 1, file_in);
        fread(&z_levels, sizeof(int), 1, file_in);

        printf("Read from file\nx_size: %d\ny_size: %d\nz_levels: %d\n", x_blocks, y_blocks, z_levels);

        tile_types.resize(boost::extents[x_blocks*16+4][y_blocks*16+4][z_levels+4]);
        cube_sides.resize(boost::extents[x_blocks*16+4][y_blocks*16+4][z_levels+4]);
        cube_textures.resize(boost::extents[x_blocks*16+4][y_blocks*16+4][z_levels+4]);

        for ( temp1=0; temp1 < x_blocks*16+4; temp1++ )
        {
            for ( temp2=0; temp2 < y_blocks*16+4; temp2++ )
            {
                for ( temp3=0; temp3 < z_levels+4; temp3++ )
                {
                    tile_types[temp1][temp2][temp3] = 32;
                    cube_sides[temp1][temp2][temp3] = 0;
                    cube_textures[temp1][temp2][temp3] = 0;
                }
            }
        }

        for ( temp1=0; temp1 < z_levels; temp1++ )
        {
            for ( temp2=0; temp2 < y_blocks*16; temp2++ )
            {
                for ( temp3=0; temp3 < x_blocks*16; temp3++ )
                {
                    fread(&tile_types[temp3+2][temp2+2][temp1+2],sizeof(short int), 1, file_in);
                }
            }
        }

        fclose(file_in);
        createTerrain();
        toggle_isloaded = 1;
    }

    return 0;
}

int Extractor::createTerrain()
{
    int temp1, temp2, temp3;

    for ( temp1=1; temp1 < z_levels +3; temp1++ )
    {
        for ( temp2=1; temp2 < y_blocks*16 +3; temp2++ )
        {
            for ( temp3=1; temp3 < x_blocks*16 +3; temp3++ )
            {
                //assign flag for floor or wall

                if ( isWallTerrain(tile_types[temp3][temp2][temp1]) )
                {
                    cube_sides[temp3][temp2][temp1] ^= WALL_FLAG;
                }
                if ( isFloorTerrain(tile_types[temp3][temp2][temp1]) )
                {
                    cube_sides[temp3][temp2][temp1] ^= FLOOR_FLAG;
                }

                // Assign Textures

                cube_textures[temp3][temp2][temp1] = picktexture(tile_types[temp3][temp2][temp1]);

                // Assign sides visible

                if ( isOpenTerrain(tile_types[temp3][temp2][temp1]) )
                {
                    // Walls

                    if ( isWallTerrain(tile_types[temp3+1][temp2][temp1]) )
                    {
                         cube_sides[temp3+1][temp2][temp1] ^= CUBE_LEFT;
                    }
                    if ( isWallTerrain(tile_types[temp3-1][temp2][temp1]) )
                    {
                         cube_sides[temp3-1][temp2][temp1] ^= CUBE_RIGHT;
                    }
                    if ( isWallTerrain(tile_types[temp3][temp2+1][temp1]) )
                    {
                         cube_sides[temp3][temp2+1][temp1] ^= CUBE_FORWARD;
                    }
                    if ( isWallTerrain(tile_types[temp3][temp2-1][temp1] ) )
                    {
                         cube_sides[temp3][temp2-1][temp1] ^= CUBE_BACK;
                    }
                    if ( isWallTerrain(tile_types[temp3][temp2][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1+1] ^= CUBE_DOWN;
                    }
                    if ( isWallTerrain(tile_types[temp3][temp2][temp1-1]) )
                    {
                         cube_sides[temp3][temp2][temp1-1] ^= CUBE_UP;
                    }

                    // Floors

                    if ( isFloorTerrain(tile_types[temp3+1][temp2][temp1]) && (isRampTerrain(tile_types[temp3+1][temp2][temp1])))
                    {
                         cube_sides[temp3+1][temp2][temp1] ^= FLOOR_LEFT;
                    }
                    if ( isFloorTerrain(tile_types[temp3-1][temp2][temp1]) && !(isRampTerrain(tile_types[temp3-1][temp2][temp1])) )
                    {
                         cube_sides[temp3-1][temp2][temp1] ^= FLOOR_RIGHT;
                    }
                    if ( isFloorTerrain(tile_types[temp3][temp2+1][temp1]) && !(isRampTerrain(tile_types[temp3][temp2+1][temp1])) )
                    {
                         cube_sides[temp3][temp2+1][temp1] ^= FLOOR_FORWARD;
                    }
                    if ( isFloorTerrain(tile_types[temp3][temp2-1][temp1] ) && !(isRampTerrain(tile_types[temp3][temp2-1][temp1] )) )
                    {
                         cube_sides[temp3][temp2-1][temp1] ^= FLOOR_BACK;
                    }
                    if ( isFloorTerrain(tile_types[temp3][temp2][temp1+1]) && !(isRampTerrain(tile_types[temp3][temp2][temp1+1])) )
                    {
                         cube_sides[temp3][temp2][temp1+1] ^= FLOOR_DOWN;
                    }
                }

                //  RAMPS

                if ( isRampTerrain(tile_types[temp3][temp2][temp1]) )
                {
                    cube_sides[temp3][temp2][temp1] ^= RAMP_FLAG;

                    if ( isFloorTerrain(tile_types[temp3+1][temp2][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= ( RAMP_E | RAMP_NE | RAMP_SE );
                    }
                    if ( isFloorTerrain(tile_types[temp3-1][temp2][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= ( RAMP_W | RAMP_NW | RAMP_SW );
                    }
                    if ( isFloorTerrain(tile_types[temp3][temp2-1][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= ( RAMP_N | RAMP_NW | RAMP_NE );
                    }
                    if ( isFloorTerrain(tile_types[temp3][temp2+1][temp1+1] ) )
                    {
                         cube_sides[temp3][temp2][temp1] |= ( RAMP_S | RAMP_SW | RAMP_SE );
                    }

                    if ( isFloorTerrain(tile_types[temp3+1][temp2-1][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= RAMP_NE;
                    }
                    if ( isFloorTerrain(tile_types[temp3+1][temp2+1][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= RAMP_SE;
                    }
                    if ( isFloorTerrain(tile_types[temp3-1][temp2-1][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= RAMP_NW;
                    }
                    if ( isFloorTerrain(tile_types[temp3-1][temp2+1][temp1+1]) )
                    {
                         cube_sides[temp3][temp2][temp1] |= RAMP_SW;
                    }
                }
            }
        }
    }

    return 0;
}

int Extractor::readMemoryFile()
{
    char tempString[100];
    char tempString2[100];
    int i,j,count;
    memory_info temp_meminfo;
    FILE *infile;


    if ((infile = fopen("Assets//Maps//memory.ini", "r")) == NULL)
    {
        fprintf(stderr, "Cannot open %s for read\n", "memory.ini");
        return -1;
    }

    while (fgets(tempString, 100, infile) != NULL)
    {
        if ( tempString[0] == 'v' )
        { //start a processing loop
            for (i=1; tempString[i] != 'v'; i++);
            i++;
            for (j=0; tempString[i+j] != '\n'; j++)
                tempString2[j] = tempString[j+i];
            tempString2[j] = '\0';

            //printf("%s\n",tempString2);
            sprintf(temp_meminfo.version,tempString2);
            fgets(tempString, 100, infile);

            for ( count=0; count < 9; count++ )
            {
                for (i=1; tempString[i] != 'x'; i++);
                i++;
                for (j=0; tempString[i+j] != '\n'; j++)
                    tempString2[j] = tempString[j+i];
                tempString2[j] = '\0';

                switch (count)
                {
                    case 0: temp_meminfo.pe_timestamp = strtoul(tempString2, NULL, 16); break;
                    case 1: temp_meminfo.pe_timestamp_offset = strtoul(tempString2, NULL, 16); break;
                    case 2: temp_meminfo.map_offset = strtoul(tempString2, NULL, 16); break;
                    case 3: temp_meminfo.x_count_offset = strtoul(tempString2, NULL, 16); break;
                    case 4: temp_meminfo.y_count_offset = strtoul(tempString2, NULL, 16); break;
                    case 5: temp_meminfo.z_count_offset = strtoul(tempString2, NULL, 16); break;
                    case 6: temp_meminfo.tile_type_offset = strtoul(tempString2, NULL, 16); break;
                    case 7: temp_meminfo.designation_offset = strtoul(tempString2, NULL, 16); break;
                    case 8: temp_meminfo.occupancy_offset = strtoul(tempString2, NULL, 16); break;
                }
                fgets(tempString, 100, infile);
            }
            meminfo.push_back(temp_meminfo);
        }
    }

    return 0;
}

int Extractor::picktexture(int in)
{

    //return 9;

    switch ( in ) {

        case 1: //slope down
            return 3;
            break;

        case 2: //murky pool
            return 2;
            break;

        case 19: //driftwood stack
            return 8;
            break;

        case 24: //tree
            //return 3;
            return 15;
            break;

        case 25: //up-down stair frozen liquid
        case 26: //down stair frozen liquid
        case 27: //up stair frozen liquid
            return 25;
            break;

        case 32:  //open space
            return 5;
            break;

        case 34: //shrub
            return 14;
            break;

        case 35: //chasm
            return 31;
            break;

        case 36: //up-down stair lavastone
        case 37: //down stair lavastone
        case 38: //up stair lavastone
            return 32;
            break;

        case 39: //up-down stair soil
        case 40: //down stair soil
        case 41: //up stair soil
            return 10;
            break;

        case 42: //eerie pit
            return 31;
            break;

        case 43: //stone floor detailed
            return 7;
            break;

        case 44: //lavastone floor detailed
            return 32;
            break;

        case 45: //featstone? floor detailed
            return 18;
            break;

        case 46: //minstone? floor detailed [calcite]
            return 9;
            break;

        case 47: //frozen liquid floor detailed
            return 27;
            break;

        case 49: //up-down stair grass1 [muddy?]
        case 50: //down stair grass1 [muddy?]
        case 51: //up stair grass1 [muddy?]
            return 0;
            break;

        case 52: //up-down stair grass2
        case 53: //down stair grass2
        case 54: //up stair grass2
            return 16;
            break;

        case 55: //up-down stair stone
        case 56: //down stair stone
        case 57: //up stair stone
            return 1;
            break;

        case 58: //up-down stair minstone
        case 59: //down stair minstone
        case 60: //up stair minstone
            return 9;
            break;

        case 61: //up-down stair featstone
        case 62: //down stair featstone
        case 63: //up stair featstone
            return 18;
            break;

        case 65: //stone fortification
            return 22;
            break;

        case 67: //campfire
            return 3;
            break;

        case 70: //fire
            return 3;
            break;

        case 79: //stone pillar
            return 1;
            break;

        case 80: //lavastone pillar
            return 32;
            break;

        case 81: //featstone pillar
            return 18;
            break;

        case 82: //minstone pillar
            return 9;
            break;

        case 83: //frozen liquid pillar
            return 27;
            break;

        case 89: //waterfall landing
            return 2;
            break;

        case 90: //river source
            return 2;
            break;

        case 176: //stone wall worn1 (most worn)
        case 177: //stone wall worn2 (sorta worn)
        case 178: //stone wall worn3 (least worn)
        case 219: //stone wall (not worn)
            return 1;
            break;

        case 231: //sapling
            return 15;
            break;

        case 233: //ramp grass dry
            return 33;
            break;

        case 234: //ramp grass dead
            return 33;
            break;

        case 235: //ramp grass1 [muddy?]
            return 0;
            break;

        case 236: //ramp grass2
            return 16;
            break;

        case 237: //ramp stone
            return 1;
            break;

        case 238: //ramp lavastone
            return 32;
            break;

        case 239: //ramp featstone
            return 18;
            break;

        case 240: //ramp minstone
            return 9;
            break;

        case 241: //ramp soil
            return 10;
            break;

        case 242: //ash1
        case 243: //ash2
        case 244: //ash3
            return 32;
            break;

        case 245: //ramp frozen liquid
            return 27;
            break;

        case 258: //frozen liquid 1
        case 259: //frozen liquid 2
        case 260: //frozen liquid 3
            return 25;
            break;

        case 261: //furrowed soil [road?]
            return 21;
            break;

        case 262: //frozen liquid 0
            return 25;
            break;

        case 264: //lava
            return 24;
            break;

        case 265: //soil wall
            return 10;
            break;

        case 269: //lavastone wall rd2
        case 270: //lavastone wall r2d
        case 271: //lavastone wall r2u
        case 272: //lavastone wall ru2
        case 273: //lavastone wall l2u
        case 274: //lavastone wall lu2
        case 275: //lavastone wall l2d
        case 276: //lavastone wall ld2
        case 277: //lavastone wall lrud
        case 278: //lavastone wall rud
        case 279: //lavastone wall lrd
        case 280: //lavastone wall lru
        case 281: //lavastone wall lud
        case 282: //lavastone wall rd
        case 283: //lavastone wall ru
        case 284: //lavastone wall lu
        case 285: //lavastone wall ld
        case 286: //lavastone wall ud
        case 287: //lavastone wall lr
            return 32;
            break;

        case 288: //featstone wall rd2
        case 289: //featstone wall r2d
        case 290: //featstone wall r2u
        case 291: //featstone wall ru2
        case 292: //featstone wall l2u
        case 293: //featstone wall lu2
        case 294: //featstone wall l2d
        case 295: //featstone wall ld2
        case 296: //featstone wall lrud
        case 297: //featstone wall rud
        case 298: //featstone wall lrd
        case 299: //featstone wall lru
        case 300: //featstone wall lud
        case 301: //featstone wall rd
        case 382: //featstone wall ru
        case 303: //featstone wall lu
        case 304: //featstone wall ld
        case 305: //featstone wall ud
        case 306: //featstone wall lr
            return 18;
            break;

        case 307: //stone wall rd2
        case 308: //stone wall r2d
        case 309: //stone wall r2u
        case 310: //stone wall ru2
        case 311: //stone wall l2u
        case 312: //stone wall lu2
        case 313: //stone wall l2d
        case 314: //stone wall ld2
        case 315: //stone wall lrud
        case 316: //stone wall rud
        case 317: //stone wall lrd
        case 318: //stone wall lru
        case 319: //stone wall lud
        case 320: //stone wall rd
        case 321: //stone wall ru
        case 322: //stone wall lu
        case 323: //stone wall ld
        case 324: //stone wall ud
        case 325: //stone wall lr
            return 1;
            break;

        case 326: //lavastone fortification
            return 32;
            break;

        case 327: //featstone fortification
            return 18;
            break;

        case 328: //lavastone wall worn1 (most worn)
        case 329: //lavastone wall worn2 (middle worn)
        case 330: //lavastone wall worn3 (least worn)
        case 331: //lavastone wall
            return 32;
            break;

        case 332: //featstone wall worn1 (most worn)
        case 333: //featstone wall worn2 (middle worn)
        case 334: //featstone wall worn3 (least worn)
        case 335: //featstone wall
            return 18;
            break;

        case 336: //stone floor 1 (raw stone)
        case 337: //stone floor 2 (raw stone)
        case 338: //stone floor 3 (raw stone)
        case 339: //stone floor 4 (raw stone)
            return 17;
            break;

        case 340: //lavastone floor 1 (raw stone)
        case 341: //lavastone floor 2 (raw stone)
        case 342: //lavastone floor 3 (raw stone)
        case 343: //lavastone floor 4 (raw stone)
            return 32;
            break;

        case 344: //featstone floor 1 (raw stone)
        case 345: //featstone floor 2 (raw stone)
        case 346: //featstone floor 3 (raw stone)
        case 347: //featstone floor 4 (raw stone)
            return 18;
            break;

        case 348: //grass floor 1 (raw)
        case 349: //grass floor 2 (raw)
        case 350: //grass floor 3 (raw)
        case 351: //grass floor 4 (raw)
            return 0;
            break;

        case 352: //soil floor 1 (raw)
        case 353: //soil floor 2 (raw)
        case 354: //soil floor 3 (raw)
        case 355: //soil floor 4 (raw)
            return 10;
            break;

        case 356: //soil floor 1 wet (raw) [red sand?]
        case 357: //soil floor 2 wet (raw) [red sand?]
        case 358: //soil floor 3 wet (raw) [red sand?]
        case 359: //soil floor 4 wet (raw) [red sand?]
            return 10;
            break;

        case 360: //frozen liquid fortification
            return 27;
            break;

        case 361: //frozen liquid wall worn1 (most worn)
        case 362: //frozen liquid wall worn2 (middle worn)
        case 363: //frozen liquid wall worn3 (least worn)
        case 364: //frozen liquid wall
            return 25;
            break;

        case 365: //river n
        case 366: //river s
        case 367: //river e
        case 368: //river w
        case 369: //river nw
        case 370: //river ne
        case 371: //river sw
        case 372: //river se
            return 2;
            break;

        case 373: //stream wall n (below)
        case 374: //stream wall s (below)
        case 375: //stream wall e (below)
        case 376: //stream wall w (below)
        case 377: //stream wall nw (below)
        case 378: //stream wall ne (below)
        case 379: //stream wall sw (below)
        case 380: //stream wall se (below)
        case 381: //stream top (above)
            return 2;
            break;

        case 387: //dry grass floor1
        case 388: //dry grass floor2
        case 389: //dry grass floor3
        case 390: //dry grass floor4
            return 33;
            break;

        case 391: //dead tree
        case 392: //dead sapling
        case 393: //dead shrub
            return 13;
            break;

        case 394: //dead grass floor1
        case 395: //dead grass floor2
        case 396: //dead grass floor3
        case 397: //dead grass floor4
            return 33;
            break;

        case 398: //grass floor1b
        case 399: //grass floor2b
        case 400: //grass floor3b
        case 401: //grass floor4b
            return 16;
            break;

        case 402: //stone boulder
        case 403: //lavastone boulder
        case 404: //featstone boulder
            return 18;
            break;

        case 405: //stone pebbles 1
        case 406: //stone pebbles 2
        case 407: //stone pebbles 3
        case 408: //stone pebbles 4
            return 12;
            break;

        case 409: //lavastone pebbles 1
        case 410: //lavastone pebbles 2
        case 411: //lavastone pebbles 3
        case 412: //lavastone pebbles 4
            return 12;
            break;

        case 413: //featstone pebbles 1
        case 414: //featstone pebbles 2
        case 415: //featstone pebbles 3
        case 416: //featstone pebbles 4
            return 12;
            break;

        case 417: //minstone wall rd2
        case 418: //minstone wall r2d
        case 419: //minstone wall r2u
        case 420: //minstone wall ru2
        case 421: //minstone wall l2u
        case 422: //minstone wall lu2
        case 423: //minstone wall l2d
        case 424: //minstone wall ld2
        case 425: //minstone wall lrud
        case 426: //minstone wall rud
        case 427: //minstone wall lrd
        case 428: //minstone wall lru
        case 429: //minstone wall lud
        case 430: //minstone wall rd
        case 431: //minstone wall ru
        case 432: //minstone wall lu
        case 433: //minstone wall ld
        case 434: //minstone wall ud
        case 435: //minstone wall lr
            return 9;
            break;

        case 436: //minstone fortification
            return 21;
            break;

        case 437: //minstone wall worn1
        case 438: //minstone wall worn2
        case 439: //minstone wall worn3
        case 440: //minstone wall worn4
            return 21;
            break;

        case 441: //minstone floor 1 (cavern raw)
        case 442: //minstone floor 2 (cavern raw)
        case 443: //minstone floor 3 (cavern raw)
        case 444: //minstone floor 4 (cavern raw)
            return 9;
            break;

        case 445: //minstone boulder
            return 18;
            break;

        case 446: //minstone pebbles 1
        case 447: //minstone pebbles 2
        case 448: //minstone pebbles 3
        case 449: //minstone pebbles 4
            return 12;
            break;

        case 450: //frozen liquid wall rd2
        case 451: //frozen liquid wall r2d
        case 452: //frozen liquid wall r2u
        case 453: //frozen liquid wall ru2
        case 454: //frozen liquid wall l2u
        case 455: //frozen liquid wall lu2
        case 456: //frozen liquid wall l2d
        case 457: //frozen liquid wall ld2
        case 458: //frozen liquid wall lrud
        case 459: //frozen liquid wall rud
        case 460: //frozen liquid wall lrd
        case 461: //frozen liquid wall lru
        case 462: //frozen liquid wall lud
        case 463: //frozen liquid wall rd
        case 464: //frozen liquid wall ru
        case 465: //frozen liquid wall lu
        case 466: //frozen liquid wall ld
        case 467: //frozen liquid wall ud
        case 468: //frozen liquid wall lr
            return 25;
            break;

        case 493: //constructed floor detailed
            return 7;
            break;

        case 494: //constructed fortification
            return 7;
            break;

        case 495: //constructed pillar
            return 7;
            break;

        case 496: //constructed wall rd2
        case 497: //constructed wall r2d
        case 498: //constructed wall r2u
        case 499: //constructed wall ru2
        case 500: //constructed wall l2u
        case 501: //constructed wall lu2
        case 502: //constructed wall l2d
        case 503: //constructed wall ld2
        case 504: //constructed wall lrud
        case 505: //constructed wall rud
        case 506: //constructed wall lrd
        case 507: //constructed wall lru
        case 508: //constructed wall lud
        case 509: //constructed wall rd
        case 510: //constructed wall ru
        case 511: //constructed wall lu
        case 512: //constructed wall ld
        case 513: //constructed wall ud
        case 514: //constructed wall lr
            return 22;
            break;

        case 515: //stair up-down constructed
        case 516: //stair down constructed
        case 517: //stair up constructed
            return 4;
            break;

        case 518: //ramp constructed
            return 4;
            break;

        case -1: //not assigned memory
            return 6;
            break;

        default:  //none of the above
            printf("TILE NOT FOUND [%d]\n",in);
            return -1;
            break;
    }

    return 6;
}

int Extractor::isOpenTerrain(int in)
{

    switch (in)
    {
        case -1: //uninitialized tile
        case 1: //slope down
        case 2: //murky pool
        case 19: //driftwood stack
        case 24: //tree
        case 25: //up-down stair frozen liquid
        case 26: //down stair frozen liquid
        case 27: //up stair frozen liquid
        case 32:  //open space
        case 34: //shrub
        case 35: //chasm
        case 36: //up-down stair lavastone
        case 37: //down stair lavastone
        case 38: //up stair lavastone
        case 39: //up-down stair soil
        case 40: //down stair soil
        case 41: //up stair soil
        case 42: //eerie pit
        case 43: //stone floor detailed
        case 44: //lavastone floor detailed
        case 45: //featstone? floor detailed
        case 46: //minstone? floor detailed [calcite]
        case 47: //frozen liquid floor detailed
        case 49: //up-down stair grass1 [muddy?]
        case 50: //down stair grass1 [muddy?]
        case 51: //up stair grass1 [muddy?]
        case 52: //up-down stair grass2
        case 53: //down stair grass2
        case 54: //up stair grass2
        case 55: //up-down stair stone
        case 56: //down stair stone
        case 57: //up stair stone
        case 58: //up-down stair minstone
        case 59: //down stair minstone
        case 60: //up stair minstone
        case 61: //up-down stair featstone
        case 62: //down stair featstone
        case 63: //up stair featstone
        case 67: //campfire
        case 70: //fire
        case 79: //stone pillar
        case 80: //lavastone pillar
        case 81: //featstone pillar
        case 82: //minstone pillar
        case 83: //frozen liquid pillar
        case 89: //waterfall landing
        case 90: //river source
        case 231: //sapling
        case 233: //ramp grass dry
        case 234: //ramp grass dead
        case 235: //ramp grass1 [muddy?]
        case 236: //ramp grass2
        case 237: //ramp stone
        case 238: //ramp lavastone
        case 239: //ramp featstone
        case 240: //ramp minstone
        case 241: //ramp soil
        case 242: //ash1
        case 243: //ash2
        case 244: //ash3
        case 245: //ramp frozen liquid
        case 261: //furrowed soil [road?]
        case 336: //stone floor 1 (raw stone)
        case 337: //stone floor 2 (raw stone)
        case 338: //stone floor 3 (raw stone)
        case 339: //stone floor 4 (raw stone)
        case 340: //lavastone floor 1 (raw stone)
        case 341: //lavastone floor 2 (raw stone)
        case 342: //lavastone floor 3 (raw stone)
        case 343: //lavastone floor 4 (raw stone)
        case 344: //featstone floor 1 (raw stone)
        case 345: //featstone floor 2 (raw stone)
        case 346: //featstone floor 3 (raw stone)
        case 347: //featstone floor 4 (raw stone)
        case 348: //grass floor 1 (raw)
        case 349: //grass floor 2 (raw)
        case 350: //grass floor 3 (raw)
        case 351: //grass floor 4 (raw)
        case 352: //soil floor 1 (raw)
        case 353: //soil floor 2 (raw)
        case 354: //soil floor 3 (raw)
        case 355: //soil floor 4 (raw)
        case 356: //soil floor 1 wet (raw) [red sand?]
        case 357: //soil floor 2 wet (raw) [red sand?]
        case 358: //soil floor 3 wet (raw) [red sand?]
        case 359: //soil floor 4 wet (raw) [red sand?]
        case 365: //river n
        case 366: //river s
        case 367: //river e
        case 368: //river w
        case 369: //river nw
        case 370: //river ne
        case 371: //river sw
        case 372: //river se
        case 387: //dry grass floor1
        case 388: //dry grass floor2
        case 389: //dry grass floor3
        case 390: //dry grass floor4
        case 391: //dead tree
        case 392: //dead sapling
        case 393: //dead shrub
        case 394: //dead grass floor1
        case 395: //dead grass floor2
        case 396: //dead grass floor3
        case 397: //dead grass floor4
        case 398: //grass floor1b
        case 399: //grass floor2b
        case 400: //grass floor3b
        case 401: //grass floor4b
        case 402: //stone boulder
        case 403: //lavastone boulder
        case 404: //featstone boulder
        case 405: //stone pebbles 1
        case 406: //stone pebbles 2
        case 407: //stone pebbles 3
        case 408: //stone pebbles 4
        case 409: //lavastone pebbles 1
        case 410: //lavastone pebbles 2
        case 411: //lavastone pebbles 3
        case 412: //lavastone pebbles 4
        case 413: //featstone pebbles 1
        case 414: //featstone pebbles 2
        case 415: //featstone pebbles 3
        case 416: //featstone pebbles 4
        case 441: //minstone floor 1 (cavern raw)
        case 442: //minstone floor 2 (cavern raw)
        case 443: //minstone floor 3 (cavern raw)
        case 444: //minstone floor 4 (cavern raw)
        case 445: //minstone boulder
        case 446: //minstone pebbles 1
        case 447: //minstone pebbles 2
        case 448: //minstone pebbles 3
        case 449: //minstone pebbles 4
        case 493: //constructed floor detailed
        case 495: //constructed pillar
        case 515: //stair up-down constructed
        case 516: //stair down constructed
        case 517: //stair up constructed
        case 518: //ramp constructed

            return 1;
            break;
    }

    return 0;
}

int Extractor::isFloorTerrain(int in)
{

    switch (in) {
        case 2: //murky pool
        case 19: //driftwood stack
        case 24: //tree
        case 27: //up stair frozen liquid
        case 34: //shrub
        case 38: //up stair lavastone
        case 41: //up stair soil
        case 43: //stone floor detailed
        case 44: //lavastone floor detailed
        case 45: //featstone? floor detailed
        case 46: //minstone? floor detailed [calcite]
        case 47: //frozen liquid floor detailed
        case 51: //up stair grass1 [muddy?]
        case 54: //up stair grass2
        case 57: //up stair stone
        case 60: //up stair minstone
        case 63: //up stair featstone
        case 67: //campfire
        case 70: //fire
        case 79: //stone pillar
        case 80: //lavastone pillar
        case 81: //featstone pillar
        case 82: //minstone pillar
        case 83: //frozen liquid pillar
        case 89: //waterfall landing
        case 90: //river source
        case 231: //sapling
        case 233: //ramp grass dry
        case 234: //ramp grass dead
        case 235: //ramp grass1 [muddy?]
        case 236: //ramp grass2
        case 237: //ramp stone
        case 238: //ramp lavastone
        case 239: //ramp featstone
        case 240: //ramp minstone
        case 241: //ramp soil
        case 242: //ash1
        case 243: //ash2
        case 244: //ash3
        case 245: //ramp frozen liquid
        case 261: //furrowed soil [road?]
        case 336: //stone floor 1 (raw stone)
        case 337: //stone floor 2 (raw stone)
        case 338: //stone floor 3 (raw stone)
        case 339: //stone floor 4 (raw stone)
        case 340: //lavastone floor 1 (raw stone)
        case 341: //lavastone floor 2 (raw stone)
        case 342: //lavastone floor 3 (raw stone)
        case 343: //lavastone floor 4 (raw stone)
        case 344: //featstone floor 1 (raw stone)
        case 345: //featstone floor 2 (raw stone)
        case 346: //featstone floor 3 (raw stone)
        case 347: //featstone floor 4 (raw stone)
        case 348: //grass floor 1 (raw)
        case 349: //grass floor 2 (raw)
        case 350: //grass floor 3 (raw)
        case 351: //grass floor 4 (raw)
        case 352: //soil floor 1 (raw)
        case 353: //soil floor 2 (raw)
        case 354: //soil floor 3 (raw)
        case 355: //soil floor 4 (raw)
        case 356: //soil floor 1 wet (raw) [red sand?]
        case 357: //soil floor 2 wet (raw) [red sand?]
        case 358: //soil floor 3 wet (raw) [red sand?]
        case 359: //soil floor 4 wet (raw) [red sand?]
        case 365: //river n
        case 366: //river s
        case 367: //river e
        case 368: //river w
        case 369: //river nw
        case 370: //river ne
        case 371: //river sw
        case 372: //river se
        case 387: //dry grass floor1
        case 388: //dry grass floor2
        case 389: //dry grass floor3
        case 390: //dry grass floor4
        case 391: //dead tree
        case 392: //dead sapling
        case 393: //dead shrub
        case 394: //dead grass floor1
        case 395: //dead grass floor2
        case 396: //dead grass floor3
        case 397: //dead grass floor4
        case 398: //grass floor1b
        case 399: //grass floor2b
        case 400: //grass floor3b
        case 401: //grass floor4b
        case 402: //stone boulder
        case 403: //lavastone boulder
        case 404: //featstone boulder
        case 405: //stone pebbles 1
        case 406: //stone pebbles 2
        case 407: //stone pebbles 3
        case 408: //stone pebbles 4
        case 409: //lavastone pebbles 1
        case 410: //lavastone pebbles 2
        case 411: //lavastone pebbles 3
        case 412: //lavastone pebbles 4
        case 413: //featstone pebbles 1
        case 414: //featstone pebbles 2
        case 415: //featstone pebbles 3
        case 416: //featstone pebbles 4
        case 441: //minstone floor 1 (cavern raw)
        case 442: //minstone floor 2 (cavern raw)
        case 443: //minstone floor 3 (cavern raw)
        case 444: //minstone floor 4 (cavern raw)
        case 445: //minstone boulder
        case 446: //minstone pebbles 1
        case 447: //minstone pebbles 2
        case 448: //minstone pebbles 3
        case 449: //minstone pebbles 4
        case 493: //constructed floor detailed
        case 495: //constructed pillar
        case 517: //stair up constructed
        case 518: //ramp constructed
            return 1;
            break;
    }

    return 0;
}

int Extractor::isRampTerrain(int in)
{

    switch (in) {
        case 233: //ramp grass dry
        case 234: //ramp grass dead
        case 235: //ramp grass1 [muddy?]
        case 236: //ramp grass2
        case 237: //ramp stone
        case 238: //ramp lavastone
        case 239: //ramp featstone
        case 240: //ramp minstone
        case 241: //ramp soil
        case 245: //ramp frozen liquid
        case 518: //ramp constructed
            return 1;
            break;
    }

    return 0;
}

int Extractor::isStairTerrain(int in)
{

    switch (in) {
        case 25: //up-down stair frozen liquid
        case 26: //down stair frozen liquid
        case 27: //up stair frozen liquid
        case 36: //up-down stair lavastone
        case 37: //down stair lavastone
        case 38: //up stair lavastone
        case 39: //up-down stair soil
        case 40: //down stair soil
        case 41: //up stair soil
        case 49: //up-down stair grass1 [muddy?]
        case 50: //down stair grass1 [muddy?]
        case 51: //up stair grass1 [muddy?]
        case 52: //up-down stair grass2
        case 53: //down stair grass2
        case 54: //up stair grass2
        case 55: //up-down stair stone
        case 56: //down stair stone
        case 57: //up stair stone
        case 58: //up-down stair minstone
        case 59: //down stair minstone
        case 60: //up stair minstone
        case 61: //up-down stair featstone
        case 62: //down stair featstone
        case 63: //up stair featstone
        case 515: //stair up-down constructed
        case 516: //stair down constructed
        case 517: //stair up constructed
            return 1;
            break;
    }

    return 0;
}

int Extractor::isWallTerrain(int in)
{

    switch (in) {
        case 65: //stone fortification
        case 79: //stone pillar
        case 80: //lavastone pillar
        case 81: //featstone pillar
        case 82: //minstone pillar
        case 83: //frozen liquid pillar
        case 176: //stone wall worn1 (most worn)
        case 177: //stone wall worn2 (sorta worn)
        case 178: //stone wall worn3 (least worn)
        case 219: //stone wall (not worn)
        case 258: //frozen liquid 1
        case 259: //frozen liquid 2
        case 260: //frozen liquid 3
        case 262: //frozen liquid 0
        case 265: //soil wall
        case 269: //lavastone wall rd2
        case 270: //lavastone wall r2d
        case 271: //lavastone wall r2u
        case 272: //lavastone wall ru2
        case 273: //lavastone wall l2u
        case 274: //lavastone wall lu2
        case 275: //lavastone wall l2d
        case 276: //lavastone wall ld2
        case 277: //lavastone wall lrud
        case 278: //lavastone wall rud
        case 279: //lavastone wall lrd
        case 280: //lavastone wall lru
        case 281: //lavastone wall lud
        case 282: //lavastone wall rd
        case 283: //lavastone wall ru
        case 284: //lavastone wall lu
        case 285: //lavastone wall ld
        case 286: //lavastone wall ud
        case 287: //lavastone wall lr
        case 288: //featstone wall rd2
        case 289: //featstone wall r2d
        case 290: //featstone wall r2u
        case 291: //featstone wall ru2
        case 292: //featstone wall l2u
        case 293: //featstone wall lu2
        case 294: //featstone wall l2d
        case 295: //featstone wall ld2
        case 296: //featstone wall lrud
        case 297: //featstone wall rud
        case 298: //featstone wall lrd
        case 299: //featstone wall lru
        case 300: //featstone wall lud
        case 301: //featstone wall rd
        case 382: //featstone wall ru
        case 303: //featstone wall lu
        case 304: //featstone wall ld
        case 305: //featstone wall ud
        case 306: //featstone wall lr
        case 307: //stone wall rd2
        case 308: //stone wall r2d
        case 309: //stone wall r2u
        case 310: //stone wall ru2
        case 311: //stone wall l2u
        case 312: //stone wall lu2
        case 313: //stone wall l2d
        case 314: //stone wall ld2
        case 315: //stone wall lrud
        case 316: //stone wall rud
        case 317: //stone wall lrd
        case 318: //stone wall lru
        case 319: //stone wall lud
        case 320: //stone wall rd
        case 321: //stone wall ru
        case 322: //stone wall lu
        case 323: //stone wall ld
        case 324: //stone wall ud
        case 325: //stone wall lr
        case 326: //lavastone fortification
        case 327: //featstone fortification
        case 328: //lavastone wall worn1 (most worn)
        case 329: //lavastone wall worn2 (middle worn)
        case 330: //lavastone wall worn3 (least worn)
        case 331: //lavastone wall
        case 332: //featstone wall worn1 (most worn)
        case 333: //featstone wall worn2 (middle worn)
        case 334: //featstone wall worn3 (least worn)
        case 335: //featstone wall
        case 360: //frozen liquid fortification
        case 361: //frozen liquid wall worn1 (most worn)
        case 362: //frozen liquid wall worn2 (middle worn)
        case 363: //frozen liquid wall worn3 (least worn)
        case 364: //frozen liquid wall
        case 373: //stream wall n (below)
        case 374: //stream wall s (below)
        case 375: //stream wall e (below)
        case 376: //stream wall w (below)
        case 377: //stream wall nw (below)
        case 378: //stream wall ne (below)
        case 379: //stream wall sw (below)
        case 380: //stream wall se (below)
        case 417: //minstone wall rd2
        case 418: //minstone wall r2d
        case 419: //minstone wall r2u
        case 420: //minstone wall ru2
        case 421: //minstone wall l2u
        case 422: //minstone wall lu2
        case 423: //minstone wall l2d
        case 424: //minstone wall ld2
        case 425: //minstone wall lrud
        case 426: //minstone wall rud
        case 427: //minstone wall lrd
        case 428: //minstone wall lru
        case 429: //minstone wall lud
        case 430: //minstone wall rd
        case 431: //minstone wall ru
        case 432: //minstone wall lu
        case 433: //minstone wall ld
        case 434: //minstone wall ud
        case 435: //minstone wall lr
        case 436: //minstone fortification
        case 437: //minstone wall worn1
        case 438: //minstone wall worn2
        case 439: //minstone wall worn3
        case 440: //minstone wall worn4
        case 450: //frozen liquid wall rd2
        case 451: //frozen liquid wall r2d
        case 452: //frozen liquid wall r2u
        case 453: //frozen liquid wall ru2
        case 454: //frozen liquid wall l2u
        case 455: //frozen liquid wall lu2
        case 456: //frozen liquid wall l2d
        case 457: //frozen liquid wall ld2
        case 458: //frozen liquid wall lrud
        case 459: //frozen liquid wall rud
        case 460: //frozen liquid wall lrd
        case 461: //frozen liquid wall lru
        case 462: //frozen liquid wall lud
        case 463: //frozen liquid wall rd
        case 464: //frozen liquid wall ru
        case 465: //frozen liquid wall lu
        case 466: //frozen liquid wall ld
        case 467: //frozen liquid wall ud
        case 468: //frozen liquid wall lr
        case 494: //constructed fortification
        case 495: //constructed pillar
        case 496: //constructed wall rd2
        case 497: //constructed wall r2d
        case 498: //constructed wall r2u
        case 499: //constructed wall ru2
        case 500: //constructed wall l2u
        case 501: //constructed wall lu2
        case 502: //constructed wall l2d
        case 503: //constructed wall ld2
        case 504: //constructed wall lrud
        case 505: //constructed wall rud
        case 506: //constructed wall lrd
        case 507: //constructed wall lru
        case 508: //constructed wall lud
        case 509: //constructed wall rd
        case 510: //constructed wall ru
        case 511: //constructed wall lu
        case 512: //constructed wall ld
        case 513: //constructed wall ud
        case 514: //constructed wall lr
            return 1;
            break;
    }

    return 0;
}

