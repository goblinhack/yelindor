//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_tile.hpp"
#include "my_ui.hpp"

void gfx_init_32x32(void)
{
  {
    const char *tiles[] = {
        /* clang-format off */
        // ##############################################################################
        "exit.0",
        "exit.1",
        "exit.2",
        "exit.3",
        "exit.4",
        "exit.5",
        "exit.6",
        "exit.7",
        // ##############################################################################
        /* clang-format on */
    };
    tile_load_arr("data/gfx/tiles_32x32.tga", "tiles_32x32", 32, 32, ARRAY_SIZE(tiles), tiles);
  }

  {
    const char *tiles[] = {
        /* clang-format off */
        // ##############################################################################
        "door.IS_JOIN_TL2",
        "door.IS_JOIN_T_3",
        "door.IS_JOIN_TR2",
        "door.IS_JOIN_TOP",
        "door.IS_JOIN_X4_180",
        "door.IS_JOIN_X4_270",
        "door.IS_JOIN_X2_270",
        "door.IS_JOIN_X2",
        "door.IS_JOIN_X1_180",
        "door.IS_JOIN_X1",
        "door.IS_JOIN_X",
        "",
        // ##############################################################################
        "door.IS_JOIN_T270_3",
        "door.IS_JOIN_BLOCK",
        "door.IS_JOIN_T90_3",
        "door.IS_JOIN_VERT",
        "door.IS_JOIN_X4_90",
        "door.IS_JOIN_X4",
        "door.IS_JOIN_X2_180",
        "door.IS_JOIN_X2_90",
        "door.IS_JOIN_X1_90",
        "door.IS_JOIN_X1_270",
        "door.IS_JOIN_X3_180",
        "door.IS_JOIN_X3",
        // ##############################################################################
        "door.IS_JOIN_BL2",
        "door.IS_JOIN_T180_3",
        "door.IS_JOIN_BR2",
        "door.IS_JOIN_BOT",
        "door.IS_JOIN_T_1",
        "door.IS_JOIN_T_2",
        "door.IS_JOIN_T270_2",
        "door.IS_JOIN_T90_1",
        "door.IS_JOIN_T",
        "door.IS_JOIN_T90",
        "door.IS_JOIN_TL",
        "door.IS_JOIN_TR",
        // ##############################################################################
        "door.IS_JOIN_LEFT",
        "door.IS_JOIN_HORIZ",
        "door.IS_JOIN_RIGHT",
        "door.IS_JOIN_NODE",
        "door.IS_JOIN_T180_2",
        "door.IS_JOIN_T180_1",
        "door.IS_JOIN_T270_1",
        "door.IS_JOIN_T90_2",
        "door.IS_JOIN_T270",
        "door.IS_JOIN_T180",
        "door.IS_JOIN_BL",
        "door.IS_JOIN_BR",
        // ##############################################################################
        "door.IS_JOIN_TL2.shadow",
        "door.IS_JOIN_T_3.shadow",
        "door.IS_JOIN_TR2.shadow",
        "door.IS_JOIN_TOP.shadow",
        "door.IS_JOIN_X4_180.shadow",
        "door.IS_JOIN_X4_270.shadow",
        "door.IS_JOIN_X2_270.shadow",
        "door.IS_JOIN_X2.shadow",
        "door.IS_JOIN_X1_180.shadow",
        "door.IS_JOIN_X1.shadow",
        "door.IS_JOIN_X.shadow",
        "",
        // ##############################################################################
        "door.IS_JOIN_T270_3.shadow",
        "door.IS_JOIN_BLOCK.shadow",
        "door.IS_JOIN_T90_3.shadow",
        "door.IS_JOIN_VERT.shadow",
        "door.IS_JOIN_X4_90.shadow",
        "door.IS_JOIN_X4.shadow",
        "door.IS_JOIN_X2_180.shadow",
        "door.IS_JOIN_X2_90.shadow",
        "door.IS_JOIN_X1_90.shadow",
        "door.IS_JOIN_X1_270.shadow",
        "door.IS_JOIN_X3_180.shadow",
        "door.IS_JOIN_X3.shadow",
        // ##############################################################################
        "door.IS_JOIN_BL2.shadow",
        "door.IS_JOIN_T180_3.shadow",
        "door.IS_JOIN_BR2.shadow",
        "door.IS_JOIN_BOT.shadow",
        "door.IS_JOIN_T_1.shadow",
        "door.IS_JOIN_T_2.shadow",
        "door.IS_JOIN_T270_2.shadow",
        "door.IS_JOIN_T90_1.shadow",
        "door.IS_JOIN_T.shadow",
        "door.IS_JOIN_T90.shadow",
        "door.IS_JOIN_TL.shadow",
        "door.IS_JOIN_TR.shadow",
        // ##############################################################################
        "door.IS_JOIN_LEFT.shadow",
        "door.IS_JOIN_HORIZ.shadow",
        "door.IS_JOIN_RIGHT.shadow",
        "door.IS_JOIN_NODE.shadow",
        "door.IS_JOIN_T180_2.shadow",
        "door.IS_JOIN_T180_1.shadow",
        "door.IS_JOIN_T270_1.shadow",
        "door.IS_JOIN_T90_2.shadow",
        "door.IS_JOIN_T270.shadow",
        "door.IS_JOIN_T180.shadow",
        "door.IS_JOIN_BL.shadow",
        "door.IS_JOIN_BR.shadow",
        // ##############################################################################
        /* clang-format on */
    };
    tile_load_arr("data/gfx/tiles_32x32_door.tga", "tiles_32x32_door", 32, 32, ARRAY_SIZE(tiles), tiles);
  }

  {
    const char *tiles[] = {
        /* clang-format off */
        // ##############################################################################
        "wall1.IS_JOIN_TL2",
        "wall1.IS_JOIN_T_3",
        "wall1.IS_JOIN_TR2",
        "wall1.IS_JOIN_TOP",
        "wall1.IS_JOIN_X4_180",
        "wall1.IS_JOIN_X4_270",
        "wall1.IS_JOIN_X2_270",
        "wall1.IS_JOIN_X2",
        "wall1.IS_JOIN_X1_180",
        "wall1.IS_JOIN_X1",
        "wall1.IS_JOIN_X",
        "",
        // ##############################################################################
        "wall1.IS_JOIN_T270_3",
        "wall1.IS_JOIN_BLOCK",
        "wall1.IS_JOIN_T90_3",
        "wall1.IS_JOIN_VERT",
        "wall1.IS_JOIN_X4_90",
        "wall1.IS_JOIN_X4",
        "wall1.IS_JOIN_X2_180",
        "wall1.IS_JOIN_X2_90",
        "wall1.IS_JOIN_X1_90",
        "wall1.IS_JOIN_X1_270",
        "wall1.IS_JOIN_X3_180",
        "wall1.IS_JOIN_X3",
        // ##############################################################################
        "wall1.IS_JOIN_BL2",
        "wall1.IS_JOIN_T180_3",
        "wall1.IS_JOIN_BR2",
        "wall1.IS_JOIN_BOT",
        "wall1.IS_JOIN_T_1",
        "wall1.IS_JOIN_T_2",
        "wall1.IS_JOIN_T270_2",
        "wall1.IS_JOIN_T90_1",
        "wall1.IS_JOIN_T",
        "wall1.IS_JOIN_T90",
        "wall1.IS_JOIN_TL",
        "wall1.IS_JOIN_TR",
        // ##############################################################################
        "wall1.IS_JOIN_LEFT",
        "wall1.IS_JOIN_HORIZ",
        "wall1.IS_JOIN_RIGHT",
        "wall1.IS_JOIN_NODE",
        "wall1.IS_JOIN_T180_2",
        "wall1.IS_JOIN_T180_1",
        "wall1.IS_JOIN_T270_1",
        "wall1.IS_JOIN_T90_2",
        "wall1.IS_JOIN_T270",
        "wall1.IS_JOIN_T180",
        "wall1.IS_JOIN_BL",
        "wall1.IS_JOIN_BR",
        // ##############################################################################
        "wall1.IS_JOIN_TL2.shadow",
        "wall1.IS_JOIN_T_3.shadow",
        "wall1.IS_JOIN_TR2.shadow",
        "wall1.IS_JOIN_TOP.shadow",
        "wall1.IS_JOIN_X4_180.shadow",
        "wall1.IS_JOIN_X4_270.shadow",
        "wall1.IS_JOIN_X2_270.shadow",
        "wall1.IS_JOIN_X2.shadow",
        "wall1.IS_JOIN_X1_180.shadow",
        "wall1.IS_JOIN_X1.shadow",
        "wall1.IS_JOIN_X.shadow",
        "",
        // ##############################################################################
        "wall1.IS_JOIN_T270_3.shadow",
        "wall1.IS_JOIN_BLOCK.shadow",
        "wall1.IS_JOIN_T90_3.shadow",
        "wall1.IS_JOIN_VERT.shadow",
        "wall1.IS_JOIN_X4_90.shadow",
        "wall1.IS_JOIN_X4.shadow",
        "wall1.IS_JOIN_X2_180.shadow",
        "wall1.IS_JOIN_X2_90.shadow",
        "wall1.IS_JOIN_X1_90.shadow",
        "wall1.IS_JOIN_X1_270.shadow",
        "wall1.IS_JOIN_X3_180.shadow",
        "wall1.IS_JOIN_X3.shadow",
        // ##############################################################################
        "wall1.IS_JOIN_BL2.shadow",
        "wall1.IS_JOIN_T180_3.shadow",
        "wall1.IS_JOIN_BR2.shadow",
        "wall1.IS_JOIN_BOT.shadow",
        "wall1.IS_JOIN_T_1.shadow",
        "wall1.IS_JOIN_T_2.shadow",
        "wall1.IS_JOIN_T270_2.shadow",
        "wall1.IS_JOIN_T90_1.shadow",
        "wall1.IS_JOIN_T.shadow",
        "wall1.IS_JOIN_T90.shadow",
        "wall1.IS_JOIN_TL.shadow",
        "wall1.IS_JOIN_TR.shadow",
        // ##############################################################################
        "wall1.IS_JOIN_LEFT.shadow",
        "wall1.IS_JOIN_HORIZ.shadow",
        "wall1.IS_JOIN_RIGHT.shadow",
        "wall1.IS_JOIN_NODE.shadow",
        "wall1.IS_JOIN_T180_2.shadow",
        "wall1.IS_JOIN_T180_1.shadow",
        "wall1.IS_JOIN_T270_1.shadow",
        "wall1.IS_JOIN_T90_2.shadow",
        "wall1.IS_JOIN_T270.shadow",
        "wall1.IS_JOIN_T180.shadow",
        "wall1.IS_JOIN_BL.shadow",
        "wall1.IS_JOIN_BR.shadow",
        /* clang-format on */
    };
    tile_load_arr("data/gfx/tiles_32x32_walls1.tga", "tiles_32x32_walls1", 32, 32, ARRAY_SIZE(tiles), tiles);
  }

  {
    const char *tiles[] = {
        /* clang-format off */
        // ##############################################################################
        "wall2.IS_JOIN_TL2",
        "wall2.IS_JOIN_T_3",
        "wall2.IS_JOIN_TR2",
        "wall2.IS_JOIN_TOP",
        "wall2.IS_JOIN_X4_180",
        "wall2.IS_JOIN_X4_270",
        "wall2.IS_JOIN_X2_270",
        "wall2.IS_JOIN_X2",
        "wall2.IS_JOIN_X1_180",
        "wall2.IS_JOIN_X1",
        "wall2.IS_JOIN_X",
        "",
        // ##############################################################################
        "wall2.IS_JOIN_T270_3",
        "wall2.IS_JOIN_BLOCK",
        "wall2.IS_JOIN_T90_3",
        "wall2.IS_JOIN_VERT",
        "wall2.IS_JOIN_X4_90",
        "wall2.IS_JOIN_X4",
        "wall2.IS_JOIN_X2_180",
        "wall2.IS_JOIN_X2_90",
        "wall2.IS_JOIN_X1_90",
        "wall2.IS_JOIN_X1_270",
        "wall2.IS_JOIN_X3_180",
        "wall2.IS_JOIN_X3",
        // ##############################################################################
        "wall2.IS_JOIN_BL2",
        "wall2.IS_JOIN_T180_3",
        "wall2.IS_JOIN_BR2",
        "wall2.IS_JOIN_BOT",
        "wall2.IS_JOIN_T_1",
        "wall2.IS_JOIN_T_2",
        "wall2.IS_JOIN_T270_2",
        "wall2.IS_JOIN_T90_1",
        "wall2.IS_JOIN_T",
        "wall2.IS_JOIN_T90",
        "wall2.IS_JOIN_TL",
        "wall2.IS_JOIN_TR",
        // ##############################################################################
        "wall2.IS_JOIN_LEFT",
        "wall2.IS_JOIN_HORIZ",
        "wall2.IS_JOIN_RIGHT",
        "wall2.IS_JOIN_NODE",
        "wall2.IS_JOIN_T180_2",
        "wall2.IS_JOIN_T180_1",
        "wall2.IS_JOIN_T270_1",
        "wall2.IS_JOIN_T90_2",
        "wall2.IS_JOIN_T270",
        "wall2.IS_JOIN_T180",
        "wall2.IS_JOIN_BL",
        "wall2.IS_JOIN_BR",
        // ##############################################################################
        "wall2.IS_JOIN_TL2.shadow",
        "wall2.IS_JOIN_T_3.shadow",
        "wall2.IS_JOIN_TR2.shadow",
        "wall2.IS_JOIN_TOP.shadow",
        "wall2.IS_JOIN_X4_180.shadow",
        "wall2.IS_JOIN_X4_270.shadow",
        "wall2.IS_JOIN_X2_270.shadow",
        "wall2.IS_JOIN_X2.shadow",
        "wall2.IS_JOIN_X1_180.shadow",
        "wall2.IS_JOIN_X1.shadow",
        "wall2.IS_JOIN_X.shadow",
        "",
        // ##############################################################################
        "wall2.IS_JOIN_T270_3.shadow",
        "wall2.IS_JOIN_BLOCK.shadow",
        "wall2.IS_JOIN_T90_3.shadow",
        "wall2.IS_JOIN_VERT.shadow",
        "wall2.IS_JOIN_X4_90.shadow",
        "wall2.IS_JOIN_X4.shadow",
        "wall2.IS_JOIN_X2_180.shadow",
        "wall2.IS_JOIN_X2_90.shadow",
        "wall2.IS_JOIN_X1_90.shadow",
        "wall2.IS_JOIN_X1_270.shadow",
        "wall2.IS_JOIN_X3_180.shadow",
        "wall2.IS_JOIN_X3.shadow",
        // ##############################################################################
        "wall2.IS_JOIN_BL2.shadow",
        "wall2.IS_JOIN_T180_3.shadow",
        "wall2.IS_JOIN_BR2.shadow",
        "wall2.IS_JOIN_BOT.shadow",
        "wall2.IS_JOIN_T_1.shadow",
        "wall2.IS_JOIN_T_2.shadow",
        "wall2.IS_JOIN_T270_2.shadow",
        "wall2.IS_JOIN_T90_1.shadow",
        "wall2.IS_JOIN_T.shadow",
        "wall2.IS_JOIN_T90.shadow",
        "wall2.IS_JOIN_TL.shadow",
        "wall2.IS_JOIN_TR.shadow",
        // ##############################################################################
        "wall2.IS_JOIN_LEFT.shadow",
        "wall2.IS_JOIN_HORIZ.shadow",
        "wall2.IS_JOIN_RIGHT.shadow",
        "wall2.IS_JOIN_NODE.shadow",
        "wall2.IS_JOIN_T180_2.shadow",
        "wall2.IS_JOIN_T180_1.shadow",
        "wall2.IS_JOIN_T270_1.shadow",
        "wall2.IS_JOIN_T90_2.shadow",
        "wall2.IS_JOIN_T270.shadow",
        "wall2.IS_JOIN_T180.shadow",
        "wall2.IS_JOIN_BL.shadow",
        "wall2.IS_JOIN_BR.shadow",
        /* clang-format on */
    };
    tile_load_arr("data/gfx/tiles_32x32_walls2.tga", "tiles_32x32_walls2", 32, 32, ARRAY_SIZE(tiles), tiles);
  }
}
