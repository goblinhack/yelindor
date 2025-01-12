//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_callstack.hpp"
#include "my_dmap.hpp"
#include "my_game.hpp"
#include "my_level.hpp"
#include "my_point.hpp"
#include "my_sdl_event.hpp"
#include "my_tp.hpp"

#include <string.h>
#include <vector>

static std::vector< point > cursor_path;

void level_cursor_set(Level *l, point p)
{
  TRACE_AND_INDENT();

  l->cursor_at = p;
}

//
// Create the cursor path, avoiding things like lava
//
// For the first pass, restrict to tiles we have walked on
// For the first pass, any tiles will do
//
static std::vector< point > level_cursor_path_draw_line_attempt(Levelp l, Thingp player, point start, point end,
                                                                int attempt)
{
  TRACE_AND_INDENT();

  static std::vector< point > empty;

  int z = player->at.z;

  Dmap  d {};
  point dmap_start = start;
  point dmap_end   = end;

  int minx, miny, maxx, maxy;
  if (dmap_start.x < dmap_end.x) {
    minx = dmap_start.x;
    maxx = dmap_end.x;
  } else {
    minx = dmap_end.x;
    maxx = dmap_start.x;
  }
  if (dmap_start.y < dmap_end.y) {
    miny = dmap_start.y;
    maxy = dmap_end.y;
  } else {
    miny = dmap_end.y;
    maxy = dmap_start.y;
  }

  auto border = game_tiles_visible_across_get(game);
  minx -= border;
  miny -= border;
  maxx += border;
  maxy += border;

  if (minx < 0) {
    minx = 0;
  }
  if (miny < 0) {
    miny = 0;
  }
  if (maxx >= MAP_WIDTH) {
    maxx = MAP_WIDTH - 1;
  }
  if (maxy >= MAP_HEIGHT) {
    maxy = MAP_HEIGHT - 1;
  }

  //
  // If standing on a hazard, then plot a course that allows travel over hazards.
  //
  if (level_is_cursor_path_hazard_at(l, player->at.x, player->at.y, z)) {
    //
    // Just map the shortest path outta here
    //
    for (auto y = miny; y < maxy; y++) {
      for (auto x = minx; x < maxx; x++) {
        if (level_is_cursor_path_blocker_at(l, x, y, z)) {
          d.val[ x ][ y ] = DMAP_IS_WALL;
        } else {
          d.val[ x ][ y ] = DMAP_IS_PASSABLE;
        }
      }
    }
  } else if (level_is_cursor_path_hazard_at(l, l->cursor_at.x, l->cursor_at.y, z)) {
    bool                               got_one = false;
    std::initializer_list< ThingFlag > init    = {is_lava, is_chasm};

    for (auto i : init) {
      if (level_flag(l, i, point3d(l->cursor_at.x, l->cursor_at.y, z))) {
        got_one = true;

        //
        // If the cursor is on a hazard then allow creating a path via hazards.
        //
        // However, be careful. If we click on lava, and the shortest path
        // to that lava is via a chasm, then we do not want to jump into the
        // chasm.
        //
        for (auto y = miny; y < maxy; y++) {
          for (auto x = minx; x < maxx; x++) {
            point3d p(x, y, z);

            if (level_is_cursor_path_hazard(l, p)) {
              if (! level_flag(l, i, p)) {
                d.val[ x ][ y ] = DMAP_IS_WALL;
                continue;
              }
              d.val[ x ][ y ] = DMAP_IS_PASSABLE;
              continue;
            }
            d.val[ x ][ y ] = DMAP_IS_PASSABLE;
          }
        }
      }
    }

    if (! got_one) {
      //
      // Plough through all hazards? This is probably not good. Let's play safe.
      //
      for (auto y = miny; y < maxy; y++) {
        for (auto x = minx; x < maxx; x++) {
          point3d p(x, y, z);

          if (level_is_cursor_path_blocker(l, p) || level_is_cursor_path_hazard(l, p)) {
            d.val[ x ][ y ] = DMAP_IS_WALL;
          } else {
            d.val[ x ][ y ] = DMAP_IS_PASSABLE;
          }
        }
      }
    }
  } else {
    //
    // Normal path. Avoid hazards.
    //
    for (auto y = miny; y < maxy; y++) {
      for (auto x = minx; x < maxx; x++) {
        point3d p(x, y, z);

        if (level_is_cursor_path_blocker(l, p) || level_is_cursor_path_hazard(l, p)) {
          d.val[ x ][ y ] = DMAP_IS_WALL;
        } else {
          d.val[ x ][ y ] = DMAP_IS_PASSABLE;
        }
      }
    }
  }

  //
  // Limit to previously walked tiles
  //
  if (attempt == 1) {
    for (auto y = miny; y < maxy; y++) {
      for (auto x = minx; x < maxx; x++) {
        point3d p(x, y, z);

        if (! l->is_walked[ x ][ y ][ z ]) {
          d.val[ x ][ y ] = DMAP_IS_WALL;
        }

        //
        // Probably best to not use tiles where there is a monster
        //
        if (level_is_monst(l, p)) {
          d.val[ x ][ y ] = DMAP_IS_WALL;
        }
      }
    }
  }

  dmap_start = point(minx, miny);
  dmap_end   = point(maxx, maxy);

  d.val[ end.x ][ end.y ]     = DMAP_IS_GOAL;
  d.val[ start.x ][ start.y ] = DMAP_IS_PASSABLE;

  // dmap_print(&d, start, dmap_start, dmap_end);
  dmap_process_allow_diagonals(&d, dmap_start, dmap_end, true);
  // dmap_print(&d, start, dmap_start, dmap_end);

  auto p         = dmap_solve(&d, start);
  auto path_size = p.size();
  if (! path_size) {
    return empty;
  }

  //
  // If we could not reach the target, then the path will just plot the distance from the start, which is not what
  // we want.
  //
  if (p[ path_size - 1 ] != end) {
    return empty;
  }

  return p;
}

//
// Create a cursor path between the two points
//
// Returns true on success
//
static std::vector< point > level_cursor_path_draw_line(Levelp l, point start, point end)
{
  static std::vector< point > empty;

  auto player = thing_player(l);
  if (! player) {
    return empty;
  }

  //
  // Choose the shortest path of each attempt.
  //
  // The first path prefers visited tiles.
  //
  auto attempt1 = level_cursor_path_draw_line_attempt(l, player, start, end, 1);
  auto attempt2 = level_cursor_path_draw_line_attempt(l, player, start, end, 2);

  std::vector< point > best;

  best = attempt1;

  if (! best.size()) {
    best = attempt2;
  } else if (attempt2.size() && (attempt2.size() < best.size())) {
    best = attempt2;
  }

  return best;
}

//
// Stop following the current path
//
void level_cursor_path_reset(Levelp l)
{
  auto t = thing_player(l);
  if (! t) {
    //
    // If no player, clear the cursor
    //
    memset(l->cursor, 0, SIZEOF(l->cursor));
    return;
  }

  auto aip = thing_ai(l, t);
  if (! aip) {
    //
    // If no player, clear the cursor
    //
    memset(l->cursor, 0, SIZEOF(l->cursor));
    return;
  }

  l->request_follow_path = false;
  l->is_following_a_path = false;
  aip->move_path.size    = 0;
}

//
// Copy the mouse path to the thing
//
static void level_cursor_path_apply(Levelp l, std::vector< point > &move_path)
{
  auto t = thing_player(l);
  if (! t) {
    //
    // If no player, clear the cursor
    //
    memset(l->cursor, 0, SIZEOF(l->cursor));
    return;
  }

  auto aip = thing_ai(l, t);
  if (! aip) {
    //
    // If no player, clear the cursor
    //
    memset(l->cursor, 0, SIZEOF(l->cursor));
    return;
  }

  if (l->request_follow_path) {
    //
    // Player wants to start following or replace the current path.
    //
    l->request_follow_path = false;
    l->is_following_a_path = true;
  } else if (l->is_following_a_path) {
    //
    // Already following a path, stick to it until completion.
    //
    return;
  }

  //
  // Copy the latest mouse path to the player
  //
  int index           = 0;
  aip->move_path.size = 0;

  for (auto p : move_path) {
    aip->move_path.points[ index ].x = p.x;
    aip->move_path.points[ index ].y = p.y;
    aip->move_path.size              = ++index;
    if (index >= ARRAY_SIZE(aip->move_path.points)) {
      break;
    }
  }
}

//
// Recreate the mouse path
//
static void level_cursor_path_create(Levelp l)
{
  auto player = thing_player(l);
  if (! player) {
    //
    // If no player, clear the cursor
    //
    memset(l->cursor, 0, SIZEOF(l->cursor));
    return;
  }

  //
  // Has something changed?
  //
  if ((player->at == player->old_at) && (l->cursor_at == l->old_cursor_at)) {
    return;
  }

  //
  // Clear the path.
  //
  memset(l->cursor, 0, SIZEOF(l->cursor));

  //
  // Draw the path
  //
  cursor_path = level_cursor_path_draw_line(l, make_point(player->at), l->cursor_at);
  for (auto p : cursor_path) {
    l->cursor[ p.x ][ p.y ] = CURSOR_PATH;
  }
  l->cursor[ l->cursor_at.x ][ l->cursor_at.y ] = CURSOR_AT;

  l->old_cursor_at = l->cursor_at;
}

//
// Recreate and possibly apply the mouse path to the player
//
void level_cursor_update(Levelp l)
{
  //
  // Recreate the mouse path
  //
  level_cursor_path_create(l);

  //
  // Update the player with the path.
  //
  level_cursor_path_apply(l, cursor_path);
}
