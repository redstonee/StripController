#include <memory>
#include <NimBLEDevice.h>
#include <NimBLECharacteristic.h>

#include "blue.h"

#include "config.h"

// Global variables, use with caution
extern SemaphoreHandle_t colorConfigQueue;

namespace blue
{
    static NimBLEServer *bleServer;
    static NimBLECharacteristic *myFuckingChar;

    static std::function<void()> onConnectCallback;
    static std::function<void()> onDisconnectCallback;

    class SvrCallbacks : public NimBLEServerCallbacks
    {
        void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
        {
            pServer->stopAdvertising();
            if (onConnectCallback)
                onConnectCallback();
            ESP_LOGI("BLE", "Connected");
        }

        void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override
        {
            if (onDisconnectCallback)
                onDisconnectCallback();
            ESP_LOGI("BLE", "Disconnected");
        }
    };

    class CharCallbacks : public NimBLECharacteristicCallbacks
    {
        void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
        {
            auto val = pCharacteristic->getValue().data();
            auto colorRecved = reinterpret_cast<const uint32_t *>(val);
            xQueueSend(colorConfigQueue, colorRecved, 10);
        }
    };

    bool isConnected()
    {
        return bleServer->getConnectedCount();
    }

    void startAdvertising()
    {
        if (!bleServer->getAdvertising()->isAdvertising())
            bleServer->startAdvertising(PAIRING_TIMEOUT);
    }

    void init()
    {
        const char *myShitServiceUUID = "4fafc201-0000-1145-1400-c5c9c331914b";
        const char *myFuckingCharUUID = "2333";

        NimBLEDevice::init(DEFAULT_NAME);

        bleServer = NimBLEDevice::createServer();
        bleServer->setCallbacks(new SvrCallbacks());

        auto myShitService = bleServer->createService(myShitServiceUUID);
        myFuckingChar = myShitService->createCharacteristic(myFuckingCharUUID, WRITE | READ | NOTIFY);
        myFuckingChar->setCallbacks(new CharCallbacks());
        myShitService->start();
    }

    void updateColor(uint32_t color)
    {
        myFuckingChar->setValue(color);
        if (isConnected())
            myFuckingChar->notify();
    }

    void setOnAdvCompleteCallback(advCompleteCB_t callback)
    {
        bleServer->getAdvertising()->setAdvertisingCompleteCallback(callback);
    }

    void setOnConnectCallback(std::function<void()> callback)
    {
        onConnectCallback = callback;
    }

    void setOnDisconnectCallback(std::function<void()> callback)
    {
        onDisconnectCallback = callback;
    }

} // namespace blue
