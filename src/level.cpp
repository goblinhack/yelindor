//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_level.hpp"
#include "my_level_ph1.hpp"
#include "my_level_ph2.hpp"
#include "my_level_ph3.hpp"
#include "my_level_ph4.hpp"
#include "my_ptrcheck.hpp"

Level::Level()
{
  TRACE_NO_INDENT();

  data = level_data_constructor();

  world_create_and_place();
  assign_tiles();
  player_create_and_place();
  player_map_center();
}

Level::~Level()
{
  TRACE_NO_INDENT();

  auto d = data;
  if (d) {
    data = nullptr;
    level_data_destructor(d);
  }
}
