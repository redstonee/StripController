#pragma once

#include <cstdint>
#include "hal/ledc_types.h"

constexpr auto BUTTON_PIN = 6;

constexpr auto LED_R_PIN = 1;
constexpr auto LED_G_PIN = 3;
constexpr auto LED_B_PIN = 0;

constexpr auto LED_R_CH = LEDC_CHANNEL_0;
constexpr auto LED_G_CH = LEDC_CHANNEL_1;
constexpr auto LED_B_CH = LEDC_CHANNEL_2;

constexpr auto DEFAULT_NAME = "Striiiiiiip";
constexpr auto PAIRING_TIMEOUT = 60 * 1000;