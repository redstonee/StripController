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
            pServer->startAdvertising();
            if (onDisconnectCallback)
                onDisconnectCallback();
            ESP_LOGI("BLE", "Disconnected");
        }
    };

    class CharCallbacks : public NimBLECharacteristicCallbacks
    {
        void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
        {
            auto value = pCharacteristic->getValue();
            if (value.length() > 4)
            {
                ESP_LOGW("BLE", "Invalid data length %d", value.length());
                return;
            }

            uint32_t colorRecved = 0;
            for (auto i = 0; i < value.length(); i++)
            {
                colorRecved |= (static_cast<uint32_t>(value[i]) << (i * 8));
            }
            xQueueSend(colorConfigQueue, &colorRecved, 10);
        }
    };

    bool isConnected()
    {
        return bleServer->getConnectedCount();
    }

    void startAdvertising()
    {
        if (!bleServer->getAdvertising()->isAdvertising())
            bleServer->startAdvertising();
    }

    void init()
    {
        const char *myShitServiceUUID = "1145";
        const char *myFuckingCharUUID = "2333";

        NimBLEDevice::init(DEFAULT_NAME);

        bleServer = NimBLEDevice::createServer();
        bleServer->setCallbacks(new SvrCallbacks());

        auto myShitService = bleServer->createService(myShitServiceUUID);
        myFuckingChar = myShitService->createCharacteristic(myFuckingCharUUID, WRITE | READ | NOTIFY);
        myFuckingChar->setCallbacks(new CharCallbacks());
        myShitService->start();

        bleServer->getAdvertising()->setName(DEFAULT_NAME);
        bleServer->getAdvertising()->addServiceUUID(myShitService->getUUID());
        bleServer->addService(myShitService);
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
