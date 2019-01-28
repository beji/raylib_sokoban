#include <assert.h>
#include <vector>

#include "../loguru/loguru.hpp"
#include "../raylib/src/raylib.h"

#include "constants.hpp"
#include "drawfunctions.hpp"
#include "map.hpp"

int main(int argc, char **argv) {

  assert(argc == 2); // TODO: Proper error handling

  loguru::init(argc, argv);

  auto world = InitWorld();

  ReadMap(&world, std::string(argv[1]));

  // Initialization
  //--------------------------------------------------------------------------------------
  int screenWidth = 800;
  int screenHeight = 450;
  float doubleTapDelay = 0;
  InitWindow(screenWidth, screenHeight, "raylib sokoban thingy");

  SetTargetFPS(60);

  // float timeSinceLastTick = 0;

  DLOG_F(INFO, "Entering main loop");
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {

    // TODO: The whole input buffer system is crap

    /* Get Input and store in buffer
       This will hopefully allow handling of input done between ticks
     */
    if (IsKeyDown(KEY_RIGHT)) {
      ResetInputBuffer(&world);
      world.inputBuffer.right = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
      ResetInputBuffer(&world);
      world.inputBuffer.left = true;
    }
    if (IsKeyDown(KEY_UP)) {
      ResetInputBuffer(&world);
      world.inputBuffer.up = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
      ResetInputBuffer(&world);
      world.inputBuffer.down = true;
    }
    if (IsKeyDown(KEY_R)) {
      ResetInputBuffer(&world);
      world.inputBuffer.reset = true;
    }

    auto frametime = GetFrameTime();

    if (doubleTapDelay == 0) {
      doubleTapDelay = DOUBLETAPDELAY;

      // Get buffered input and move player
      if (world.inputBuffer.right) {
        DLOG_F(INFO, "Input: right");
        TryMovePlayerRight(&world);
      }
      if (world.inputBuffer.left) {
        DLOG_F(INFO, "Input: left");
        TryMovePlayerLeft(&world);
      }
      if (world.inputBuffer.up) {
        DLOG_F(INFO, "Input: up");
        TryMovePlayerUp(&world);
      }
      if (world.inputBuffer.down) {
        DLOG_F(INFO, "Input: down");
        TryMovePlayerDown(&world);
      }
      if (world.inputBuffer.reset) {
        DLOG_F(INFO, "Input: reset");
        ResetMap(&world);
      }
      ResetInputBuffer(&world);
      // Update the player position struct
      UpdatePlayerPosition(&world);
    } else if (doubleTapDelay > 0) {
      doubleTapDelay = doubleTapDelay - frametime;
    } else { // doubleTapDelay < 0
      doubleTapDelay = 0;
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);

    for (std::vector<char>::size_type i = 0; i != world.map.map.size(); i++) {
      switch (world.map.map[i]) {
      case 'X':
        DrawWall(i, world.map.mapwidth, RED);
        break;
      case 'b':
        DrawTileOutline(i, world.map.mapwidth, WHITE);
        DrawBox(i, world.map.mapwidth, BLUE);
        break;
      case '@':
        DrawTileOutline(i, world.map.mapwidth, WHITE);
        DrawPlayer(i, world.map.mapwidth, BLUE);
      default:
        DrawTileOutline(i, world.map.mapwidth, WHITE);
      }
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
