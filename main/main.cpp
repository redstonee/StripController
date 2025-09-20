#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "blue.h"
#include "LED.hpp"

#include "config.h"

constexpr auto TAG = "Maaaaaain";

static bool isPairing = false;

// Global variables, use with caution
SemaphoreHandle_t colorConfigQueue; // Color setting from BLE, 0 for off, 0xffffffff for on
SemaphoreHandle_t paringQueue;      // Color setting from BLE

void ledTask(void *param)
{
    ShittyLED ledR(LED_R_PIN, LED_R_CH);
    ShittyLED ledG(LED_G_PIN, LED_G_CH);
    ShittyLED ledB(LED_B_PIN, LED_B_CH);

    auto setColor = [&ledR, &ledG, &ledB](uint32_t color)
    {
        ESP_LOGI(TAG, "Color is set to %06lx", color);
        ledR.setBrightness((color >> 16) & 0xFF);
        ledG.setBrightness((color >> 8) & 0xFF);
        ledB.setBrightness(color & 0xFF);
    };

    // Initialize the NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    nvs_handle_t nvsHandle;
    ESP_ERROR_CHECK(nvs_open("Strip", NVS_READWRITE, &nvsHandle));

    // Get previously configured color
    uint32_t colorConfigured = 0x808080;
    nvs_get_u32(nvsHandle, "color", &colorConfigured);
    setColor(colorConfigured);
    blue::updateColor(colorConfigured);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (isPairing)
        {
            static bool dirUp = true;
            setColor(dirUp ? 0x00A000 : 0x001000);
            dirUp = !dirUp;
            continue;
        }

        uint32_t colorRecv;
        if (xQueueReceive(colorConfigQueue, &colorRecv, 0))
        {
            if (colorRecv == 0xffffffff)
            {
                setColor(colorConfigured);
                blue::updateColor(colorConfigured);
            }
            else
            {
                setColor(colorRecv);
                if (colorRecv != 0)
                {
                    colorConfigured = colorRecv;
                    nvs_set_u32(nvsHandle, "color", colorConfigured);
                    nvs_commit(nvsHandle);
                }
                else
                    blue::updateColor(0);
            }
        }
    }
}

extern "C" void app_main()
{
    static const uint32_t zero = 0;
    static const uint32_t allF = 0xffffffff;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = 80,
        .min_freq_mhz = 40,
        // .light_sleep_enable = true,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    blue::init();
    
    colorConfigQueue = xQueueCreate(5, sizeof(uint32_t));
    xTaskCreate(ledTask, "LED Task", 4096, nullptr, 2, nullptr);

    blue::startAdvertising(); // TODO: A pairing process
    ESP_LOGI(TAG, "BLE initialized");

    auto onPairingStop = []
    {
        ESP_LOGI(TAG, "Pairing complete");
        xQueueSend(colorConfigQueue, &allF, portMAX_DELAY);
        isPairing = false;
    };
    blue::setOnConnectCallback(onPairingStop);
    blue::setOnAdvCompleteCallback([&onPairingStop](NimBLEAdvertising *)
                                   { onPairingStop(); });

    gpio_config_t buttonIOConfig = {
        .pin_bit_mask = 1ULL << BUTTON_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&buttonIOConfig);

    static bool isLedOn = true;
    static uint8_t pressCnt = 0;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(20));

        if (!gpio_get_level(static_cast<gpio_num_t>(BUTTON_PIN)))
        {
            if (pressCnt < 30)
                pressCnt++;
        }
        else
        {
            // Long-press to enter pairing mode
            if ((pressCnt >= 30) && (!isPairing))
            {
                // TODO: Enter pairing mode
                isPairing = true;
            }
            // Short-press to toggle on/off or cancel pairing mode
            else if (pressCnt > 3)
            {
                if (isPairing)
                    onPairingStop();
                else
                {
                    isLedOn = !isLedOn;
                    xQueueSend(colorConfigQueue, isLedOn ? &allF : &zero, portMAX_DELAY);
                    blue::updateColor(isLedOn);
                }
            }
            pressCnt = 0;
        }
    }
}