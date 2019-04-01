#include "map.hpp"

#include <assert.h>
#include <fstream>

#include <loguru.hpp>

#ifndef NDEBUG

#define ASSERT_SANE_WORLD                                                      \
  CHECK_F(world->map.map.size() != 0, "The map seems to be empty");
#define ASSERT_SANE_PLAYER                                                     \
  CHECK_F(playerIndex > 0, "The map doesn't seem to contain a player");
#define ASSERT_SANE_WORLD_PLAYER                                               \
  ASSERT_SANE_WORLD                                                            \
  ASSERT_SANE_PLAYER
#define ASSERT_SANE_WORLD_PLAYER_TARGET                                        \
  ASSERT_SANE_WORLD_PLAYER                                                     \
  CHECK_F(targetIndex > 0, "The target doesn't seem to exist");                \
  CHECK_F(playerIndex != targetIndex,                                          \
          "Player and target seem to be the same thing");

#else
#define ASSERT_SANE_WORLD
#define ASSERT_SANE_PLAYER
#define ASSERT_SANE_WORLD_PLAYER
#define ASSERT_SANE_WORLD_PLAYER_TARGET
#endif

int GetGridYPosition(const int i, const int mapwidth) { return i / mapwidth; }
int GetGridXPosition(const int i, const int mapwidth) { return i % mapwidth; }

void ReadMap(world *world, const std::string file) {
  std::string line;
  std::ifstream mapfile(file);
  int linecount = 0;
  bool playerFound = false;
  int boxCount = 0;

  CHECK_F(mapfile.good(), "The mapfile doesn't seem to work or be readable");
  if (mapfile.is_open()) {
    LOG_F(INFO, "Reading file %s", file.c_str());
    while (getline(mapfile, line)) {
      DLOG_F(INFO, "Line: %s", line.c_str());
      if (linecount == 0) {
        world->map.mapwidth = std::stoi(line);
        DLOG_F(INFO, "Map width is %i", world->map.mapwidth);
      } else {
        CHECK_F((int)line.length() == world->map.mapwidth,
                "The line length doesn't match the set map width");
        for (std::string::size_type i = 0; i < line.length(); ++i) {
          std::string::value_type character = line[i];
          if (character == '@') {
            playerFound = true;
          }
          if (character == 'b') {
            boxCount++;
          }
          world->map.map.push_back(character);
          world->map.originalmap.push_back(character);
        }
      }
      linecount++;
    }
    world->map.mapheight = linecount + 1;
  } else {
    LOG_F(ERROR, "Failed to open file %s", file.c_str());
  }
  CHECK_F(playerFound, "Didn't find a player on the map");
  CHECK_F(boxCount > 0, "Didn't find a single box on the map");
  UpdatePlayerPosition(world);
}

void UpdatePlayerPosition(world *world) {
  ASSERT_SANE_WORLD;
  for (std::vector<char>::size_type i = 0; i != world->map.map.size(); i++) {
    if (world->map.map[i] == '@') {
      world->playerPosition.x = GetGridXPosition(i, world->map.mapwidth);
      world->playerPosition.y = GetGridYPosition(i, world->map.mapwidth);
      return;
    }
  }
}

void RestoreTileFromOriginal(world *world, const size_t playerIndex) {
  ASSERT_SANE_WORLD_PLAYER;
  // This will be useful later on when we have targets to push blocks on
  if (world->map.originalmap[playerIndex] == '.') {
    world->map.map[playerIndex] = world->map.originalmap[playerIndex];
  } else {
    world->map.map[playerIndex] = '.';
  }
}

void MoveBlockAndPlayerOnMap(world *world, const size_t playerIndex,
                             const size_t targetIndex) {
  /* we need to find the direction that the player is trying to push the block
     towards which can be done by comparing playerIndex and targetIndex

     @b -> playerIndex = targetIndex - 1
     b@ -> playerIndex = targetIndex + 1
     @
     b -> playerIndex < targetIndex
     b
     @ -> playerIndex > targetIndex
  */
  ASSERT_SANE_WORLD_PLAYER_TARGET;
  if (playerIndex == targetIndex - 1) {
    size_t blockTargetIndex = targetIndex + 1;
    if (world->map.map[blockTargetIndex] == '.') {
      world->map.map[blockTargetIndex] = 'b';
      world->map.map[targetIndex] = '@';
      RestoreTileFromOriginal(world, playerIndex);
      world->turns++;
    }
  } else if (playerIndex == targetIndex + 1) {
    size_t blockTargetIndex = targetIndex - 1;
    if (world->map.map[blockTargetIndex] == '.') {
      world->map.map[blockTargetIndex] = 'b';
      world->map.map[targetIndex] = '@';
      RestoreTileFromOriginal(world, playerIndex);
      world->turns++;
    }
  } else if (playerIndex < targetIndex) {
    size_t blockTargetIndex = targetIndex + world->map.mapwidth;
    if (world->map.map[blockTargetIndex] == '.') {
      world->map.map[blockTargetIndex] = 'b';
      world->map.map[targetIndex] = '@';
      RestoreTileFromOriginal(world, playerIndex);
      world->turns++;
    }
  } else if (playerIndex > targetIndex) {
    size_t blockTargetIndex = targetIndex - world->map.mapwidth;
    if (world->map.map[blockTargetIndex] == '.') {
      world->map.map[blockTargetIndex] = 'b';
      world->map.map[targetIndex] = '@';
      RestoreTileFromOriginal(world, playerIndex);
      world->turns++;
    }
  }
}

void MovePlayerOnMap(world *world, const size_t playerIndex,
                     const size_t targetIndex) {
  ASSERT_SANE_WORLD_PLAYER_TARGET;
  switch (world->map.map[targetIndex]) {
  case '.': // Target space is empty
    RestoreTileFromOriginal(world, playerIndex);
    world->map.map[targetIndex] = '@';
    world->turns++;
    break;
  case 'b': // Target space is a moveable block
    MoveBlockAndPlayerOnMap(world, playerIndex, targetIndex);
    break;
  }
}

void TryMovePlayerRight(world *world) {
  ASSERT_SANE_WORLD;
  // Player is already at the right edge of the map
  if (world->playerPosition.x == world->map.mapwidth - 1) {
    return;
  }

  size_t playerIndex =
      world->playerPosition.y * world->map.mapwidth + world->playerPosition.x;

  MovePlayerOnMap(world, playerIndex, playerIndex + 1);
}

void TryMovePlayerLeft(world *world) {
  ASSERT_SANE_WORLD;
  // Player is already at the left edge of the map
  if (world->playerPosition.x == 0) {
    return;
  }

  size_t playerIndex =
      world->playerPosition.y * world->map.mapwidth + world->playerPosition.x;

  MovePlayerOnMap(world, playerIndex, playerIndex - 1);
}

void TryMovePlayerUp(world *world) {
  ASSERT_SANE_WORLD;

  // Player is already at the top edge of the map
  if (world->playerPosition.y == 0) {
    return;
  }

  size_t playerIndex =
      world->playerPosition.y * world->map.mapwidth + world->playerPosition.x;
  size_t targetIndex = (world->playerPosition.y - 1) * world->map.mapwidth +
                       world->playerPosition.x;

  MovePlayerOnMap(world, playerIndex, targetIndex);
}

void TryMovePlayerDown(world *world) {
  ASSERT_SANE_WORLD;
  // Player is already at the bottom edge of the map
  if (world->playerPosition.y == world->map.mapheight - 1) {
    return;
  }

  size_t playerIndex =
      world->playerPosition.y * world->map.mapwidth + world->playerPosition.x;
  size_t targetIndex = (world->playerPosition.y + 1) * world->map.mapwidth +
                       world->playerPosition.x;

  MovePlayerOnMap(world, playerIndex, targetIndex);
}

void ResetMap(world *world) {
  ASSERT_SANE_WORLD;
  CHECK_F(world->map.originalmap.size() > 0,
          "The original map seems to be empty");
  world->map.map = world->map.originalmap;
  world->turns = 0;
}

world InitWorld() {
  world world;
  world.turns = 0;
  return world;
}
