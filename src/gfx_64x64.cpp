//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_tile.hpp"
#include "my_ui.hpp"

void gfx_init_64x64(void)
{
  std::initializer_list< std::string > tiles = {
      // clang-format off
      // ##############################################################################
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      // ##############################################################################
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      // ##############################################################################
      "dungeon_wall.IS_JOIN_TL2",
      "dungeon_wall.IS_JOIN_T_3",
      "dungeon_wall.IS_JOIN_TR2",
      "dungeon_wall.IS_JOIN_TOP",
      "dungeon_wall.IS_JOIN_X4_180",
      "dungeon_wall.IS_JOIN_X4_270",
      "dungeon_wall.IS_JOIN_X2_270",
      "dungeon_wall.IS_JOIN_X2",
      "dungeon_wall.IS_JOIN_X1_180",
      "dungeon_wall.IS_JOIN_X1",
      "dungeon_wall.IS_JOIN_X",
      "",
      // ##############################################################################
      "dungeon_wall.IS_JOIN_T270_3",
      "dungeon_wall.IS_JOIN_BLOCK",
      "dungeon_wall.IS_JOIN_T90_3",
      "dungeon_wall.IS_JOIN_VERT",
      "dungeon_wall.IS_JOIN_X4_90",
      "dungeon_wall.IS_JOIN_X4",
      "dungeon_wall.IS_JOIN_X2_180",
      "dungeon_wall.IS_JOIN_X2_90",
      "dungeon_wall.IS_JOIN_X1_90",
      "dungeon_wall.IS_JOIN_X1_270",
      "dungeon_wall.IS_JOIN_X3_180",
      "dungeon_wall.IS_JOIN_X3",
      // ##############################################################################
      "dungeon_wall.IS_JOIN_BL2",
      "dungeon_wall.IS_JOIN_T180_3",
      "dungeon_wall.IS_JOIN_BR2",
      "dungeon_wall.IS_JOIN_BOT",
      "dungeon_wall.IS_JOIN_T_1",
      "dungeon_wall.IS_JOIN_T_2",
      "dungeon_wall.IS_JOIN_T270_2",
      "dungeon_wall.IS_JOIN_T90_1",
      "dungeon_wall.IS_JOIN_T",
      "dungeon_wall.IS_JOIN_T90",
      "dungeon_wall.IS_JOIN_TL",
      "dungeon_wall.IS_JOIN_TR",
      // ##############################################################################
      "dungeon_wall.IS_JOIN_LEFT",
      "dungeon_wall.IS_JOIN_HORIZ",
      "dungeon_wall.IS_JOIN_RIGHT",
      "dungeon_wall.IS_JOIN_NODE",
      "dungeon_wall.IS_JOIN_T180_2",
      "dungeon_wall.IS_JOIN_T180_1",
      "dungeon_wall.IS_JOIN_T270_1",
      "dungeon_wall.IS_JOIN_T90_2",
      "dungeon_wall.IS_JOIN_T270",
      "dungeon_wall.IS_JOIN_T180",
      "dungeon_wall.IS_JOIN_BL",
      "dungeon_wall.IS_JOIN_BR",
      // ##############################################################################
  };
  const std::vector< std::string > arr(tiles);
  tile_load_arr("data/gfx/tiles_64x64.tga", "tiles_64x64", 64, 64, arr);
}