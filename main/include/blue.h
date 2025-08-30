#pragma once

#include <Arduino.h>

#include "config.h"

namespace blue
{
    void init();
    void deinit();
    bool isConnected();
    void setBatteryLevel(uint8_t level);

    void onKeyStatusChange(KeyID key, bool isPressed);

    void setOnConnectCallback(std::function<void()> callback);
    void setOnDisconnectCallback(std::function<void()> callback);
}