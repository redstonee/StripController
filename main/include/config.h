#pragma once
#include "stdint.h"

enum KeyID : uint8_t
{
    KEY1 = 1,
    KEY2 = 2,
    KEY3 = 3,

};
const KeyID keyIDs[] = {KEY1, KEY2, KEY3};
constexpr auto KEY_COUNT = sizeof(keyIDs);

constexpr uint8_t PIXEL_PIN = 18;
constexpr uint8_t PIXEL_COUNT = KEY_COUNT;
// constexpr uint8_t

constexpr uint8_t VBAT_SENS_PIN = 0;
constexpr float VBAT_DIV_COEFF = (10.0 + 22.0) / 22;

constexpr uint32_t BLE_PAIR_TIMEOUT = 2e4;
constexpr uint32_t BLE_RECONN_TIMEOUT = 1e4;
constexpr uint32_t IDLE_SLEEP_TIMEOUT = 3e4;
constexpr auto DEFAULT_NAME = "CV Keyboard";
