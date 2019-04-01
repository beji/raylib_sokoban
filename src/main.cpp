#include <assert.h>
#include <vector>
#include <stdio.h>

#include <loguru.hpp>

#include <raylib.h>

#include "constants.hpp"
#include "drawfunctions.hpp"
#include "map.hpp"

int main(int argc, char **argv) {

  loguru::init(argc, argv);
  CHECK_F(argc == 2, "You need to pass a mapfile to the application!");

  auto world = InitWorld();

  ReadMap(&world, std::string(argv[1]));

  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(SCREENWIDTH, SCREENHEIGHT, "raylib sokoban thingy");

  SetTargetFPS(60);

  // float timeSinceLastTick = 0;

  DLOG_F(INFO, "Entering main loop");
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {

    if (IsKeyPressed(KEY_RIGHT)) {
      DLOG_F(INFO, "Input: right");
      TryMovePlayerRight(&world);
    }
    if (IsKeyPressed(KEY_LEFT)) {
      DLOG_F(INFO, "Input: left");
      TryMovePlayerLeft(&world);
    }
    if (IsKeyPressed(KEY_UP)) {
      DLOG_F(INFO, "Input: up");
      TryMovePlayerUp(&world);
    }
    if (IsKeyPressed(KEY_DOWN)) {
      DLOG_F(INFO, "Input: down");
      TryMovePlayerDown(&world);
    }
    if (IsKeyPressed(KEY_R)) {
      DLOG_F(INFO, "Input: reset");
      ResetMap(&world);
    }

    UpdatePlayerPosition(&world);

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
        DrawTileOutline(i, world.map.mapwidth, RAYWHITE);
        DrawBox(i, world.map.mapwidth, BLUE);
        break;
      case '@':
        DrawTileOutline(i, world.map.mapwidth, RAYWHITE);
        DrawPlayer(i, world.map.mapwidth, SKYBLUE);
        break;
      default:
        DrawTileOutline(i, world.map.mapwidth, RAYWHITE);
      }
    }

    char turnsBuffer [TURNBUFFER_LENGTH];
    int cx = snprintf(turnsBuffer, TURNBUFFER_LENGTH, "Turns: %i", world.turns);
    CHECK_F(cx >= 0 && cx < TURNBUFFER_LENGTH, "Something went wrong while writing the turnBuffer");
    DrawText(turnsBuffer, 10, SCREENHEIGHT - 28, 18, RAYWHITE);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
