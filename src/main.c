#include <pebble.h>
#include "lignite_data_framework.h"
#include "main_window.h"

int main() {
	data_framework_init();
    main_window_init();
    window_stack_push(main_window_get_window(), true);
    app_event_loop();
    main_window_destroy();
    return 1337;
}
