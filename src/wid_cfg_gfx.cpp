//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_ascii.hpp"
#include "my_callstack.hpp"
#include "my_game.hpp"
#include "my_main.hpp"
#include "my_sdl_event.hpp"
#include "my_sdl_proto.hpp"
#include "my_wids.hpp"

static WidPopup *wid_cfg_gfx_window;
static bool      local_g_need_restart = false;
static bool      config_changed;

static void wid_cfg_gfx_destroy(class Game *g)
{
  TRACE_AND_INDENT();
  delete wid_cfg_gfx_window;
  wid_cfg_gfx_window = nullptr;
  config_changed     = false;
}

static bool wid_cfg_gfx_cancel(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  CON("INF: Reload config");
  if (config_changed) {
    config_changed = false;
    game_load_config(game);
    sdl_config_update_all();
  }
  wid_cfg_gfx_destroy(game);
  wid_cfg_top_menu(game);
  return true;
}

static bool wid_cfg_gfx_save(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  CON("INF: Save config");
  game_save_config(game);

  wid_cfg_gfx_destroy(game);

  if (local_g_need_restart) {
    g_need_restart = true;
    wid_main_menu_destroy(game);
  } else {
    wid_cfg_top_menu(game);
  }

  return true;
}

static bool wid_cfg_gfx_back(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  wid_cfg_gfx_destroy(game);
  wid_cfg_top_menu(game);
  return true;
}

static bool wid_cfg_gfx_vsync_enable_toggle(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle vsync");
  game_gfx_vsync_enable_set(game, ! game_gfx_vsync_enable_get(game));
  config_gfx_vsync_update();
  wid_cfg_gfx_select(game);
  return true;
}

static bool wid_cfg_gfx_fullscreen_toggle(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle gfx fullscreen");
  game_gfx_fullscreen_set(game, ! game_gfx_fullscreen_get(game));

  if (game_gfx_fullscreen_get(game)) {
    game_gfx_fullscreen_desktop_set(game, false);
  }

  if (game_gfx_fullscreen_get(game)) {
    SDL_SetWindowFullscreen(sdl.window, SDL_WINDOW_FULLSCREEN);
  } else if (game_gfx_fullscreen_desktop_get(game)) {
    SDL_SetWindowFullscreen(sdl.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else {
    SDL_SetWindowFullscreen(sdl.window, 0);
  }

  //
  // These kind of changes seem to need a restart
  //
  local_g_need_restart = true;
  wid_cfg_gfx_save(nullptr, 0, 0, 0);
  wid_cfg_gfx_select(game);

  return true;
}

static bool wid_cfg_gfx_fullscreen_desktop_toggle(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle gfx fullscreen desktop");
  game_gfx_fullscreen_desktop_set(game, ! game_gfx_fullscreen_desktop_get(game));

  if (game_gfx_fullscreen_desktop_get(game)) {
    game_gfx_fullscreen_set(game, false);
  }

  if (game_gfx_fullscreen_get(game)) {
    SDL_SetWindowFullscreen(sdl.window, SDL_WINDOW_FULLSCREEN);
  } else if (game_gfx_fullscreen_desktop_get(game)) {
    SDL_SetWindowFullscreen(sdl.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    local_g_need_restart = true;
  } else {
    SDL_SetWindowFullscreen(sdl.window, 0);
  }

  //
  // These kind of changes seem to need a restart
  //
  local_g_need_restart = true;
  wid_cfg_gfx_save(nullptr, 0, 0, 0);
  wid_cfg_gfx_select(game);

  return true;
}

static bool wid_cfg_gfx_allow_highdpi_toggle(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle gfx allow highdpi");
  game_gfx_allow_highdpi_set(game, ! game_gfx_allow_highdpi_get(game));
  wid_cfg_gfx_select(game);
  local_g_need_restart = true;
  return true;
}

static bool wid_cfg_gfx_borderless_toggle(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle gfx borderless");
  game_gfx_borderless_set(game, ! game_gfx_borderless_get(game));
  SDL_SetWindowBordered(sdl.window, game_gfx_borderless_get(game) ? SDL_TRUE : SDL_FALSE);
  wid_cfg_gfx_select(game);
  return true;
}

static bool wid_cfg_other_fps_counter_toggle(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Toggle fps counter");
  game_fps_counter_set(game, ! game_fps_counter_get(game));
  wid_cfg_gfx_select(game);
  return true;
}

static bool wid_cfg_gfx_resolution_incr(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Increment resolution");
  auto res = std::to_string(game_window_pix_width_get(game)) + "x" + std::to_string(game_window_pix_height_get(game));
  auto n   = SDL_GetNumDisplayModes(0);
  int  chosen = 0;
  for (int i = 0; i < n; ++i) {
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, i, &mode);
    auto cand = std::to_string(mode.w) + "x" + std::to_string(mode.h);
    LOG(" - candidate: %s", cand.c_str());
    if (res == cand) {
      chosen = i - 1;
    }
  }
  for (int i = 0; i < n; ++i) {
    SDL_DisplayMode mode;
    if (i == chosen) {
      SDL_GetDisplayMode(0, i, &mode);
      auto cand = std::to_string(mode.w) + "x" + std::to_string(mode.h);
      LOG(" - chosen: %s", cand.c_str());
      game_window_pix_width_set(game, mode.w);
      game_window_pix_height_set(game, mode.h);
      game_config_pix_width_set(game, mode.w);
      game_config_pix_height_set(game, mode.h);
      SDL_SetWindowSize(sdl.window, mode.w, mode.h);
    }
  }
  wid_cfg_gfx_save(nullptr, 0, 0, 0);
  sdl_config_update_all();
  wid_cfg_gfx_select(game);
  return true;
}

static bool wid_cfg_gfx_resolution_decr(Widp w, int x, int y, uint32_t button)
{
  TRACE_AND_INDENT();
  config_changed = true;
  CON("INF: Decrement resolution");
  auto res = std::to_string(game_window_pix_width_get(game)) + "x" + std::to_string(game_window_pix_height_get(game));
  auto n   = SDL_GetNumDisplayModes(0);
  int  chosen = 0;
  for (int i = 0; i < n; ++i) {
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, i, &mode);
    auto cand = std::to_string(mode.w) + "x" + std::to_string(mode.h);
    LOG(" - candidate: %s", cand.c_str());
    if (res == cand) {
      chosen = i + 1;
    }
  }
  for (int i = 0; i < n; ++i) {
    SDL_DisplayMode mode;
    if (i == chosen) {
      SDL_GetDisplayMode(0, i, &mode);
      auto cand = std::to_string(mode.w) + "x" + std::to_string(mode.h);
      LOG(" - chosen: %s", cand.c_str());
      game_window_pix_width_set(game, mode.w);
      game_window_pix_height_set(game, mode.h);
      game_config_pix_width_set(game, mode.w);
      game_config_pix_height_set(game, mode.h);
      SDL_SetWindowSize(sdl.window, mode.w, mode.h);
    }
  }
  wid_cfg_gfx_save(nullptr, 0, 0, 0);
  sdl_config_update_all();
  wid_cfg_gfx_select(game);
  return true;
}

static bool wid_cfg_gfx_key_up(Widp w, const struct SDL_Keysym *key)
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
              case 'C' : wid_cfg_gfx_cancel(nullptr, 0, 0, 0); return true;
              case 's' :
              case 'S' : wid_cfg_gfx_save(nullptr, 0, 0, 0); return true;
              case 'b' :
              case 'B' :
              case SDLK_ESCAPE : wid_cfg_gfx_cancel(nullptr, 0, 0, 0); return true;
            }
          }
      }
  }

  return false;
}

static bool wid_cfg_gfx_key_down(Widp w, const struct SDL_Keysym *key)
{
  TRACE_AND_INDENT();

  if (sdlk_eq(*key, game_key_console_get(game))) {
    return false;
  }

  return true;
}

void wid_cfg_gfx_select(class Game *g)
{
  TRACE_AND_INDENT();
  if (wid_cfg_gfx_window) {
    wid_cfg_gfx_destroy(g);
  }

  auto box_style           = UI_WID_STYLE_HORIZ_DARK;
  auto box_highlight_style = UI_WID_STYLE_HORIZ_LIGHT;
  auto m                   = TERM_WIDTH / 2;

  point outer_tl(m - 23, TERM_HEIGHT / 2 - 11);
  point outer_br(m + 22, TERM_HEIGHT / 2 + 13);

  auto width = outer_br.x - outer_tl.x - 2;

  wid_cfg_gfx_window = new WidPopup("Gfx select", outer_tl, outer_br, nullptr, "", false, false);
  {
    TRACE_AND_INDENT();
    Widp w = wid_cfg_gfx_window->wid_popup_container;
    wid_set_on_key_up(w, wid_cfg_gfx_key_up);
    wid_set_on_key_down(w, wid_cfg_gfx_key_down);
  }

  int y_at = 0;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "configuration");

    point tl(1, y_at);
    point br(width, y_at + 2);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "Configuration");
  }

  y_at = 3;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Back");

    point tl(1, y_at);
    point br(6, y_at + 2);
    wid_set_style(w, UI_WID_STYLE_NORMAL);
    wid_set_on_mouse_up(w, wid_cfg_gfx_back);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "%%fg=white$B%%fg=reset$ack");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Save");

    point tl(width - 15, y_at);
    point br(width - 10, y_at + 2);
    wid_set_style(w, UI_WID_STYLE_GREEN);
    wid_set_on_mouse_up(w, wid_cfg_gfx_save);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "%%fg=white$S%%fg=reset$ave");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Cancel");

    point tl(width - 8, y_at);
    point br(width - 1, y_at + 2);
    wid_set_style(w, UI_WID_STYLE_RED);
    wid_set_on_mouse_up(w, wid_cfg_gfx_cancel);
    wid_set_pos(w, tl, br);
    wid_set_text(w, "%%fg=white$C%%fg=reset$ancel");
  }

  /////////////////////////////////////////////////////////////////////////
  // resolution
  /////////////////////////////////////////////////////////////////////////
  y_at += 4;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Resolution");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Resolution");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Resolution value");

    point tl(width / 2, y_at);
    point br(width / 2 + 12, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);

    auto res
        = std::to_string(game_window_pix_width_get(game)) + "x" + std::to_string(game_window_pix_height_get(game));
    wid_set_text(w, res);
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Resolution value +");

    point tl(width / 2 + 13, y_at);
    point br(width / 2 + 15, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_resolution_incr);
    wid_set_text(w, "+");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Resolution value -");

    point tl(width / 2 + 16, y_at);
    point br(width / 2 + 18, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_resolution_decr);
    wid_set_text(w, "-");
  }
  y_at++;

  /////////////////////////////////////////////////////////////////////////
  // Fullscreen
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Fullscreen");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Fullscreen(restart)");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Fullscreen value");

    point tl(29, y_at);
    point br(37, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_fullscreen_toggle);

    if (game_gfx_fullscreen_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // Fullscreen desktop
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Fullscreen desktop");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Full desktop(restart)");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Fullscreen value");

    point tl(29, y_at);
    point br(37, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_fullscreen_desktop_toggle);

    if (game_gfx_fullscreen_desktop_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // fullscreen desktop
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "High DPI res");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "High DPI resolution");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "High DPI value");

    point tl(29, y_at);
    point br(37, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_allow_highdpi_toggle);

    if (game_gfx_allow_highdpi_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // borderless
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Borderless");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Borderless window");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Borderless");

    point tl(29, y_at);
    point br(37, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_borderless_toggle);

    if (game_gfx_borderless_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // vsync
  /////////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Vertical sync");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "Vertical sync");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "Vertical sync value");

    point tl(29, y_at);
    point br(37, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_gfx_vsync_enable_toggle);

    if (game_gfx_vsync_enable_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  //////////////////////////////////////////////////////////////////////
  // fps
  //////////////////////////////////////////////////////////////////////
  y_at++;
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "FPS counter");

    point tl(1, y_at);
    point br(width / 2, y_at);
    wid_set_shape_none(w);
    wid_set_pos(w, tl, br);
    wid_set_text_lhs(w, true);
    wid_set_text(w, "FPS counter");
  }
  {
    TRACE_AND_INDENT();
    auto p = wid_cfg_gfx_window->wid_text_area->wid_text_area;
    auto w = wid_new_square_button(p, "FPS counter value");

    point tl(29, y_at);
    point br(37, y_at);
    wid_set_mode(w, WID_MODE_OVER);
    wid_set_style(w, box_highlight_style);
    wid_set_mode(w, WID_MODE_NORMAL);
    wid_set_style(w, box_style);
    wid_set_pos(w, tl, br);
    wid_set_on_mouse_up(w, wid_cfg_other_fps_counter_toggle);

    if (game_fps_counter_get(game)) {
      wid_set_text(w, "True");
    } else {
      wid_set_text(w, "False");
    }
  }

  wid_update(wid_cfg_gfx_window->wid_text_area->wid_text_area);
}
