//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_ascii.hpp"
#include "my_callstack.hpp"
#include "my_game.hpp"
#include "my_main.hpp"
#include "my_sdl_proto.hpp"
#include "my_wids.hpp"

static WidPopup *wid_cfg_mouse_window;
static bool      local_g_need_restart = false;
static bool      config_changed;

static void wid_cfg_mouse_destroy(class Game *g)
{
  TRACE_AND_INDENT();
  delete wid_cfg_mouse_window;
  wid_cfg_mouse_window = nullptr;
  config_changed       = false;
}

static bool wid_cfg_mouse_cancel(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  CON("INF: Reload config");
  if (config_changed) {
    config_changed = false;
    game_load_config(game);
    sdl_config_update_all();
  }
  wid_cfg_mouse_destroy(game);
  wid_cfg_top_menu(game);
  return true;
}

static bool wid_cfg_mouse_save(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  CON("INF: Save config");
  game_save_config(game);
  wid_cfg_mouse_destroy(game);
  wid_cfg_top_menu(game);
  if (local_g_need_restart) {
    g_need_restart = true;
  }
  return true;
}

static bool wid_cfg_mouse_back(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  wid_cfg_mouse_destroy(game);
  wid_cfg_top_menu(game);
  return true;
}

static bool wid_cfg_mouse_wheel_lr_negated(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle wheel mouse lr");
  game_mouse_wheel_lr_negated_set(game, ! game_mouse_wheel_lr_negated_get(game));
  wid_cfg_mouse_select(game);
  return true;
}

static bool wid_cfg_mouse_wheel_ud_negated(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle wheel mouse ud");
  game_mouse_wheel_ud_negated_set(game, ! game_mouse_wheel_ud_negated_get(game));
  wid_cfg_mouse_select(game);
  return true;
}

static bool wid_cfg_mouse_key_up(Widp w, const struct SDL_Keysym *key)
{
  TRACE_AND_INDENT();

  if (sdlk_eq(*key, game_key_console_get(game))) {
    return false;
  }

  switch (key->mod) {
    case KMOD_LCTRL :
    case KMOD_RCTRL :
    default :
      switch (key->sym) {
        default :
          {
            TRACE_AND_INDENT();
            auto c = wid_event_to_char(key);
            switch (c) {
              case 'c' :
              case 'C' : wid_cfg_mouse_cancel(nullptr, 0, 0, 0); return true;
              case 's' :
              case 'S' : wid_cfg_mouse_save(nullptr, 0, 0, 0); return true;
              case 'b' :
              case 'B' :
              case SDLK_ESCAPE : wid_cfg_mouse_cancel(nullptr, 0, 0, 0); return true;
            }
          }
      }
  }

  return false;
}

static bool wid_cfg_mouse_key_down(Widp w, const struct SDL_Keysym *key)
{
  TRACE_AND_INDENT();

  if (sdlk_eq(*key, game_key_console_get(game))) {
    return false;
  }

  return true;
}

void wid_cfg_mouse_select(class Game *g)
{
  TRACE_AND_INDENT();
  if (wid_cfg_mouse_window) {
    wid_cfg_mouse_destroy(g);
  }

  auto box_style           = UI_WID_STYLE_HORIZ_DARK;
  auto box_highlight_style = UI_WID_STYLE_HORIZ_LIGHT;

  int   menu_height = 20;
  int   menu_width  = UI_WID_POPUP_WIDTH_NORMAL * 2;
  point outer_tl(TERM_WIDTH / 2 - (menu_width / 2), TERM_HEIGHT / 2 - (menu_height / 2));
  point outer_br(TERM_WIDTH / 2 + (menu_width / 2), TERM_HEIGHT / 2 + (menu_height / 2));
  wid_cfg_mouse_window = new WidPopup("Mouse", outer_tl, outer_br, nullptr, "", false, false);

  auto button_width = outer_br.x - outer_tl.x - 2;

  {
    TRACE_AND_INDENT();
    Widp w = wid_cfg_mouse_window->wid_popup_container;
    wid_set_on_key_up(w, wid_cfg_mouse_key_up);
    wid_set_on_key_down(w, wid_cfg_mouse_key_down);
  }

  int y_at = 0;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "configuration");

    point tl(1, y_at);
    point br(button_width, y_at + 2);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "Configuration");
  }

  y_at = 3;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Back");

    point tl(1, y_at);
    point br(6, y_at + 2);
    wid_set_style(w, UI_WID_STYLE_NORMAL);
    wid_set_on_mouse_up(w, wid_cfg_mouse_back);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "%%fg=white$B%%fg=reset$ack");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Save");

    point tl(button_width - 15, y_at);
    point br(button_width - 10, y_at + 2);
    wid_set_style(w, UI_WID_STYLE_GREEN);
    wid_set_on_mouse_up(w, wid_cfg_mouse_save);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "%%fg=white$S%%fg=reset$ave");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Cancel");

    point tl(button_width - 8, y_at);
    point br(button_width - 1, y_at + 2);
    wid_set_style(w, UI_WID_STYLE_RED);
    wid_set_on_mouse_up(w, wid_cfg_mouse_cancel);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "%%fg=white$C%%fg=reset$ancel");
  }

  y_at = 6;

  /////////////////////////////////////////////////////////////////////////
  // Inverted mouse scrolling
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Mouse scroll lr invert lr");

    point tl(1, y_at);
    point br(button_width, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Mouse invert left/right");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Mouse scroll lr invert");

    point tl(28, y_at);
    point br(34, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_mouse_wheel_lr_negated);

    if (game_mouse_wheel_lr_negated_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // Inverted mouse scrolling
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Mouse scroll ud invert");

    point tl(1, y_at);
    point br(button_width, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Mouse invert up/down");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_mouse_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Mouse scroll ud invert value");

    point tl(28, y_at);
    point br(34, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_mouse_wheel_ud_negated);

    if (game_mouse_wheel_ud_negated_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  wid_update(wid_cfg_mouse_window->wid_text_area->wid_text_area);
}
