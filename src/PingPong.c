#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "lang_de.h"

#define MY_UUID { 0xEC, 0x10, 0x26, 0xB1, 0xBA, 0xE2, 0x47, 0xFA, 0x92, 0x89, 0x7B, 0x0E, 0x4C, 0xFC, 0x18, 0x2F }
PBL_APP_INFO(MY_UUID,
 TXT_TITLE, "Gerald Schneider",
             0, 1, /* App version */
 RESOURCE_ID_IMAGE_MENU_ICON,
             //IMAGE_MENU_ICON,
 APP_INFO_STANDARD_APP);

#define FRAME_MATCH (GRect(5, 0, 144-10, 20))
#define FRAME_SCORE_ME (GRect(5, 20, 144-10, 100))
#define FRAME_SCORE_OPPONENT (GRect(5, 75, 144-10, 100))
#define FRAME_DURATION (GRect(5, 168-40, 144-10, 20))

#define TEXT_COLOR GColorBlack
#define BACKGROUND_COLOR GColorClear

#define FONT_MATCH FONT_KEY_GOTHIC_18
#define FONT_SCORE FONT_KEY_GOTHAM_42_BOLD
#define FONT_DURATION FONT_KEY_GOTHIC_18

// https://github.com/iNate71/RobotoClock/blob/master/src/jelly_bean_clock.c
// http://en.wikipedia.org/wiki/Ping_pong

Window window;
Layer match_layer;
Layer score_layer_me;
Layer score_layer_opponent;
Layer duration_layer;

int match_me = 0;
int match_opponent = 0;
int score_me = 0;
int score_opponent = 0;

void update_match_layer_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  graphics_text_draw(ctx,
         TXT_MATCH,
         fonts_get_system_font(FONT_MATCH),
         FRAME_MATCH,
         GTextOverflowModeWordWrap,
         GTextAlignmentCenter,
         NULL);
}

void update_score_layer_me_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  //char* scoreSelf;
  //sprintf(scoreSelf, TXT_ScoreSelf, 0);
  graphics_text_draw(ctx,
         TXT_SCORE_SELF,
         fonts_get_system_font(FONT_SCORE),
         FRAME_SCORE_ME,
         GTextOverflowModeWordWrap,
         GTextAlignmentLeft,
         NULL);
}

void update_score_layer_opponent_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  graphics_text_draw(ctx,
         TXT_SCORE_OPPONENT,
         fonts_get_system_font(FONT_SCORE),
         FRAME_SCORE_OPPONENT,
         GTextOverflowModeWordWrap,
         GTextAlignmentLeft,
         NULL);
}

void update_duration_layer_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  graphics_text_draw(ctx,
         TXT_DURATION,
         fonts_get_system_font(FONT_DURATION),
         FRAME_DURATION,
         GTextOverflowModeWordWrap,
         GTextAlignmentCenter,
         NULL);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, TXT_TITLE);
  window_stack_push(&window, true /* Animated */);

  layer_init(&score_layer_me, window.layer.frame);
  score_layer_me.update_proc = update_score_layer_me_callback;
  layer_add_child(&window.layer, &score_layer_me);
  layer_mark_dirty(&score_layer_me);

  layer_init(&score_layer_opponent, window.layer.frame);
  score_layer_opponent.update_proc = update_score_layer_opponent_callback;
  layer_add_child(&window.layer, &score_layer_opponent);
  layer_mark_dirty(&score_layer_opponent);

  layer_init(&match_layer, window.layer.frame);
  match_layer.update_proc = update_match_layer_callback;
  layer_add_child(&window.layer, &match_layer);
  layer_mark_dirty(&match_layer);

  layer_init(&duration_layer, window.layer.frame);
  duration_layer.update_proc = update_duration_layer_callback;
  layer_add_child(&window.layer, &duration_layer);
  layer_mark_dirty(&duration_layer);

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };

  app_event_loop(params, &handlers);
}
