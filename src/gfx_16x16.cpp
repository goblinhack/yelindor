//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_tile.hpp"
#include "my_ui.hpp"

void gfx_init_16x16(void)
{
  std::initializer_list< std::string > tiles = {
      // clang-format off
      // ##############################################################################
      "dungeon_floor.1",
      "dungeon_floor.2",
      "dungeon_floor.3",
      "dungeon_floor.4",
  };
  const std::vector< std::string > arr(tiles);
  tile_load_arr("data/gfx/tiles_16x16.tga", "tiles_16x16", 16, 16, arr);
}