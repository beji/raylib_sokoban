#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

using mapdef = struct _MAPDEF {
  int mapwidth;
  int mapheight;
  std::vector<char> map;
  std::vector<char> originalmap;
};

using position = struct _POSITION {
  int x;
  int y;
};

using world = struct _WORLD {
  mapdef map;
  position playerPosition;
  int turns;
};

int GetGridYPosition(const int i, const int mapwidth);
int GetGridXPosition(const int i, const int mapwidth);

void ReadMap(world *world, const std::string file);

void UpdatePlayerPosition(world *world);

void TryMovePlayerRight(world *world);
void TryMovePlayerLeft(world *world);
void TryMovePlayerUp(world *world);
void TryMovePlayerDown(world *world);

void ResetMap(world *world);

world InitWorld();

#endif /* MAP_H */
