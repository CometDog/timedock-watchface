#pragma once
#define SETTINGS_KEY 0

typedef enum AppKey {
	APP_KEY_INVERT = 0,
	APP_KEY_WEATHER_FORMAT,
	APP_KEY_ANIMATIONS,
	APP_KEY_AUTO_OPEN,
	APP_KEY_DATE_FORMAT,
	APP_KEY_TEMPERATURE = 6,
	APP_KEY_CONDITION = 7,
	APP_KEY_REDUCE_ANIMATIONS = 8
} AppKey;

typedef struct Settings {
	bool lightTheme;
	bool fahrenheit;
	bool autoOpen;
	bool animations;
	bool reduceAnimations;
	bool altDateFormat;
	int16_t previousTemp;
	int16_t previousTempRaw;
	uint8_t previousCondition;
} Settings;

void data_framework_inbox(DictionaryIterator *iter, void *context);
Settings data_framework_get_settings();
int data_framework_save_settings(Settings settings);
int data_framework_load_settings();
void data_framework_load_default_settings();
void data_framework_init();
void data_framework_ask_for_weather_with_manners();