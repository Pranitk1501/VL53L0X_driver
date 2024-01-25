#include "VL53L0X.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



// Define configuration values
#define I2C_PORT 0
#define SCL_PIN 15
#define SDA_PIN 18
#define XSHUT_PIN -1 



void app_main() {
    // Configuring VL53L0X
    // vl53l0x_t *tof_sensor = {
    //     .port = -1,
    //     .address = 0x29,
    //     .xshut = -1,
    //     .io_timeout = 0, // Set to the desired value
    //     .io_2v8 = 1,
    //     .did_timeout = 0,
    //     .i2c_fail = 0
    // };

      vl53l0x_t *sensor = vl53l0x_config(I2C_PORT, SCL_PIN, SDA_PIN, XSHUT_PIN, 0x29, 1);

    if (sensor == NULL) {
        printf("Error configuring VL53L0X sensor\n");
        vTaskDelete(NULL);
    }

    const char *init_result = vl53l0x_init(sensor);

    if (init_result != NULL) {
        printf("Error initializing VL53L0X: %s\n", init_result);
        vTaskDelete(NULL);
    }

    // Setting measurement timing budget and signal rate limit (example values)
    vl53l0x_setMeasurementTimingBudget(sensor, 20000);
    vl53l0x_setSignalRateLimit(sensor, 0.25);
    printf(" %u address",vl53l0x_getAddress(sensor));
    // Starting continuous measurement with a period of 20 ms
    vl53l0x_startContinuous(sensor, 20);

    // Reading continuous data for 10 seconds
    for (int i = 0; i < 100; ++i) {
        uint16_t distance = vl53l0x_readRangeContinuousMillimeters(sensor);
        printf("Distance: %u mm\n", distance);
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms
    }

    // Stopping continuous measurement
    vl53l0x_stopContinuous(sensor);
    // vl53l0x_end(sensor);
    vTaskDelete(NULL);
}
