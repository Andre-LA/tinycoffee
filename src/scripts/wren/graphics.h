#include "module.h"

static const char *tcWrenGraphicsModule =
"class Render {\n"
" foreign static clear()\n"
" foreign static clear(color)\n"
" foreign static fillRectangle(x, y, w, h)\n"
" foreign static fillRectangle(x, y, w, h, color)\n"
" foreign static drawRectangle(x, y, w, h, color)\n"
" foreign static fillCircle(x, y, radius, color)\n"
" foreign static drawCircle(x, y, radius, color)\n"
"}\n"
"foreign class Font {\n"
" construct load(filename) {}\n"
" construct load(filename, size) {}\n"
" foreign static print(text, x, y, color)\n"
" foreign static print(text, x, y, sx, sy, color)\n"
" foreign print(text, x, y, color)\n"
" foreign print(text, x, y, sx, sy, color)\n"
"}\n"
"foreign class ColorF {\n"
" construct new(r,g,b) {}\n"
" construct new(r,g,b,a) {}\n"
" foreign r\n"
" foreign g\n"
" foreign b\n"
" foreign a\n"
"}\n"
"class Color {\n"
" static load() {\n"
"  __white = ColorF.new(255, 255, 255)\n"
"  __black = ColorF.new(0, 0, 0)\n"
"  __red = ColorF.new(255, 0, 0)\n"
" __bg = ColorF.new(75, 90, 90)\n"
"}\n"
" static new(r,g,b) { ColorF.new(r,g,b) }\n"
" static new(r,g,b,a) { ColorF.new(r,g,b,a)}\n"
" static White { __white }\n"
" static Black { __black }\n"
" static Red { __red }\n"
" static BG { __bg }\n"
"}\n"
"Color.load()\n"
"foreign class Image {\n"
" construct create(data, width, height) {}\n"
" construct load(filename) {}\n"
" foreign width\n"
" foreign height\n"
" foreign draw(x, y, color)\n"
" foreign draw(x, y, sx, sy, color)\n"
" foreign draw(x, y, angle, sx, sy, cx, cy, color)\n"
" foreign drawPart(rect, x, y, color)\n"
" foreign drawPart(rect, x, y, sx, sy, color)\n"
" foreign drawPart(rect, x, y, angle, sx, sy, cx, cy, color)\n"
"}\n"
"foreign class Canvas {\n"
" construct new(width, height) {}\n"
" foreign width\n"
" foreign height\n"
" foreign set()\n"
" foreign unset()\n"
" foreign draw(x, y, color)\n"
" foreign draw(x, y, sx, sy, color)\n"
"}\n";
// "foreign class Shader {\n"
// " construct create(vertexSource, fragmentSource) {}\n"
// " construct load(vertexSource, fragmentSource) {}\n"
// " foreign set()\n"
// " foreign unset()\n"
// "]\n";

/* Graphics module */

static void wren_graphics_clear(WrenVM* vm) {
  tc_Color *color = &BLACK;
  if (wrenGetSlotCount(vm) > 1) color = wrenGetSlotForeign(vm, 1);

  tc_clear(*color);
}

static void wren_fill_rectangle(WrenVM *vm) {
  wrenInitSlot(0);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float width = wrenGetSlotDouble(vm, nextSlot());
  float height = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);
  // wrenEnsureSlots(vm, 0);

  tc_fill_rectangle(x, y, width, height, *color);
}

static void wren_draw_rectangle(WrenVM *vm) {
  wrenInitSlot(0);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float width = wrenGetSlotDouble(vm, nextSlot());
  float height = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);
  // wrenEnsureSlots(vm, 0);

  tc_draw_rectangle(x, y, width, height, *color);
}

static void wren_fill_circle(WrenVM *vm) {
  wrenInitSlot(0);
  int x = wrenGetSlotDouble(vm, nextSlot());
  int y = wrenGetSlotDouble(vm, nextSlot());
  float radius = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_fill_circle(x, y, radius, *color);
}

static void wren_draw_circle(WrenVM *vm) {
  wrenInitSlot(0);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float radius = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_circle(x, y, radius, *color);
}

tc_WrenLib wrenRenderLib[] = {
  {"s_clear()", wren_graphics_clear},
  {"s_clear(_)", wren_graphics_clear},
  {"s_fillRectangle(_,_,_,_)", wren_fill_rectangle},
  {"s_fillRectangle(_,_,_,_,_)", wren_fill_rectangle},
  {"s_drawRectangle(_,_,_,_,_)", wren_draw_rectangle},
  {"s_fillCircle(_,_,_,_)", wren_fill_circle},
  {"s_drawCircle(_,_,_,_)", wren_draw_circle}
};

/* Font class */

static void wren_font_allocate(WrenVM *vm) {
  tc_Font *font = wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_Font));
  const tc_uint8 *filename = wrenGetSlotString(vm, 1);
  tc_uint8 size = 16;
  if (wrenGetSlotCount(vm) > 2) size = wrenGetSlotDouble(vm, 2);

//   *font = tc_load_font(filename, size);
  size_t buffSize;
  tc_uint8 *data = tc_read_file(filename, &buffSize);
  tc_init_font(font, data, buffSize, size);
  free(data);
}

static void wren_font_finalize(void *data) {
  tc_Font *font = data;
  tc_destroy_font(font);
  free(font);
}

static void wren_font_print(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Font *font = wrenGetSlotForeign(vm, slot);
//   if (font->size == 0) *font = CORE.defaultFont;
  const tc_uint8* text = wrenGetSlotString(vm, nextSlot());
  int x = wrenGetSlotDouble(vm, nextSlot());
  int y = wrenGetSlotDouble(vm, nextSlot());
  float sx = 1;
  float sy = 1;
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > 5) {
    sx = wrenGetSlotDouble(vm, nextSlot());
    sy = wrenGetSlotDouble(vm, nextSlot());
  }
  color = wrenGetSlotForeign(vm, nextSlot());

  if (font->size == 0) tc_draw_text_scale(text, x, y, sx, sy, *color);
  else tc_draw_text_font_scale(*font, text, x, y, sx, sy, *color);
}

static tc_WrenLib wrenFontLib[] = {
  {"s_print(_,_,_,_)", wren_font_print},
  {"s_print(_,_,_,_,_,_)", wren_font_print},
  {"print(_,_,_,_)", wren_font_print},
  {"print(_,_,_,_,_,_)", wren_font_print}
};

/* Texture class */

static void wren_image_allocate(WrenVM *vm) {
  tc_Image *tex = (tc_Image*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_Image));
  if (wrenGetSlotCount(vm) < 3) {
    const char *filename = wrenGetSlotString(vm, 1);
    *tex = tc_load_image(filename);
  } else {
    void *data = (void *)wrenGetSlotString(vm, 1);
    int width = wrenGetSlotDouble(vm, 2);
    int height = wrenGetSlotDouble(vm, 3);
    *tex = tc_create_image(data, width, height);
  }
}

static void wren_image_finalize(void *data) {
  tc_Image *tex = (tc_Image*)data;
  tc_destroy_image(tex);
  free(tex);
}

static void wren_image_draw(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Image *tex = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm ,nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  // tc_Color color = WHITE;
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_image(*tex, x, y, *color);
}

static void wren_image_draw_scale(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Image *tex = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_image_scale(*tex, x, y, sx, sy, *color);
}

static void wren_image_draw_ex(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Image *tex = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float angle = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  float cx = wrenGetSlotDouble(vm, nextSlot());
  float cy = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_image_ex(*tex, x, y, angle, sx, sy, cx, cy, *color);
}

static void wren_image_draw_part(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Image *tex = wrenGetSlotForeign(vm, slot);
  tc_Rectf rect = wrenGetSlotRectList(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_image_part(*tex, rect, x, y, *color);
}

static void wren_image_draw_part_scale(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Image *tex = wrenGetSlotForeign(vm, slot);
  tc_Rectf rect = wrenGetSlotRectList(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_image_part_scale(*tex, rect, x, y, sx, sy, *color);
}

static void wren_image_draw_part_ex(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Image *tex = wrenGetSlotForeign(vm, slot);
  tc_Rectf rect = wrenGetSlotRectList(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float angle = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  float cx = wrenGetSlotDouble(vm, nextSlot());
  float cy = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_image_part_ex(*tex, rect, x, y, angle, sx, sy, cx, cy, *color);
}

static void wren_image_width(WrenVM *vm) {
  tc_Image *tex = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, tex->width);
}

static void wren_image_height(WrenVM *vm) {
  tc_Image *tex = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, tex->height);
}

static tc_WrenLib wrenImageLib[] = {
  {"draw(_,_,_)", wren_image_draw},
  {"draw(_,_,_,_,_)", wren_image_draw_scale},
  {"draw(_,_,_,_,_,_,_,_)", wren_image_draw_ex},
  {"drawPart(_,_,_,_)", wren_image_draw_part},
  {"drawPart(_,_,_,_,_,_)", wren_image_draw_part_scale},
  {"drawPart(_,_,_,_,_,_,_,_,_)", wren_image_draw_part_ex},
  {"width", wren_image_width},
  {"height", wren_image_height}
};

/*******************
 * Canvas class
 *******************/

static void wren_canvas_allocate(WrenVM *vm) {
  tc_Canvas *canvas = (tc_Canvas*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_Canvas));
  float width = wrenGetSlotDouble(vm, 1);
  float height = wrenGetSlotDouble(vm, 2);
  *canvas = tc_create_canvas(width, height);
}
static void wren_canvas_finalize(void *data) {
  tc_Canvas *canvas = data;
  tc_destroy_canvas(canvas);
  free(canvas);
}

static void wren_canvas_set(WrenVM* vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);
  tc_attach_canvas(*canvas);
}
static void wren_canvas_unset(WrenVM* vm) {
  tc_detach_canvas();
}

static void wren_canvas_width(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, canvas->width);
}
static void wren_canvas_height(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, canvas->height);
}

static void wren_canvas_draw(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Canvas *canvas = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_canvas(*canvas, x, y, *color);
}
static void wren_canvas_draw_scale(WrenVM *vm) {
  wrenInitSlot(0);
  tc_Canvas *canvas = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_Color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_canvas_scale(*canvas, x, y, sx, sy, *color);
}

static tc_WrenLib wrenCanvasLib[] = {
  {"set()", wren_canvas_set},
  {"unset()", wren_canvas_unset},
  {"width", wren_canvas_width},
  {"height", wren_canvas_height},
  {"draw(_,_,_)", wren_canvas_draw},
  {"draw(_,_,_,_,_)", wren_canvas_draw_scale}
};

/* Color class */

static void wren_color_allocate(WrenVM *vm) {
  tc_Color *color = (tc_Color*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_Color));
  *color = WHITE;
  if (wrenGetSlotCount(vm) < 5) {
    color->r = wrenGetSlotDouble(vm, 1);
    color->g = wrenGetSlotDouble(vm, 2);
    color->b = wrenGetSlotDouble(vm, 3);
  } else {
    color->r = wrenGetSlotDouble(vm, 1);
    color->g = wrenGetSlotDouble(vm, 2);
    color->b = wrenGetSlotDouble(vm, 3);
    color->a = wrenGetSlotDouble(vm, 4);
  }
}

static void wren_color_finalize(void *data) {
  free(data);
}

static void wren_color_r(WrenVM *vm) {
  tc_Color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->r);
}

static void wren_color_g(WrenVM *vm) {
  tc_Color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->g);
}

static void wren_color_b(WrenVM *vm) {
  tc_Color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->b);
}

static void wren_color_a(WrenVM *vm) {
  tc_Color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->a);
}


static tc_WrenLib wrenColorLib[] = {
  {"r", wren_color_r},
  {"g", wren_color_g},
  {"b", wren_color_b},
  {"a", wren_color_a}
};