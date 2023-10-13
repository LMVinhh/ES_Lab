#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// GPIO Setting
#define BUTTON_GPIO GPIO_NUM_2  // Define BUTTON_GPIO as a gpio_num_t



void task1(void *para) {
    while (1) {
        printf("2120082 \n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
#define BUTTON_GPIO GPIO_NUM_2  // Define BUTTON_GPIO as a gpio_num_t

void task2(void *para) {
    gpio_pad_select_gpio ( GPIO_NUM_0 );
    gpio_set_direction ( GPIO_NUM_0 , GPIO_MODE_OUTPUT );
    gpio_config_t io_conf;
    // Configure the button GPIO as input with pull-up
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE; // No interrupt for polling
    gpio_config(&io_conf);

    int last_button_state = 1; // Initialize to not pressed
    int ledstate = 0;
    while (1) {
        int new_button_state = gpio_get_level(BUTTON_GPIO);
        if (new_button_state == 0 && last_button_state == 1) {
            // Button was pressed
            printf("ESP32\n");
            ledstate = ~ledstate;
            gpio_set_level ( GPIO_NUM_0 , ledstate);

        }
        last_button_state = new_button_state;
        vTaskDelay(10 / portTICK_PERIOD_MS); // Polling delay
    }
    vTaskDelete(NULL);

}

void app_main(void) {
    xTaskCreate(task1, "cyclic_task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task2, "acyclic_task", configMINIMAL_STACK_SIZE + 1024, NULL, 2, NULL);
}
