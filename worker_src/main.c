#include <pebble_worker.h>

void battery_handler(BatteryChargeState state){
	if(state.is_plugged){
		worker_launch_app();
	}
}

int main() {
	battery_state_service_subscribe(battery_handler);
	worker_event_loop();
	return 666;
}
