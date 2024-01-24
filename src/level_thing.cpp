//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include <iostream>
#include <sstream>
#include <string.h>

#include "my_level.hpp"
// REMOVED #include "my_level_data.hpp"
// REMOVED #include "my_main.hpp"
// REMOVED #include "my_thing.hpp"
// REMOVED #include "my_thing_id.hpp"
#include "my_tp.hpp"

Thingp thing_get(LevelData *data, point p, uint8_t slot, Tpp *out)
{
  TRACE_NO_INDENT();

  Id id = data->obj[ p.x ][ p.y ][ slot ].id;

  if (out) {
    *out = nullptr;
  }

  if (! id) {
    return nullptr;
  }

  if (id >= THING_ID_BASE) {
    auto t = thing_find(data, id);
    if (! t) {
      return nullptr;
    }

    if (out) {
      *out = tp_find(t->tp_id);
    }

    return t;
  }

  if (out) {
    *out = tp_find(id);
  }

  return nullptr;
}

Thingp Level::thing_get(point p, uint8_t slot, Tpp *out)
{
  TRACE_NO_INDENT();
  return ::thing_get(data, p, slot, out);
}

Thingp thing_find_optional(LevelData *data, ThingId id)
{
  TRACE_NO_INDENT();

  auto thing_id = id;
  auto x        = THING_ID_GET_X(thing_id);
  auto y        = THING_ID_GET_Y(thing_id);

  ASSERT_EX(x, <, (1 << THING_ID_X_BITS));
  ASSERT_EX(y, <, (1 << THING_ID_Y_BITS));

  auto t = &data->things[ x ][ y ];
  if (t->id == thing_id) {
    return t;
  }
  return nullptr;
}

Thingp Level::thing_find_optional(ThingId id)
{
  TRACE_NO_INDENT();
  return ::thing_find_optional(data, id);
}

Thingp thing_find(LevelData *data, ThingId id)
{
  TRACE_NO_INDENT();

  auto thing_id = id;
  auto x        = THING_ID_GET_X(thing_id);
  auto y        = THING_ID_GET_Y(thing_id);

  ASSERT_EX(x, <, (1 << THING_ID_X_BITS));
  ASSERT_EX(y, <, (1 << THING_ID_Y_BITS));

  auto t = &data->things[ x ][ y ];
  if (! t) {
    DIE("Thing not found for id, %" PRIX32 "", id);
  }

  if (t->id != id) {
    DIE("Thing found but entropy mismatch for id, %" PRIX32 "", id);
  }

  return t;
}

Thingp Level::thing_find(ThingId id)
{
  TRACE_NO_INDENT();
  return ::thing_find_optional(data, id);
}

Thingp thing_new(LevelData *data, Tpp tp, point p)
{
  TRACE_NO_INDENT();

  int16_t x = 0;
  int16_t y = 0;

  for (x = 0; x < (1 << THING_ID_X_BITS); x++) {
    for (y = 0; y < (1 << THING_ID_Y_BITS); y++) {
      auto t = &data->things[ x ][ y ];
      if (t->id) {
        continue;
      }

      static uint16_t entropy;
      entropy++;
      entropy &= (1 << THING_ID_ENTROPY_BITS) - 1;
      if (! entropy) {
        entropy++;
      }

      memset(t, 0, sizeof(*t));

      ThingId thing_id;
      thing_id = (entropy << (THING_ID_X_BITS + THING_ID_Y_BITS)) | (x << THING_ID_Y_BITS) | y;
      t->id    = thing_id;
      t->pix_x = (int) p.x * TILE_WIDTH;
      t->pix_y = (int) p.y * TILE_HEIGHT;

      if (tp) {
        t->tp_id = tp->id;
      }

      return t;
    }
  }

  DIE("out of things");
}

Thingp Level::thing_new(Tpp tp, point p)
{
  TRACE_NO_INDENT();
  return ::thing_new(data, tp, p);
}

void thing_free(LevelData *data, Thingp t)
{
  TRACE_NO_INDENT();

  auto o = ::thing_find(data, t->id);
  if (t != o) {
    DIE("Thing mismatch found for id, %" PRIX32 "", t->id);
  }

  memset(t, 0, sizeof(*t));
}

void Level::thing_free(Thingp t) { ::thing_free(data, t); }
