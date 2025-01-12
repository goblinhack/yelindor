//
// Copyright Neil McGill, goblinhack@gmail.com
//

#include "my_ascii.hpp"
#include "my_callstack.hpp"
#include "my_color_defs.hpp"
#include "my_command.hpp"
#include "my_game.hpp"
#include "my_gl.hpp"
#include "my_sdl_event.hpp"
#include "my_sdl_proto.hpp"
#include "my_ui.hpp"
#include "my_wid_console.hpp"

SDL sdl;

void sdl_fini(void)
{
  LOG("SDL: Fini");
  TRACE_AND_INDENT();

#ifdef ENABLE_UI_ASCII_MOUSE
  SDL_ShowCursor(0);
  SDL_ShowCursor(1);
#endif

  if (sdl.init_video) {
    LOG("SDL: Video quit");
    sdl.init_video = 0;
    SDL_VideoQuit();
  }

  LOG("SDL: Delete GL context");
  SDL_GL_DeleteContext(sdl.context);

  LOG("SDL: Destroy window");
  SDL_DestroyWindow(sdl.window);

  LOG("SDL: Quit");
  SDL_Quit();

  LOG("SDL: Quit done");
}

static inline void sdl_list_video_size(void)
{
  TRACE_NO_INDENT();
  int i;

  for (i = 0; i < SDL_GetNumDisplayModes(0); ++i) {
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, i, &mode);
    DBG("+ SDL video            : %dx%d available, ratio %f", mode.w, mode.h, ((float) mode.w) / ((float) mode.h));
  }
}

void sdl_joy_rumble(float strength, uint32_t ms)
{
  TRACE_NO_INDENT();
  if (! sdl.haptic) {
    return;
  }

  SDL_HapticRumblePlay(sdl.haptic, strength, ms);
}

static void sdl_init_rumble(void)
{
  LOG("SDL: Init rumble:");
  TRACE_AND_INDENT();

  if (! sdl.haptic) {
    sdl.haptic = SDL_HapticOpenFromJoystick(sdl.joy);
    if (! sdl.haptic) {
      LOG("- Couldn't initialize SDL rumble: %s", SDL_GetError());
      SDL_ClearError();
      return;
    }
  }

  if (! SDL_HapticRumbleSupported(sdl.haptic)) {
    LOG("- No SDL rumble support: %s", SDL_GetError());
    SDL_ClearError();
    return;
  }

  if (SDL_HapticRumbleInit(sdl.haptic) != 0) {
    LOG("- SDL rumble nit failed: %s", SDL_GetError());
    SDL_ClearError();
    return;
  }

  LOG("- Opened Haptic for joy index %d", sdl.joy_index);
}

static void sdl_init_joystick(void)
{
  LOG("SDL: Init input:");
  TRACE_NO_INDENT();

  SDL_GameController *controller = nullptr;

  LOG("- Init game controleer");
  SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

  LOG("- Init haptic");
  SDL_InitSubSystem(SDL_INIT_HAPTIC);

  LOG("- Init joystick");
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);

  sdl.joy_index = 0;
  for (sdl.joy_index = 0; sdl.joy_index < SDL_NumJoysticks(); ++sdl.joy_index) {

    if (SDL_IsGameController(sdl.joy_index)) {
      controller = SDL_GameControllerOpen(sdl.joy_index);
      if (controller) {
        LOG("- Found gamecontroller");
        break;
      }
      WARN("Could not open gamecontroller %i: %s", sdl.joy_index, SDL_GetError());
      SDL_ClearError();
    }
  }

  if (! controller) {
    LOG("- No found gamecontroller");
    return;
  }

  sdl.joy = SDL_JoystickOpen(sdl.joy_index);
  if (sdl.joy) {
    LOG("- Opened Joystick  : %d", sdl.joy_index);
    LOG("- Name             : %s", SDL_JoystickNameForIndex(0));
    LOG("- Number of Axes   : %d", SDL_JoystickNumAxes(sdl.joy));
    LOG("- Number of Buttons: %d", SDL_JoystickNumButtons(sdl.joy));
    LOG("- Number of Balls  : %d", SDL_JoystickNumBalls(sdl.joy));

    sdl.joy_naxes    = SDL_JoystickNumAxes(sdl.joy);
    sdl.joy_nbuttons = SDL_JoystickNumButtons(sdl.joy);
    sdl.joy_balls    = SDL_JoystickNumBalls(sdl.joy);
  } else {
    WARN("Couldn't open Joystick 0");
  }
}

uint8_t sdl_init(void)
{
  gl_ext_init();

  int video_width;
  int video_height;
  int value;

  LOG("SDL: Version: %u.%u", SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
  TRACE_AND_INDENT();

  LOG("SDL: Init audio");
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    DIE("SDL_Init failed %s", SDL_GetError());
    return false;
  }

  LOG("SDL: Init video");
  if (SDL_VideoInit(nullptr) != 0) {
    DIE("SDL_VideoInit failed %s", SDL_GetError());
    return false;
  }

  //
  // Unused for now
  //
  if (0) {
    sdl_init_joystick();
    sdl_init_rumble();
  }

  sdl.init_video = 1;

  sdl_list_video_size();

  //
  // If we have a saved setting, use that.
  //
  if (game_config_pix_width_get(game) && game_config_pix_height_get(game)) {
    video_width  = game_config_pix_width_get(game);
    video_height = game_config_pix_height_get(game);
  } else {
    //
    // Else guess.
    //
    SDL_DisplayMode mode;
    memset(&mode, 0, SIZEOF(mode));

    LOG("SDL: Init display");
    if (SDL_GetCurrentDisplayMode(0, &mode) < 0) {
      DIE("SDL_GetCurrentDisplayMode couldn't set windowed display: %s", SDL_GetError());
      return false;
    }

    game_config_pix_width_set(game, mode.w);
    game_config_pix_height_set(game, mode.h);

    video_width  = game_config_pix_width_get(game);
    video_height = game_config_pix_height_get(game);
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  uint32_t video_is_unused_flags;

  LOG("SDL: Set SDL_WINDOW_OPENGL");
  video_is_unused_flags = SDL_WINDOW_OPENGL;

  if (game_gfx_borderless_get(game)) {
    LOG("SDL: Set SDL_WINDOW_BORDERLESS");
    video_is_unused_flags |= SDL_WINDOW_BORDERLESS;
  }

  if (game_gfx_fullscreen_get(game)) {
    LOG("SDL: Set SDL_WINDOW_FULLSCREEN");
    video_is_unused_flags |= SDL_WINDOW_FULLSCREEN;
  }

  if (game_gfx_fullscreen_desktop_get(game)) {
    LOG("SDL: Set SDL_WINDOW_FULLSCREEN_DESKTOP");
    video_is_unused_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  if (game_gfx_allow_highdpi_get(game)) {
    //
    // For a lo pixel game this makes no sense as the frame
    // buffers are really large and slows things down.
    //
    LOG("SDL: Calling SDL_GetDisplayDPI");
    float dpi;
    if (SDL_GetDisplayDPI(0, nullptr, &dpi, nullptr) == 0) {
      video_is_unused_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
      LOG("SDL: Set SDL_WINDOW_ALLOW_HIGHDPI");
    } else {
      ERR("SDL: Cannot enable high DPI: %s", SDL_GetError());
    }
  }

  LOG("SDL: Create window");
  sdl.window = SDL_CreateWindow("gorget", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, video_width, video_height,
                                video_is_unused_flags);
  if (! sdl.window) {
    ERR("SDL_CreateWindow couldn't set windowed display %ux%u: %s", video_width, video_height, SDL_GetError());
    game_config_reset(game);
    game_save_config(game);
    return false;
  }

  int w, h;
  if (video_is_unused_flags & SDL_WINDOW_ALLOW_HIGHDPI) {
    SDL_GL_GetDrawableSize(sdl.window, &w, &h);
    game_window_pix_width_set(game, w);
    game_window_pix_height_set(game, h);
  } else {
    SDL_GetWindowSize(sdl.window, &w, &h);
    game_window_pix_width_set(game, w);
    game_window_pix_height_set(game, h);
  }

  LOG("SDL: Call SDL_GL_CreateContext(%dx%d)", game_window_pix_width_get(game), game_window_pix_height_get(game));
  sdl.context = SDL_GL_CreateContext(sdl.window);
  if (! sdl.context) {
    SDL_ClearError();
    ERR("SDL_GL_CreateContext failed %s", SDL_GetError());
    return false;
  }

  LOG("SDL: Call SDL_GL_CreateContext(%dx%d) done", game_window_pix_width_get(game),
      game_window_pix_height_get(game));
  if (SDL_GL_MakeCurrent(sdl.window, sdl.context) < 0) {
    SDL_ClearError();
    ERR("SDL_GL_MakeCurrent failed %s", SDL_GetError());
    return false;
  }

  LOG("SDL: Call SDL_GL_MakeCurrent() done");

  SDL_ClearError();

  //
  // Clear the screen, both buffers
  //
  glcolor(WHITE);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow(sdl.window);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow(sdl.window);

  LOG("SDL: Call SDL_SetWindowTitle");
  SDL_SetWindowTitle(sdl.window, "gorget");

  LOG("SDL: OpenGL Vendor   : %s", glGetString(GL_VENDOR));
  LOG("SDL: OpenGL Renderer : %s", glGetString(GL_RENDERER));
  LOG("SDL: OpenGL Version  : %s", glGetString(GL_VERSION));

  IF_DEBUG { DBG("SDL: OpenGL Exts     : %s", glGetString(GL_EXTENSIONS)); }

  SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
  LOG("SDL: Red size    : %d", value);

  SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
  LOG("SDL: Green size  : %d", value);

  SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
  LOG("SDL: Blue size   : %d", value);

  SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
  LOG("SDL: Depth size  : %d", value);

  SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
  LOG("SDL: Double buf  : %d", value);

  SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &value);
  LOG("SDL: Hw Accel    : %d", value);

  LOG("SDL: Vsync       : %d", SDL_GL_GetSwapInterval());

  return true;
}

int sdl_get_mouse(void)
{
  TRACE_NO_INDENT();

  if (! wid_mouse_visible) {
    return 0;
  }

  int x = 0, y = 0;
  int button = SDL_GetMouseState(&x, &y);

  if (! x && ! y) {
    return button;
  }

  sdl.mouse_x = x;
  sdl.mouse_y = y;

  return button;
}

void sdl_mouse_center(void)
{
  TRACE_NO_INDENT();

  int x, y;

  x = game_window_pix_width_get(game) / 2;
  y = game_window_pix_height_get(game) / 2;

  sdl_mouse_warp(x, y);
}

void sdl_mouse_warp(int x, int y)
{
  TRACE_NO_INDENT();

  int border = 10;

  if (x <= 0) {
    x = border;
  } else if (x >= game_window_pix_width_get(game) - border) {
    x = game_window_pix_width_get(game) - border;
  }
  if (y <= 0) {
    y = border;
  } else if (y >= game_window_pix_height_get(game) - border) {
    y = game_window_pix_height_get(game) - border;
  }

  SDL_WarpMouseInWindow(sdl.window, x, y);

  sdl.mouse_x = x;
  sdl.mouse_y = y;
}

void sdl_tick(void)
{
  TRACE_NO_INDENT();
  sdl.left_fire = false;
  sdl.left_fire = true;

  sdl.joy1_right = false;
  sdl.joy1_left  = false;
  sdl.joy1_down  = false;
  sdl.joy1_up    = false;

  sdl.joy2_right = false;
  sdl.joy2_left  = false;
  sdl.joy2_down  = false;
  sdl.joy2_up    = false;

  sdl_get_mouse();

  //
  // Right stick
  //
  if (sdl.joy_axes[ 3 ] > sdl.joy_deadzone) {
    DBG2("SDL: right stick, right");
    sdl.joy1_right = true;

    sdl.joy_buttons[ SDL_JOY_BUTTON_RIGHT ]++;
    wid_joy_button(game, sdl.mouse_x, sdl.mouse_y);
    sdl.joy_buttons[ SDL_JOY_BUTTON_RIGHT ]--;
  }

  if (sdl.joy_axes[ 3 ] < -sdl.joy_deadzone) {
    DBG2("SDL: right stick, left");
    sdl.joy1_left = true;

    sdl.joy_buttons[ SDL_JOY_BUTTON_LEFT ]++;
    wid_joy_button(game, sdl.mouse_x, sdl.mouse_y);
    sdl.joy_buttons[ SDL_JOY_BUTTON_LEFT ]--;
  }

  if (sdl.joy_axes[ 4 ] > sdl.joy_deadzone) {
    DBG2("SDL: right stick, down");
    sdl.joy1_down = true;

    sdl.joy_buttons[ SDL_JOY_BUTTON_DOWN ]++;
    wid_joy_button(game, sdl.mouse_x, sdl.mouse_y);
    sdl.joy_buttons[ SDL_JOY_BUTTON_DOWN ]--;
  }

  if (sdl.joy_axes[ 4 ] < -sdl.joy_deadzone) {
    DBG2("SDL: right stick, up");
    sdl.joy1_up = true;

    sdl.joy_buttons[ SDL_JOY_BUTTON_UP ]++;
    wid_joy_button(game, sdl.mouse_x, sdl.mouse_y);
    sdl.joy_buttons[ SDL_JOY_BUTTON_UP ]--;
  }

  //
  // Left stick
  //
  int mx = 0;
  int my = 0;

  if (sdl.joy_axes[ 0 ] > sdl.joy_deadzone) {
    DBG2("SDL: left stick, right");
    sdl.joy2_right = true;
    mx             = 1;
  }

  if (sdl.joy_axes[ 0 ] < -sdl.joy_deadzone) {
    DBG2("SDL: left stick, left");
    sdl.joy2_left = true;
    mx            = -1;
  }

  if (sdl.joy_axes[ 1 ] > sdl.joy_deadzone) {
    DBG2("SDL: left stick, down");
    sdl.joy2_down = true;
    my            = 1;
  }

  if (sdl.joy_axes[ 1 ] < -sdl.joy_deadzone) {
    DBG2("SDL: left stick, up");
    sdl.joy2_up = true;
    my          = -1;
  }

  static double accel = 1.0;
  static ts_t   ts;

  if (time_have_x_tenths_passed_since(5, ts)) {
    accel = 1.0;
  }

  if ((mx != 0) || (my != 0)) {
    ts = time_ms();

    accel *= UI_SCROLL_JOY_SCALE;

    if (accel > UI_SCROLL_JOY_SCALE_MAX) {
      accel = UI_SCROLL_JOY_SCALE_MAX;
    }

    double x = sdl.mouse_x + ((double) mx * accel);
    double y = sdl.mouse_y + ((double) my * accel);

    if (x < 0) {
      x = 0;
    }

    if (y < 0) {
      y = 0;
    }

    if (x > game_window_pix_width_get(game) - 1) {
      x = game_window_pix_width_get(game) - 1;
    }

    if (y > game_window_pix_height_get(game) - 1) {
      y = game_window_pix_height_get(game) - 1;
    }

    if (wid_mouse_visible) {
      sdl_mouse_warp(x, y);
    }
  }
}

void sdl_exit(void)
{
  if (! g_main_loop_running) {
    return;
  }

  LOG("Finishing: SDL main loop is exiting...");
  TRACE_AND_INDENT();

#ifdef ENABLE_UI_ASCII_MOUSE
  SDL_ShowCursor(1);
#endif

  g_main_loop_running = false;
}

//
// User has entered a command, run it
//
uint8_t config_fps_counter_set(class Tokens *tokens, void *context)
{
  TRACE_NO_INDENT();

  char *s = tokens->args[ 2 ];

  if (! s || (*s == '\0')) {
    game_fps_counter_set(game, true);
    CON("FPS counter enabled (default).");
  } else {
    game_fps_counter_set(game, strtol(s, nullptr, 10) ? true : false);
    if (game_fps_counter_get(game)) {
      CON("FPS counter enabled.");
    } else {
      CON("FPS counter disabled.");
    }
  }

  return true;
}

//
// User wants to change the debug level
//
uint8_t config_debug_set(class Tokens *tokens, void *context)
{
  TRACE_NO_INDENT();

  char *s = tokens->args[ 2 ];

  if (! s || (*s == '\0')) {
    g_opt_debug1 = false;
    g_opt_debug2 = false;
    g_opt_debug3 = false;
  } else {
    g_opt_debug1 = false;
    g_opt_debug2 = false;
    g_opt_debug3 = false;
    switch (strtol(s, nullptr, 10) ? 1 : 0) {
      case 0 : break;
      default : g_opt_debug1 = true; break;
    }
  }

  if (g_opt_debug1) {
    CON("Debug: on.");
  } else {
    CON("Debug: off.");
  }

  return true;
}

//
// User has entered a command, run it
//
uint8_t config_gfx_vsync_enable(class Tokens *tokens, void *context)
{
  TRACE_NO_INDENT();

  char *s = tokens->args[ 2 ];

  if (! s || (*s == '\0')) {
    game_gfx_vsync_enable_set(game, true);
  } else {
    game_gfx_vsync_enable_set(game, strtol(s, nullptr, 10) ? 1 : 0);
  }

  if (game_gfx_vsync_enable_get(game)) {
    CON("SDL: Vsync enabled.");
    SDL_GL_SetSwapInterval(1);
  } else {
    CON("SDL: Vsync disabled.");
    SDL_GL_SetSwapInterval(0);
  }
  GL_ERROR_CHECK();

  return true;
}

void config_gfx_vsync_update(void)
{
  TRACE_NO_INDENT();

  if (game_gfx_vsync_enable_get(game)) {
    SDL_GL_SetSwapInterval(1);
  } else {
    SDL_GL_SetSwapInterval(0);
  }
  GL_ERROR_CHECK();
}

//
// User has entered a command, run it
//
uint8_t config_errored(class Tokens *tokens, void *context)
{
  TRACE_NO_INDENT();
  g_errored = false;
  CON("SDL: Errored mode cleared.");
  wid_hide(wid_console_window);
  sdl_display_reset();
  return true;
}

void sdl_config_update_all(void)
{
  TRACE_NO_INDENT();
  LOG("SDL: OpenGL leave 2D mode");
  config_game_gfx_update();
  config_gfx_vsync_update();
  LOG("SDL: OpenGL enter 2D mode");
  gl_init_2d_mode();
}

//
// User has entered a command, run it
//
uint8_t sdl_user_exit(class Tokens *tokens, void *context)
{
  TRACE_NO_INDENT();
  sdl_exit();

  return true;
}

void sdl_flush_display(class Game *g, bool force)
{
  TRACE_NO_INDENT();

  if (! force) {
    IF_NODEBUG { return; }
    if (g_opt_no_slow_log_flush) {
      return;
    }
  }

  glEnable(GL_TEXTURE_2D);
  gl_enter_2d_mode();
  wid_display_all();
  gl_leave_2d_mode();
  gl_enter_2d_mode(game_window_pix_width_get(game), game_window_pix_height_get(game));

  sdl_display(g);
}

void config_game_gfx_update(void)
{
  LOG("SDL: Update");
  TRACE_AND_INDENT();

  //
  // Display ratio
  //
  game_aspect_ratio_set(game, (double) game_window_pix_width_get(game) / (double) game_window_pix_height_get(game));

  TERM_WIDTH  = game_ui_gfx_term_width_get(game);
  TERM_HEIGHT = game_ui_gfx_term_height_get(game);

  if (! TERM_WIDTH) {
    ERR("TERM_WIDTH is zero");
    return;
  }
  if (! TERM_HEIGHT) {
    ERR("TERM_HEIGHT is zero");
    return;
  }

  if (TERM_WIDTH > TERM_WIDTH_MAX) {
    LOG("SDL: - %d exceeded console max width: %d", TERM_WIDTH, TERM_WIDTH_MAX);
    TERM_WIDTH = TERM_WIDTH_MAX;
  }

  if (TERM_HEIGHT > TERM_HEIGHT_MAX) {
    LOG("SDL: - %d exceeded console max height: %d", TERM_HEIGHT, TERM_HEIGHT_MAX);
    TERM_HEIGHT = TERM_HEIGHT_MAX;
  }

  float font_width  = game_window_pix_width_get(game) / TERM_WIDTH;
  float font_height = game_window_pix_height_get(game) / TERM_HEIGHT;

  game_ascii_gl_width_set(game, font_width);
  game_ascii_gl_height_set(game, font_height);

  //
  // If we overflow the screen, try to cut a few rows and columns off
  //
  while (game_ascii_gl_width_get(game) * TERM_WIDTH > game_window_pix_width_get(game)) {
    TERM_WIDTH--;
    LOG("SDL: - font width %u exceeded terminal pixel width %u, try term width: %d", game_ascii_gl_width_get(game),
        game_window_pix_width_get(game), TERM_WIDTH);
  }
  while (game_ascii_gl_height_get(game) * TERM_HEIGHT > game_window_pix_height_get(game)) {
    TERM_HEIGHT--;
    LOG("SDL: - font height %u exceeded terminal pixel height %u, try term height: %d",
        game_ascii_gl_height_get(game), game_window_pix_height_get(game), TERM_HEIGHT);
  }

  //
  // Work out the size of the game map
  //
  if (! TILE_WIDTH) {
    ERR("TILE_WIDTH zero");
    return;
  }

  if (! TILE_HEIGHT) {
    ERR("TILE_HEIGHT zero");
    return;
  }

  //
  // What size in terminal cells does the map take up?
  //
  double term_cells_w = TERM_WIDTH - (UI_LEFTBAR_WIDTH + UI_RIGHTBAR_WIDTH);
  double term_cells_h = TERM_HEIGHT - UI_TOPCON_HEIGHT - 2;
  double ratio        = (term_cells_w / ((double) UI_FONT_HEIGHT / (double) UI_FONT_WIDTH)) / term_cells_h;
  //
  // Now convert that to pixels
  //
  double visible_game_map_w = (double) TILE_WIDTH * MAP_TILES_ACROSS;
  double visible_game_map_h = (double) TILE_HEIGHT * (MAP_TILES_ACROSS / ratio);
  //
  // Compensate for any additional pixels the rounding of the terminal cells loses
  //
  double pixel_rounding_w_fixup
      = (game_ascii_gl_width_get(game) * TERM_WIDTH) / (double) game_window_pix_width_get(game);
  double pixel_rounding_h_fixup
      = (game_ascii_gl_height_get(game) * TERM_HEIGHT) / (double) game_window_pix_height_get(game);
  visible_game_map_w *= pixel_rounding_w_fixup;
  visible_game_map_h *= pixel_rounding_h_fixup;

  game_pix_width_set(game, visible_game_map_w);
  game_pix_height_set(game, visible_game_map_h);

  if (! game_pix_width_get(game)) {
    ERR("game->config.game_pix_width is zero");
    return;
  }
  if (! game_pix_height_get(game)) {
    ERR("game->config.game_pix_height is zero");
    return;
  }

  //
  // The map within the game fbo. Use the height of the screen so the width is pixel perfect.
  //
  game_map_pix_width_set(game, game_pix_width_get(game));
  game_map_pix_height_set(game, game_pix_height_get(game));

  float tiles_across = game_pix_width_get(game) / TILE_WIDTH;
  float tiles_down   = game_pix_height_get(game) / TILE_HEIGHT;

  game_tiles_visible_across_set(game, tiles_across);
  game_tiles_visible_down_set(game, tiles_down);

  LOG("SDL: Window:");
  LOG("SDL: - window pixel size         : %dx%d", game_window_pix_width_get(game), game_window_pix_height_get(game));
  LOG("SDL: - game map pixel size       : %dx%d", game_pix_width_get(game), game_pix_height_get(game));
  LOG("SDL: - visible map pixel size    : %dx%d", game_map_pix_width_get(game), game_map_pix_height_get(game));
  LOG("SDL: Terminal");
  LOG("SDL: - ascii gl size        : %ux%u", game_ascii_gl_width_get(game), game_ascii_gl_height_get(game));
  LOG("SDL: - term size            : %dx%d", TERM_WIDTH, TERM_HEIGHT);
  LOG("SDL: - aspect ratio         : %f", game_aspect_ratio_get(game));
  LOG("SDL: Map");
  LOG("SDL: - size                 : %dx%d", MAP_WIDTH, MAP_HEIGHT);
}
