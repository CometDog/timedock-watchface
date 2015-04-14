#include <pebble.h>
#include "main_window.h"
#include "lignite_data_framework.h"

Settings data_framework_local_settings;

void process_tuple(Tuple *t){
    int key = t->key;
    int value = t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded key %d with value %d.", key, value);
    switch (key) {
        case APP_KEY_INVERT:
            data_framework_local_settings.lightTheme = value;
            break;
        case APP_KEY_WEATHER_FORMAT:
            data_framework_local_settings.fahrenheit = value;
            if(data_framework_local_settings.fahrenheit){
                data_framework_local_settings.previousTemp = (1.8*(data_framework_local_settings.previousTempRaw-273)+32);
            }
            else{
                data_framework_local_settings.previousTemp = data_framework_local_settings.previousTempRaw-273;
            }
            break;
        case APP_KEY_ANIMATIONS:
            data_framework_local_settings.animations = value;
            break;
        case APP_KEY_AUTO_OPEN:
            data_framework_local_settings.autoOpen = value;
            if(data_framework_local_settings.autoOpen){
                app_worker_launch();
            }
            else{
                app_worker_kill();
            }
            break;
        case APP_KEY_DATE_FORMAT:
            data_framework_local_settings.altDateFormat = value;
            break;
        case APP_KEY_TEMPERATURE:
            data_framework_local_settings.previousTemp = value;
            data_framework_local_settings.previousTempRaw = value;
            if(data_framework_local_settings.fahrenheit){
                data_framework_local_settings.previousTemp = (1.8*(value-273)+32);
            }
            else{
                data_framework_local_settings.previousTemp = value-273;
            }
            break;
        case APP_KEY_CONDITION:
            data_framework_local_settings.previousCondition = value;
            break;
        case APP_KEY_REDUCE_ANIMATIONS:
            data_framework_local_settings.reduceAnimations = value;
            break;
    }
}

void data_framework_ask_for_weather_with_manners(){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_uint16(iter, 200, 202);
  dict_write_end(iter);

  app_message_outbox_send();
}

void data_framework_inbox(DictionaryIterator *iter, void *context){
    Tuple *t = dict_read_first(iter);
    if(t){
        process_tuple(t);
    }
    while(t != NULL){
        t = dict_read_next(iter);
        if(t){
            process_tuple(t);
        }
    }
    main_window_update_settings(data_framework_local_settings);
}

Settings data_framework_get_settings(){
    return data_framework_local_settings;
}

int data_framework_save_settings(Settings settings){
    int result = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
    return result;
}

int data_framework_load_settings(){
    int result = persist_read_data(SETTINGS_KEY, &data_framework_local_settings, sizeof(data_framework_local_settings));
    return result;
}

void data_framework_load_default_settings(){
    data_framework_local_settings.lightTheme = false;
    data_framework_local_settings.fahrenheit = true;
    data_framework_local_settings.animations = true;
    data_framework_local_settings.reduceAnimations = false;
    data_framework_local_settings.autoOpen = true;
    data_framework_local_settings.altDateFormat = true;
    data_framework_local_settings.previousTemp = 0;
    data_framework_local_settings.previousCondition = 0;
}

void data_framework_init(){
    app_message_register_inbox_received(data_framework_inbox);
    app_message_open(512, 512);
}