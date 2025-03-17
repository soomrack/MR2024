#include <DHT11.h>
#include "Microclimate.hpp"


MicroclimateController& controller = MicroclimateController::get_instance();


void setup() {
    controller.set_time_day_start(7);
    controller.set_time_day_end(20);
    controller.set_target_soil_humidity_level(45);
    controller.set_target_temp_level(25);
    controller.set_target_air_humidity_level(35);
    controller.set_air_out_time(15, 30, 16, 0);

    Serial.begin(115200);
}


void loop() {
    controller.update();
    controller.print_current_environment_params();
}
