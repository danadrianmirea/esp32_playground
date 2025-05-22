#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN 2
#define BLINK_DELAY_MS 100

static const char *TAG = "BLINK";

void app_main(void)
{
    // Configure LED pin as output
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "ESP32 LED Blink Example");

    while (1) {
        // Turn LED on
        gpio_set_level(LED_PIN, 1);
        ESP_LOGI(TAG, "LED ON");
        vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);

        // Turn LED off
        gpio_set_level(LED_PIN, 0);
        ESP_LOGI(TAG, "LED OFF");
        vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
    }
} 