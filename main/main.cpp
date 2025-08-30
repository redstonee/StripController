#include <Arduino.h>
#include <Preferences.h>
#include <utility>
#include "esp_log.h"
#include "esp_pm.h"

#include "blue.h"

#include "config.h"

constexpr auto TAG = "Keyboard";

extern "C" void app_main()
{
    initArduino();
    ESP_LOGI(TAG, "Morning!");

    ESP_LOGI(TAG, "RGB initialized");

    blue::init();
    ESP_LOGI(TAG, "BLE initialized");

    while (1)
    {
        delay(20);
    }
}