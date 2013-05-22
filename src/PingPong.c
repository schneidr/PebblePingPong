#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mini-printf.h"

#include "lang_en.h"

#define MY_UUID { 0xEC, 0x10, 0x26, 0xB1, 0xBA, 0xE2, 0x47, 0xFA, 0x92, 0x89, 0x7B, 0x0E, 0x4C, 0xFC, 0x18, 0x2F }
PBL_APP_INFO(MY_UUID,
            TXT_TITLE, "Gerald Schneider",
            0, 1,
            RESOURCE_ID_IMAGE_MENU_ICON,
            APP_INFO_STANDARD_APP);

#define FRAME_MATCH (GRect(5, 0, 144-10, 20))
#define FRAME_SCORE_ME (GRect(5, 75, 144-10, 100))
#define FRAME_SCORE_OPPONENT (GRect(5, 20, 144-10, 100))
#define FRAME_DURATION (GRect(5, 168-40, 144-10, 20))

#define TEXT_COLOR GColorBlack
#define BACKGROUND_COLOR GColorClear

#define FONT_MATCH FONT_KEY_GOTHIC_18_BOLD
#define FONT_SCORE FONT_KEY_GOTHAM_42_BOLD
#define FONT_DURATION FONT_KEY_GOTHIC_18

#define VIBE_NONE 0
#define VIBE_SINGLE 1
#define VIBE_DOUBLE 2
#define VIBE_LONG 3

Window window;
Layer match_layer;
Layer score_layer_me;
Layer score_layer_opponent;
Layer duration_layer;

int match_me = 0;
int match_opponent = 0;
int score_me = 0;
int score_opponent = 0;
int duration_seconds;
int count = 0;
bool overtime = false;
bool paused = false;

void update_score(int me, int opponent) {
  int vibrate = VIBE_NONE;
  score_me += me;
  score_opponent += opponent;
  count++;
  if (score_me == 10 && score_opponent == 10) {
    overtime = true;
  }
  if (((count % 2) == 0 && overtime == false) || overtime) {
    vibrate = VIBE_SINGLE;
  }

  if ((!overtime && score_me == 11) || (overtime && (score_me >= (score_opponent + 2)))) {
    match_me++;
    score_me = 0;
    score_opponent = 0;
    count = 0;
    overtime = false;
    layer_mark_dirty(&match_layer);
    vibrate = VIBE_DOUBLE;
  }

  if ((!overtime && score_opponent == 11) || (overtime && (score_opponent >= (score_me + 2)))) {
    match_opponent++;
    score_me = 0;
    score_opponent = 0;
    count = 0;
    overtime = false;
    layer_mark_dirty(&match_layer);
    vibrate = VIBE_DOUBLE;
  }

  if (match_me == 3 || match_opponent == 3) {
    paused = true;
    vibrate = VIBE_LONG;
  }
  
  switch (vibrate) {
    case VIBE_SINGLE:
      vibes_short_pulse();
      break;
    case VIBE_DOUBLE:
      vibes_double_pulse();
      break;
    case VIBE_LONG:
      vibes_long_pulse();
      break;
  }

  layer_mark_dirty(&score_layer_me);
  layer_mark_dirty(&score_layer_opponent);
}

void update_match_layer_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  static char txt[21];
  mini_snprintf(txt, 20, TXT_MATCH, match_me, match_opponent);
  graphics_text_draw(ctx,
         txt,
         fonts_get_system_font(FONT_MATCH),
         FRAME_MATCH,
         GTextOverflowModeWordWrap,
         GTextAlignmentCenter,
         NULL);
}

void update_score_layer_me_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  static char txt[11];
  mini_snprintf(txt, 10, TXT_SCORE_SELF, score_me);
  graphics_text_draw(ctx,
         txt,
         fonts_get_system_font(FONT_SCORE),
         FRAME_SCORE_ME,
         GTextOverflowModeWordWrap,
         GTextAlignmentLeft,
         NULL);
}

void update_score_layer_opponent_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  static char txt[11];
  mini_snprintf(txt, 10, TXT_SCORE_OPPONENT, score_opponent);
  graphics_text_draw(ctx,
         txt,
         fonts_get_system_font(FONT_SCORE),
         FRAME_SCORE_OPPONENT,
         GTextOverflowModeWordWrap,
         GTextAlignmentLeft,
         NULL);
}

void update_duration_layer_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, TEXT_COLOR);

  int minutes = (int)duration_seconds / 60;
  int seconds = duration_seconds % 60;
  static char txt[21];
  mini_snprintf(txt, 20, TXT_DURATION, minutes, seconds);
  graphics_text_draw(ctx,
         txt,
         fonts_get_system_font(FONT_DURATION),
         FRAME_DURATION,
         GTextOverflowModeWordWrap,
         GTextAlignmentCenter,
         NULL);
}

void start_match() {
  match_me = 0;
  match_opponent = 0;
  score_me = 0;
  score_opponent = 0;
  duration_seconds = 0;
  count = 0;
  overtime = false;
  paused = false;
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  if (paused) return;
  update_score(0, 1);
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  if (paused) return;
  update_score(1, 0);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  paused = !paused;
}

void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  start_match();
  layer_mark_dirty(&duration_layer);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *event) {
  (void)ctx;
  (void)event;
  if (paused) return;
  duration_seconds++;
  layer_mark_dirty(&match_layer);
  layer_mark_dirty(&score_layer_opponent);
  layer_mark_dirty(&score_layer_me);
  layer_mark_dirty(&duration_layer);
}


void config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_UP ]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP ]->click.repeat_interval_ms = 1000;
  config[BUTTON_ID_DOWN ]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN ]->click.repeat_interval_ms = 1000;
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 1000;
  // long click config:
  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;
  config[BUTTON_ID_SELECT]->long_click.delay_ms = 700;
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, TXT_TITLE);
  window_stack_push(&window, true /* Animated */);

  start_match();

  window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);

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
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };

  app_event_loop(params, &handlers);
}
