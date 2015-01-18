#include <pebble.h>
#include "TaskMenu.h"
#include "Timer.h"
#include "comm.h"
  
int main(void) {
  init_comm();
  init_menu();
  app_event_loop();
  deinit_menu();
}