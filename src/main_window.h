#pragma once
#define AMOUNT_OF_ANGLES 3
#include "lignite_data_framework.h"

void main_window_init();
Window *main_window_get_window();
void main_window_destroy();
void main_window_update_settings(Settings new_settings);