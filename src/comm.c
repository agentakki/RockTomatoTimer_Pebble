#include <pebble.h>
#include <stdio.h>
#include "comm.h"
  
task_t **tasks = NULL; // list of tasks, global to this file, 
                      // should never be accessed directly other
                      // than by functions in this file

int nTasks = 0;
int expectedTasks = 0;

task_t** get_tasks() {
  return tasks;
}

int getNTtasks() {
  return nTasks;
}

task_t* get_task(int index) {
  if (index < 0 || nTasks < index) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "get_task OUT OF RANGE");
    return NULL;
  }
  return tasks[index];
}

void pomo_completed(int t_id) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sending pomo");
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "iter is null in pomo_completed");
    return;
  } 
  dict_write_cstring(iter, 0, POMO_COMPLETE);
  dict_write_int(iter, 1, &t_id, sizeof(int), false);
  dict_write_end(iter);
  app_message_outbox_send();
  
}

void push_task(Tuple *t, DictionaryIterator *iterator) {
  // TODO: WHAT IF THE LIST IS FULL
  
  task_t *new_task = (task_t*) malloc(sizeof(task_t));
  
  new_task->t_id = t->value->int32;
  t = dict_read_next(iterator);
  strcpy(new_task->name, t->value->cstring);
  t = dict_read_next(iterator);
  new_task->nTarget = t->value->int32;
  t = dict_read_next(iterator);
  new_task->nCompleted = t->value->int32;
  tasks[nTasks] = new_task;
  ++nTasks;
  
  APP_LOG(APP_LOG_LEVEL_INFO, "added task with name '%s'", tasks[nTasks-1]->name);
}

void list_request() {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sending list request");
 
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "iter is null in list_request");
    return;
  } 

  dict_write_cstring(iter, 0, LIST_REQUEST);
  dict_write_end(iter);
  app_message_outbox_send();
  
}

void free_tasks() {
  if (tasks != NULL) {
    task_t **curr = tasks;
    while (*curr != NULL) 
      free (*curr);
    free (tasks);
  }
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  if (!strcmp(t->value->cstring, LIST_RESPONSE)) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "received LIST_RESPONSE");
    t = dict_read_next(iterator);
    free_tasks();
    tasks = calloc(t->value->int32, sizeof(task_t*));
    nTasks = 0;
    expectedTasks = t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "expecting %ld tasks", t->value->int32);
  }
  else if (!strcmp(t->value->cstring, TASK)) {
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "received TASK");
    
      
    t = dict_read_next(iterator);
    push_task(t, iterator);
    if(nTasks == expectedTasks)
    {
      APP_LOG(APP_LOG_LEVEL_INFO, "Here are the tasks:");
      for(int i=0; i<nTasks; i++){
        //APP_LOG(APP_LOG_LEVEL_INFO, "id: %i", i);
             APP_LOG(APP_LOG_LEVEL_INFO, "id: %i %s", tasks[i]->t_id, tasks[i]->name);
      }
    }
    
  }
  else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "received unkown message from iOS app ...");
  }
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void init_comm() {
   // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
