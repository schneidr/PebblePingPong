#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xD6, 0xB0, 0xA8, 0x14, 0x0A, 0xBF, 0x43, 0xF5, 0x90, 0xEE, 0x20, 0xA2, 0xEC, 0xCE, 0xC2, 0xEE }
PBL_APP_INFO(MY_UUID,
             "Ping Pong", "Gerald Schneider",
             0, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

Layer layer;

void update_layer_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, GColorBlack);

  graphics_text_draw(ctx,
         "Ich: 00",
         fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD),
         GRect(5, 5, 144-10, 100),
         GTextOverflowModeWordWrap,
         GTextAlignmentLeft,
         NULL);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Ping Pong");
  window_stack_push(&window, true /* Animated */);

  layer_init(&layer, window.layer.frame);
  layer.update_proc = update_layer_callback;
  layer_add_child(&window.layer, &layer);
  layer_mark_dirty(&layer);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };

  app_event_loop(params, &handlers);
}
