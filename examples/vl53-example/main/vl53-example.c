#include "VL53L0X.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


// Define configuration values
#define I2C_PORT 0
#define SCL_PIN 15
#define SDA_PIN 18
#define XSHUT_PIN 3 



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
    gpio_pad_select_gpio(XSHUT_PIN);
   gpio_set_direction(XSHUT_PIN, GPIO_MODE_INPUT);

   // Read initial XSHUT value
   int xshut_value = gpio_get_level(XSHUT_PIN);

   // Configure VL53L0X with dynamic XSHUT value
   vl53l0x_t *sensor = vl53l0x_config(I2C_PORT, SCL_PIN, SDA_PIN, xshut_value, 0x29, 2);

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
    // vl53l0x_end(sensor);

    for (int i = 0; i < 100; ++i) {
        uint16_t distance = vl53l0x_readRangeContinuousMillimeters(sensor);
        printf("Distance2: %u mm\n", distance);
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 ms
    }

    // vl53l0x_stopContinuous(sensor);

    vTaskDelete(NULL);
}
