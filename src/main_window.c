#include <pebble.h>
#include "main_window.h"
#include "extras.h"

Window *main_window;
Settings main_settings;
GBitmap *current_icon;
BatteryChargeState current_state;
AppTimer *weather_timer;
bool boot = true;

TextLayer *time_layer, *date_layer, *temperature_layer, *condition_layer, *percent_layer;
Layer *graphics_layer;
BitmapLayer *triangle_layer;

static const char *climacon_condition_codes[8] = {
	"F", //Thunder
	"'", //Drizzle
	"*", //Rain
	"9", //Snow
	"<", //Haze or fog
	"I", //Clear
	"!", //Few clouds
	"?"  //Scattered
};

void graphics_proc(Layer *layer, GContext *ctx){
	#ifdef PBL_COLOR
		graphics_context_set_fill_color(ctx, GColorRed);
		graphics_fill_rect(ctx, GRect(8, 154, 42, 8), 2, GCornersAll);
		graphics_context_set_fill_color(ctx, GColorYellow);
		graphics_fill_rect(ctx, GRect(40, 154, 42, 8), 2, GCornerNone);
		graphics_context_set_fill_color(ctx, GColorGreen);
		graphics_fill_rect(ctx, GRect(78, 154, 58, 8), 2, GCornersRight);
	#else
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, GRect(8, 154, 128, 8), 2, GCornersAll);
	#endif

	graphics_context_set_fill_color(ctx, GColorWhite);
}

void main_window_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%R", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
	}
	text_layer_set_text(time_layer, time_buffer);

	static char date_buffer[] = "This is the date...";
	if(main_settings.altDateFormat){
		strftime(date_buffer, sizeof(date_buffer), "%a, %m.%d.%y", t);
	}
	else{
		strftime(date_buffer, sizeof(date_buffer), "%a, %d.%m.%y", t);
	}
	text_layer_set_text(date_layer, date_buffer);
}

void battery_handler(BatteryChargeState state){
	current_state = state;

	static char battery_buffer[] = "Hello there";
	snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", state.charge_percent);
	text_layer_set_text(percent_layer, battery_buffer);

	if(!boot){
		layer_set_frame(text_layer_get_layer(percent_layer), GRect(8+(11.2*(state.charge_percent/10)), 124, 36, 24));
	}
	else{
		boot = false;
	}
	layer_set_frame(bitmap_layer_get_layer(triangle_layer), GRect(8+(11.2*(state.charge_percent/10)), 138, 28, 28));
}

void weather_callback(){
	data_framework_ask_for_weather_with_manners();
	weather_timer = app_timer_register(900000, weather_callback, NULL);
}

void main_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);

	time_layer = text_layer_init(GRect(-150, 48, 144, 168), GTextAlignmentCenter, 0);
	text_layer_set_text(time_layer, "Test");
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	date_layer = text_layer_init(GRect(-150, 48, 144, 168), GTextAlignmentCenter, 1);
	text_layer_set_text(date_layer, "Hello");
	layer_add_child(window_layer, text_layer_get_layer(date_layer));

	temperature_layer = text_layer_init(GRect(-150, 92, 36, 36), GTextAlignmentCenter, 2);
	text_layer_set_text(temperature_layer, "22°");
	layer_add_child(window_layer, text_layer_get_layer(temperature_layer));

	condition_layer = text_layer_init(GRect(350, 78, 64, 64), GTextAlignmentCenter, 3);
	text_layer_set_text(condition_layer, ">");
	layer_add_child(window_layer, text_layer_get_layer(condition_layer));

	graphics_layer = layer_create(GRect(0, 168, 144, 168));
	layer_set_update_proc(graphics_layer, graphics_proc);
	layer_add_child(window_layer, graphics_layer);

	triangle_layer = bitmap_layer_create(GRect(10, 138, 28, 28));
	bitmap_layer_set_compositing_mode(triangle_layer, GCompOpClear);
	bitmap_layer_set_bitmap(triangle_layer, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TRIANGLE_BLACK));
	layer_add_child(window_layer, bitmap_layer_get_layer(triangle_layer));

	percent_layer = text_layer_init(GRect(-150, 124, 36, 24), GTextAlignmentCenter, 1);
	text_layer_set_text(percent_layer, "10%");
	layer_add_child(window_layer, text_layer_get_layer(percent_layer));

	BatteryChargeState state = battery_state_service_peek();

	if(main_settings.animations){
		animate_layer(text_layer_get_layer(time_layer), &GRect(0, -150, 144, 168), &GRect(0, 4, 144, 168), 600, 250);
		animate_layer(text_layer_get_layer(date_layer), &GRect(0, -150, 144, 168), &GRect(0, 48, 144, 168), 600, 0);
		if(!main_settings.reduceAnimations){
			animate_layer(text_layer_get_layer(temperature_layer), &GRect(-150, 92, 36, 36), &GRect(28, 80, 36, 36), 600, 500);
			animate_layer(text_layer_get_layer(condition_layer), &GRect(350, 78, 64, 64), &GRect(58, 68, 64, 64), 600, 650);
			animate_layer(graphics_layer, &GRect(0, 168, 144, 168), &GRect(0, 0, 144, 168), 600, 800);
			animate_layer(text_layer_get_layer(percent_layer), &GRect(350, 124, 36, 24), &GRect(8+(11.2*(state.charge_percent/10)), 124, 36, 24), 600, 950);

		}
		else{
			layer_set_frame(text_layer_get_layer(temperature_layer), GRect(28, 80, 36, 36));
			layer_set_frame(text_layer_get_layer(condition_layer), GRect(58, 68, 64, 64));
			layer_set_frame(graphics_layer, GRect(0, 0, 144, 168));
			layer_set_frame(text_layer_get_layer(percent_layer), GRect(8+(11.2*(state.charge_percent/10)), 124, 36, 24));
		}
	} 
	else {
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, 4, 144, 168));
		layer_set_frame(text_layer_get_layer(date_layer), GRect(0, 48, 144, 168));
		layer_set_frame(text_layer_get_layer(temperature_layer), GRect(28, 80, 36, 36));
		layer_set_frame(text_layer_get_layer(condition_layer), GRect(58, 68, 64, 64));
		layer_set_frame(graphics_layer, GRect(0, 0, 144, 168));
		layer_set_frame(text_layer_get_layer(percent_layer), GRect(8+(11.2*(state.charge_percent/10)), 124, 36, 24));
	}

  	main_window_update_settings(main_settings);
  	battery_handler(state);

  	weather_timer = app_timer_register(900000, weather_callback, NULL);
}

void main_window_unload(Window *window){
	text_layer_destroy(time_layer);
}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	tick_timer_service_subscribe(MINUTE_UNIT, main_window_tick_handler);
	battery_state_service_subscribe(battery_handler);
	int result = data_framework_load_settings();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d bytes read from settings.", result);

	main_settings = data_framework_get_settings();

	if(main_settings.autoOpen){
		app_worker_launch();
	}
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_destroy(){
	window_destroy(main_window);
	int result = data_framework_save_settings(main_settings);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d bytes written to settings.", result);
}

void main_window_update_settings(Settings new_settings){
	main_settings = new_settings;

	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
  	main_window_tick_handler(t, MINUTE_UNIT);

  	static char temperature_buffer[] = "Temp.";
  	snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", main_settings.previousTemp);
  	text_layer_set_text(temperature_layer, temperature_buffer);

  	static char condition_buffer[] = "...";
  	snprintf(condition_buffer, sizeof(condition_buffer), "%s", climacon_condition_codes[main_settings.previousCondition]);
  	text_layer_set_text(condition_layer, condition_buffer);
}