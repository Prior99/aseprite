// SHE library
// Copyright (C) 2012-2015  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gfx/rect.h"
#include "gfx/size.h"
#include "she/she.h"

#include "she/common/system.h"

#ifdef _WIN32
  #include <windows.h>

  #include "she/win/window.h"
#else
  #error There is no Window implementation
#endif

// #include "SkApplication.h"
// #include "SkCanvas.h"
// #include "SkEvent.h"
// #include "SkGradientShader.h"
#include "SkBitmap.h"
// #include "SkGraphics.h"
#include "SkImageInfo.h"
// #include "SkGr.h"

#include <queue>

#include "she/skia/skia_surface.h"
#include "she/skia/skia_event_queue.h"
#include "she/skia/skia_window.h"
#include "she/skia/skia_display.h"
#include "she/skia/skia_font.h"
#include "she/skia/skia_system.h"

namespace she {

static System* g_instance;

System* create_system() {
  return g_instance = new SkiaSystem();
}

System* instance()
{
  return g_instance;
}

void error_message(const char* msg)
{
  // TODO
}

bool is_key_pressed(KeyScancode scancode)
{
  // TODO
  return false;
}

void clear_keyboard_buffer()
{
  // Do nothing
}

int clock_value()
{
  // TODO
  return 0; // clock_var;
}

Font* load_bitmap_font(const char* filename, int scale)
{
  // TODO
  return new SkiaFont(SkiaFont::DeleteThis);
}

} // namespace she

extern int app_main(int argc, char* argv[]);

#if _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow)
{
  int argc = 1;
  char* argv[] = { "" };
#else
int main(int argc, char* argv[])
{
#endif
  // SkGraphics::Init();
  // SkEvent::Init();

  int res = app_main(argc, argv);

  // SkEvent::Term();
  // SkGraphics::Term();

  return res;
}
