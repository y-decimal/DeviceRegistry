#ifndef DEVICEREGISTRY_H
#define DEVICEREGISTRY_H

#ifdef UNIT_TEST
#ifdef TEST_WITH_FLASH
#define USE_FLASH true
#else
#define USE_FLASH false
#endif
#else
#define USE_FLASH true
#endif

#include <array>
#include <map>
#include <Preferences.h>

using MacArray = uint8_t[6];
constexpr uint8_t REGISTRY_ARRAY_SIZE = 251;
constexpr uint8_t BroadCastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// First 251 IDs (0-250) used for devices, (251-255) reserved for error handling

struct MacEntry
{
    uint8_t macData[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};

template <typename DeviceID, size_t Count>
class DeviceRegistry
{

public:
    DeviceRegistry();

    bool addDevice(uint8_t deviceID, const uint8_t *macPtr);
    bool removeDevice(uint8_t deviceID);

    const uint8_t *getDeviceMac(uint8_t deviceID) const;
    bool updateDeviceMac(uint8_t deviceID, const uint8_t *newMacPtr);

    void saveToFlash();

#ifdef UNIT_TEST
    void readFromFlash();
#endif

private:
    MacEntry registry[REGISTRY_ARRAY_SIZE]{};

    Preferences prefs;

    static constexpr const char *REGISTRY_NAMESPACE = "dReg";
    static constexpr const char *REGISTRY_KEY = "val";

    static constexpr size_t toIndex(DeviceID id);

#ifndef UNIT_TEST
    void readFromFlash();
#endif
};

template <typename DeviceID, size_t Count>
DeviceRegistry<DeviceID, Count>::DeviceRegistry()
{
#if USE_FLASH
    DeviceRegistry::readFromFlash();
#endif
}

template <typename DeviceID, size_t Count>
bool DeviceRegistry<DeviceID, Count>::addDevice(uint8_t deviceID, const uint8_t *macPtr)
{
    deviceID = toIndex(deviceID);
    if (deviceID >= toIndex(Count))
    {
        return false; // ID out of bounds
    }

    if (memcmp(registry[deviceID].macData, BroadCastMac, 6) == 0)
    {
        return false; // Device already exists
    }

    memcpy(registry[deviceID].macData, macPtr, 6);

    if (memcmp(registry[deviceID].macData, macPtr, 6) != 0)
    {
        return false; // Copying failed
    }

    return true;
}

template <typename DeviceID, size_t Count>
bool DeviceRegistry<DeviceID, Count>::removeDevice(uint8_t deviceID)
{
    return addDevice(deviceID, BroadCastMac);
}

template <typename DeviceID, size_t Count>
const uint8_t *DeviceRegistry<DeviceID, Count>::getDeviceMac(uint8_t deviceID) const
{

    deviceID = toIndex(deviceID);
    if (deviceID >= deviceID(Count))
    {
        return nullptr; // ID out of bounds
    }

    if (memcmp(registry[deviceID].macData, BroadCastMac, 6) == 0)
    {
        return nullptr; // Device not registered
    }

    return registry[deviceID].macData;
}

template <typename DeviceID, size_t Count>
bool DeviceRegistry<DeviceID, Count>::updateDeviceMac(uint8_t deviceID, const uint8_t *newMacPtr)
{
    deviceID = toIndex(deviceID);

    if (deviceID >= toIndex(Count))
    {
        return false; // ID out of bounds
    }

    if (memcmp(registry[deviceID].macData, BroadCastMac, 6) == 0)
    {
        return false; // Device not registered
    }

    memcpy(registry[deviceID].macData, newMacPtr, 6);
    return true;
}

template <typename DeviceID, size_t Count>
void DeviceRegistry<DeviceID, Count>::saveToFlash()
{
#if USE_FLASH
    prefs.begin(REGISTRY_NAMESPACE, false);
    prefs.putBytes(REGISTRY_KEY, (const uint8_t *)&registry, sizeof(registry));
    prefs.end();
#endif
}

template <typename DeviceID, size_t Count>
void DeviceRegistry<DeviceID, Count>::readFromFlash()
{
#if USE_FLASH
    prefs.begin(REGISTRY_NAMESPACE, false);
    if (prefs.getBytes(REGISTRY_KEY, (uint8_t *)&registry, sizeof(registry)) == 0)
    {
    }
    prefs.end();
}
#endif

template <typename DeviceID, size_t Count>
constexpr size_t DeviceRegistry<DeviceID, Count>::
    toIndex(DeviceID deviceID)
{
    return static_cast<size_t>(deviceID);
}

#endif