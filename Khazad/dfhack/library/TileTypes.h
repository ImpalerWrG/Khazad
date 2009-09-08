#ifndef TILETYPES_H_INCLUDED
#define TILETYPES_H_INCLUDED

/// TODO: turn into XML

bool isWallTerrain(int in);
bool isFloorTerrain(int in);
bool isRampTerrain(int in);
bool isStairTerrain(int in);
bool isOpenTerrain(int in);
int picktexture(int in);

#endif // TILETYPES_H_INCLUDED
