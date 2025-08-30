#pragma once

#include <driver/ledc.h>
#include <esp_err.h>

class ShittyLED
{
private:
    ledc_channel_t _channel;

    bool fadeEnd = false;

    static IRAM_ATTR bool ledc_fade_end_event(const ledc_cb_param_t *param, void *user_arg)
    {
        ShittyLED *led = static_cast<ShittyLED *>(user_arg);
        led->fadeEnd = true;
        return true;
    }

public:
    ShittyLED(int gpio, ledc_channel_t channel) : _channel(channel)
    {
        ledc_timer_config_t ledc_timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_8_BIT,
            .timer_num = LEDC_TIMER_0,
            .freq_hz = 4000,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        ledc_channel_config_t ledc_channel = {
            .gpio_num = gpio,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = _channel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0, // Set duty to 0%
            .hpoint = 0,
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

        // Initialize fade service.
        ledc_fade_func_install(0);
        ledc_cbs_t callbacks = {.fade_cb = ledc_fade_end_event};
        ledc_cb_register(LEDC_LOW_SPEED_MODE, _channel, &callbacks, this);
    }

    void setBrightness(uint8_t brightness)
    {
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, _channel, brightness, 1000, LEDC_FADE_NO_WAIT);
    }
};
