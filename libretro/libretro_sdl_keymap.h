// SDL to RETROK key mapping
// Excerpted from RetroArch's input/input_keymaps.c
// https://github.com/libretro/RetroArch/blob/b4143882245edd737c7e7c522b25e32f8d1f64ad/input/input_keymaps.c#L607
// 9ca5c5e 2023-07-08

#pragma once

struct rarch_key_map
{
   unsigned sym;
   enum retro_key rk;
};

const struct rarch_key_map rarch_key_map_sdl[] = {
   { SDLK_BACKSPACE, RETROK_BACKSPACE },
   { SDLK_TAB, RETROK_TAB },
   { SDLK_CLEAR, RETROK_CLEAR },
   { SDLK_RETURN, RETROK_RETURN },
   { SDLK_PAUSE, RETROK_PAUSE },
   { SDLK_ESCAPE, RETROK_ESCAPE },
   { SDLK_SPACE, RETROK_SPACE },
   { SDLK_EXCLAIM, RETROK_EXCLAIM },
   { SDLK_QUOTEDBL, RETROK_QUOTEDBL },
   { SDLK_HASH, RETROK_HASH },
   { SDLK_DOLLAR, RETROK_DOLLAR },
   { SDLK_AMPERSAND, RETROK_AMPERSAND },
   { SDLK_QUOTE, RETROK_QUOTE },
   { SDLK_LEFTPAREN, RETROK_LEFTPAREN },
   { SDLK_RIGHTPAREN, RETROK_RIGHTPAREN },
   { SDLK_ASTERISK, RETROK_ASTERISK },
   { SDLK_PLUS, RETROK_PLUS },
   { SDLK_COMMA, RETROK_COMMA },
   { SDLK_MINUS, RETROK_MINUS },
   { SDLK_PERIOD, RETROK_PERIOD },
   { SDLK_SLASH, RETROK_SLASH },
   { SDLK_0, RETROK_0 },
   { SDLK_1, RETROK_1 },
   { SDLK_2, RETROK_2 },
   { SDLK_3, RETROK_3 },
   { SDLK_4, RETROK_4 },
   { SDLK_5, RETROK_5 },
   { SDLK_6, RETROK_6 },
   { SDLK_7, RETROK_7 },
   { SDLK_8, RETROK_8 },
   { SDLK_9, RETROK_9 },
   { SDLK_COLON, RETROK_COLON },
   { SDLK_SEMICOLON, RETROK_SEMICOLON },
   { SDLK_LESS, RETROK_OEM_102 },
   { SDLK_EQUALS, RETROK_EQUALS },
   { SDLK_GREATER, RETROK_GREATER },
   { SDLK_QUESTION, RETROK_QUESTION },
   { SDLK_AT, RETROK_AT },
   { SDLK_LEFTBRACKET, RETROK_LEFTBRACKET },
   { SDLK_BACKSLASH, RETROK_BACKSLASH },
   { SDLK_RIGHTBRACKET, RETROK_RIGHTBRACKET },
   { SDLK_CARET, RETROK_CARET },
   { SDLK_UNDERSCORE, RETROK_UNDERSCORE },
   { SDLK_BACKQUOTE, RETROK_BACKQUOTE },
   { SDLK_a, RETROK_a },
   { SDLK_b, RETROK_b },
   { SDLK_c, RETROK_c },
   { SDLK_d, RETROK_d },
   { SDLK_e, RETROK_e },
   { SDLK_f, RETROK_f },
   { SDLK_g, RETROK_g },
   { SDLK_h, RETROK_h },
   { SDLK_i, RETROK_i },
   { SDLK_j, RETROK_j },
   { SDLK_k, RETROK_k },
   { SDLK_l, RETROK_l },
   { SDLK_m, RETROK_m },
   { SDLK_n, RETROK_n },
   { SDLK_o, RETROK_o },
   { SDLK_p, RETROK_p },
   { SDLK_q, RETROK_q },
   { SDLK_r, RETROK_r },
   { SDLK_s, RETROK_s },
   { SDLK_t, RETROK_t },
   { SDLK_u, RETROK_u },
   { SDLK_v, RETROK_v },
   { SDLK_w, RETROK_w },
   { SDLK_x, RETROK_x },
   { SDLK_y, RETROK_y },
   { SDLK_z, RETROK_z },
   { SDLK_DELETE, RETROK_DELETE },
#ifdef HAVE_SDL2
   { SDLK_KP_0, RETROK_KP0 },
   { SDLK_KP_1, RETROK_KP1 },
   { SDLK_KP_2, RETROK_KP2 },
   { SDLK_KP_3, RETROK_KP3 },
   { SDLK_KP_4, RETROK_KP4 },
   { SDLK_KP_5, RETROK_KP5 },
   { SDLK_KP_6, RETROK_KP6 },
   { SDLK_KP_7, RETROK_KP7 },
   { SDLK_KP_8, RETROK_KP8 },
   { SDLK_KP_9, RETROK_KP9 },
#else
   { SDLK_KP0, RETROK_KP0 },
   { SDLK_KP1, RETROK_KP1 },
   { SDLK_KP2, RETROK_KP2 },
   { SDLK_KP3, RETROK_KP3 },
   { SDLK_KP4, RETROK_KP4 },
   { SDLK_KP5, RETROK_KP5 },
   { SDLK_KP6, RETROK_KP6 },
   { SDLK_KP7, RETROK_KP7 },
   { SDLK_KP8, RETROK_KP8 },
   { SDLK_KP9, RETROK_KP9 },
#endif
   { SDLK_KP_PERIOD, RETROK_KP_PERIOD },
   { SDLK_KP_DIVIDE, RETROK_KP_DIVIDE },
   { SDLK_KP_MULTIPLY, RETROK_KP_MULTIPLY },
   { SDLK_KP_MINUS, RETROK_KP_MINUS },
   { SDLK_KP_PLUS, RETROK_KP_PLUS },
   { SDLK_KP_ENTER, RETROK_KP_ENTER },
   { SDLK_KP_EQUALS, RETROK_KP_EQUALS },
   { SDLK_UP, RETROK_UP },
   { SDLK_DOWN, RETROK_DOWN },
   { SDLK_RIGHT, RETROK_RIGHT },
   { SDLK_LEFT, RETROK_LEFT },
   { SDLK_INSERT, RETROK_INSERT },
   { SDLK_HOME, RETROK_HOME },
   { SDLK_END, RETROK_END },
   { SDLK_PAGEUP, RETROK_PAGEUP },
   { SDLK_PAGEDOWN, RETROK_PAGEDOWN },
   { SDLK_F1, RETROK_F1 },
   { SDLK_F2, RETROK_F2 },
   { SDLK_F3, RETROK_F3 },
   { SDLK_F4, RETROK_F4 },
   { SDLK_F5, RETROK_F5 },
   { SDLK_F6, RETROK_F6 },
   { SDLK_F7, RETROK_F7 },
   { SDLK_F8, RETROK_F8 },
   { SDLK_F9, RETROK_F9 },
   { SDLK_F10, RETROK_F10 },
   { SDLK_F11, RETROK_F11 },
   { SDLK_F12, RETROK_F12 },
   { SDLK_F13, RETROK_F13 },
   { SDLK_F14, RETROK_F14 },
   { SDLK_F15, RETROK_F15 },
#ifdef HAVE_SDL2
   { SDLK_NUMLOCKCLEAR, RETROK_NUMLOCK },
#else
   { SDLK_NUMLOCK, RETROK_NUMLOCK },
#endif
   { SDLK_CAPSLOCK, RETROK_CAPSLOCK },
#ifdef HAVE_SDL2
   { SDLK_SCROLLLOCK, RETROK_SCROLLOCK },
#else
   { SDLK_SCROLLOCK, RETROK_SCROLLOCK },
#endif
   { SDLK_RSHIFT, RETROK_RSHIFT },
   { SDLK_LSHIFT, RETROK_LSHIFT },
   { SDLK_RCTRL, RETROK_RCTRL },
   { SDLK_LCTRL, RETROK_LCTRL },
   { SDLK_RALT, RETROK_RALT },
   { SDLK_LALT, RETROK_LALT },
#ifdef HAVE_SDL2
   /* { ?, RETROK_RMETA }, */
   /* { ?, RETROK_LMETA }, */
   { SDLK_LGUI, RETROK_LSUPER },
   { SDLK_RGUI, RETROK_RSUPER },
#else
   { SDLK_RMETA, RETROK_RMETA },
   { SDLK_LMETA, RETROK_LMETA },
   { SDLK_LSUPER, RETROK_LSUPER },
   { SDLK_RSUPER, RETROK_RSUPER },
#endif
   { SDLK_MODE, RETROK_MODE },
#ifndef HAVE_SDL2
   { SDLK_COMPOSE, RETROK_COMPOSE },
#endif
   { SDLK_HELP, RETROK_HELP },
#ifdef HAVE_SDL2
   { SDLK_PRINTSCREEN, RETROK_PRINT },
#else
   { SDLK_PRINT, RETROK_PRINT },
#endif
   { SDLK_SYSREQ, RETROK_SYSREQ },
   { SDLK_PAUSE, RETROK_BREAK },
   { SDLK_MENU, RETROK_MENU },
   { SDLK_POWER, RETROK_POWER },

#ifndef HAVE_SDL2
   { SDLK_EURO, RETROK_EURO },
#endif
   { SDLK_UNDO, RETROK_UNDO },

#ifdef __LIBRETRO__
   { SDLK_LESS, RETROK_LESS }, // non-USA ST keyboards have a <> key next to left-shift
#endif

   { 0, RETROK_UNKNOWN },
};
