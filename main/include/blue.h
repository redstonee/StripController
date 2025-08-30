#pragma once

#include <functional>
#include "NimBLEAdvertising.h"
#include "config.h"

/**
    The BLE controlling interface uses a characteristic to
    set/get the color and on/off status of the LEDs.

    To set the color, write the color value in RGB888 to the
    characteristic; to turn off the LEDs, write 0 to it.

    When the LEDs are turned on/off by the button, an
    notification will be sent so that the central device
    could read the status from the characteristic.
*/
namespace blue
{
    void init();
    bool isConnected();
    void startAdvertising();

    void updateColor(uint32_t color);

    void setOnAdvCompleteCallback(advCompleteCB_t callback);
    void setOnConnectCallback(std::function<void()> callback);
    void setOnDisconnectCallback(std::function<void()> callback);
}