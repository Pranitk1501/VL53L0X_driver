#include "VL53L0X.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

// Define configuration values
#define I2C_PORT 0
#define SCL_PIN 15
#define SDA_PIN 18
#define XSHUT_PIN 3 
#define UART_PORT_NUM UART_NUM_0


void read_task(void *pvParameters) {
   vl53l0x_t *sensor = (vl53l0x_t *)pvParameters;
    uint8_t uart_buffer[1];
   while (true) {
       uint16_t distance = vl53l0x_readRangeContinuousMillimeters(sensor);
       printf("Distance: %u mm\n", distance);

       // Check for Q input
       if (uart_read_bytes(UART_PORT_NUM, (uint8_t *)uart_buffer, 1, 20 / portTICK_PERIOD_MS) == 1 && uart_buffer[0] == 'Q') {
           printf("Q pressed, stopping task\n");
           break;
       }

       vTaskDelay(pdMS_TO_TICKS(20)); // Delay between readings
   }

   vTaskDelete(NULL); // Delete task when finished
}



void app_main() {
   
       // Configure UART for serial communication
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_driver_install(UART_PORT_NUM, 2048, 0, 0, NULL, 0);

    esp_rom_gpio_pad_select_gpio(XSHUT_PIN);
    gpio_set_direction(XSHUT_PIN, GPIO_MODE_INPUT);

   // Read initial XSHUT value
//    int xshut_value = gpio_get_level(XSHUT_PIN);

   // Configure VL53L0X with dynamic XSHUT value
   vl53l0x_t *sensor = vl53l0x_config(I2C_PORT, SCL_PIN, SDA_PIN, -1, 0x29, 2);

    if (sensor == NULL) {
        printf("Error configuring VL53L0X sensor\n");
        vTaskDelete(NULL);
    }
    else
    {
        printf("Config done");
    }

    const char *init_result = vl53l0x_init(sensor);

    if (init_result != NULL) {
        printf("Error initializing VL53L0X: %s\n", init_result);
        vTaskDelete(NULL);
    }
      else
    {
        printf("init done");
    }

    // Setting measurement timing budget and signal rate limit (example values)
    // vl53l0x_setMeasurementTimingBudget(sensor, 20000);
    // vl53l0x_setSignalRateLimit(sensor, 0.25);
    printf(" %u address",vl53l0x_getAddress(sensor));
    // Starting continuous measurement with a period of 20 ms
    vl53l0x_startContinuous(sensor, 20);

    // Reading continuous data for 10 seconds

    // vl53l0x_stopContinuous(sensor);
    xTaskCreate(read_task, "read_task", 2048, sensor, 5, NULL);
    // vTaskDelete(NULL);
}
