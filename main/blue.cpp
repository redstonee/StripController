#include <memory>
#include <NimBLEDevice.h>
#include <NimBLECharacteristic.h>
#include <Preferences.h>
#include <unordered_map>

#include "blue.h"

#include "config.h"

namespace blue
{
    static Preferences pref;
    static NimBLEServer *pServer;

    static std::function<void()> onConnectCallback;
    static std::function<void()> onDisconnectCallback;

    class SvrCallbacks : public NimBLEServerCallbacks
    {
        void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo)
        {
            pServer->stopAdvertising();
            if (onConnectCallback)
                onConnectCallback();
            ESP_LOGI("BLE", "Connected");
        }

        void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason)
        {
            pServer->startAdvertising();
            if (onDisconnectCallback)
                onDisconnectCallback();
            ESP_LOGI("BLE", "Disconnected");
        }
    };

    class RenameCharCallbacks : public NimBLECharacteristicCallbacks
    {
        void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
        {
            std::string rxValue = pCharacteristic->getValue();

            if (rxValue.length() > 0)
            {
                printf("Received: %s\n", rxValue.c_str());
                pref.putString("name", rxValue.c_str());
            }
        }
    };

    bool isConnected()
    {
        return pServer->getConnectedCount();
        return false;
    }

    void init()
    {

        const char *myShitServiceUUID = "4fafc201-0000-1145-1400-c5c9c331914b";
        const char *renameCharaUUID = "beb5483e-0000-1145-1400-696969696969";

        pref.begin("keyboard", false);

        auto devName = pref.getString("name", DEFAULT_NAME);

        NimBLEDevice::init(devName.c_str());

        if (!pServer) // The first time to initialize
        {
            pServer = NimBLEDevice::createServer();
            pServer->setCallbacks(new SvrCallbacks());

            auto pMyShitService = pServer->createService(myShitServiceUUID);
            auto pRenameChara = pMyShitService->createCharacteristic(renameCharaUUID, NIMBLE_PROPERTY::WRITE_NR);
            pRenameChara->setCallbacks(new RenameCharCallbacks());
            pMyShitService->start();
        }

        // Start advertising
        auto pAdvertising = pServer->getAdvertising();
        pAdvertising->start();
    }

    void deinit()
    {
        pref.end();
        NimBLEDevice::deinit();
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
