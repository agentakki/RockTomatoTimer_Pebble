#pragma once
  
#define WORK_SECS 4
#define BREAK_SECS 2

//functions
void select_click_handler(ClickRecognizerRef recognizer, void *context);
//static void select_reset_handler(ClickRecognizerRef recognizer, void *context);
void click_config_provider(Window *window);
  
void setup_timer();
void pause_timer(void);
void resume_timer(void);
void update_timer(void* content);

void timer_init(void);
void handle_deinit(void);
void window_load(Window *window);
void window_unload(Window *window);

void pass_variables(int pomos_remaining, int pomos_target, char* task_name);