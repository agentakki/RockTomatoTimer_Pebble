#include <pebble.h>
#include "TaskMenu.h"
#include "Timer.h"
#include "comm.h"
  
Window *window;
MenuLayer *menu_layer;
char str1[] = "Noay";

void draw_row_callback (GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    task_t *task = get_task(cell_index->row);
    char subtitle[35];
    char format_str[] = "Target: %i pomos, %i done";
    snprintf(subtitle, sizeof(format_str), format_str, task->nTarget, task->nCompleted);
    if (getNTtasks() != 0)
      menu_cell_basic_draw(ctx, cell_layer, task->name, subtitle, NULL);
    else
      menu_cell_basic_draw(ctx, cell_layer, "IS NULL :(", "subtitle", NULL);
    return;
}

uint16_t num_rows_callback (MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  //menu_layer_reload_data(menu_layer);
  return getNTtasks();
}


void select_click_callback (MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
/*  int which = cell_index->row;
  uint32_t segments[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  for (int i = 0; i <= which; ++i) {
    segments[2*i] = 200;
    segments[(2*i)+1] = 100;
  }
  VibePattern pattern = {
    .durations = segments,
    .num_segments = 16
  };
  vibes_enqueue_custom_pattern(pattern);
*/
  timer_init();
}

void window_load1 (Window *window){
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "loading task menu");
  
  list_request();
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "should be done with list_request now");
  
  GRect bounds = layer_get_bounds(window_get_root_layer(window));
  menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(menu_layer, window);
  MenuLayerCallbacks callbacks = {
    .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
    .select_click = (MenuLayerSelectCallback) select_click_callback
  };
  menu_layer_set_callbacks(menu_layer, NULL, callbacks);
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "done task menu");
}

void window_unload1 (Window *window) {
  menu_layer_destroy(menu_layer);
}

void init_menu() {
  window = window_create();
  WindowHandlers handlers = {
    .load = window_load1,
    .unload = window_unload1
  };
  window_set_window_handlers (window, (WindowHandlers) handlers);
  window_stack_push (window, true);
}

void deinit_menu() {
  window_destroy(window);
}
