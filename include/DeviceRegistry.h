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

#define REGISTRY_TEMPLATE template <typename DeviceID, size_t Count>
#define REGISTRY_PARAMS DeviceRegistry<DeviceID, Count>

using MacArray = uint8_t[6];
constexpr uint8_t BroadCastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

REGISTRY_TEMPLATE
class DeviceRegistry
{

public:
    DeviceRegistry();

    bool addDevice(DeviceID deviceID, const uint8_t *macPtr);
    bool removeDevice(DeviceID deviceID);

    const uint8_t *getDeviceMac(DeviceID deviceID) const;
    bool updateDeviceMac(DeviceID deviceID, const uint8_t *newMacPtr);

    void saveToFlash();

#ifdef UNIT_TEST
    void readFromFlash();
#endif

private:
    struct MacEntry
    {
        uint8_t macData[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    };

    MacEntry registry[Count]{};

    Preferences prefs;

    static constexpr const char *REGISTRY_NAMESPACE = "dReg";
    static constexpr const char *REGISTRY_KEY = "val";

    static constexpr size_t toIndex(DeviceID id);

#ifndef UNIT_TEST
    void readFromFlash();
#endif
};

// Full definitions

// Template implementation
REGISTRY_TEMPLATE
REGISTRY_PARAMS::DeviceRegistry()
{
#if USE_FLASH
    DeviceRegistry::readFromFlash();
#endif
}

REGISTRY_TEMPLATE
bool REGISTRY_PARAMS::addDevice(DeviceID deviceID, const uint8_t *macPtr)
{
    size_t index = toIndex(deviceID);
    if (index >= Count)
    {
        return false; // ID out of bounds
    }

    if (!memcmp(registry[index].macData, BroadCastMac, 6) == 0)
    // memcmp returns 0 if macData = BroadCastMac
    // we invert to check if macData != BroadCastMac
    {
        return false; // Device already exists
    }

    memcpy(registry[index].macData, macPtr, 6);

    if (!memcmp(registry[index].macData, macPtr, 6) == 0)
    // memcmp returns 0 if macData = macPtr
    // we invert to check if macData != macPtr
    {
        return false; // Copying failed
    }

    return true;
}

REGISTRY_TEMPLATE
bool REGISTRY_PARAMS::removeDevice(DeviceID deviceID)
{
    return updateDeviceMac(deviceID, BroadCastMac);
}

REGISTRY_TEMPLATE
const uint8_t *REGISTRY_PARAMS::getDeviceMac(DeviceID deviceID) const
{
    size_t index = toIndex(deviceID);
    if (index >= Count)
    {
        return nullptr; // ID out of bounds
    }

    if (memcmp(registry[index].macData, BroadCastMac, 6) == 0)
    {
        return nullptr; // Device not registered
    }

    return registry[index].macData;
}

REGISTRY_TEMPLATE
bool REGISTRY_PARAMS::updateDeviceMac(DeviceID deviceID, const uint8_t *newMacPtr)
{
    size_t index = toIndex(deviceID);

    if (index >= Count)
    {
        return false; // ID out of bounds
    }

    if (memcmp(registry[index].macData, BroadCastMac, 6) == 0)
    {
        return false; // Device not registered
    }

    memcpy(registry[index].macData, newMacPtr, 6);
    return true;
}

REGISTRY_TEMPLATE
void REGISTRY_PARAMS::saveToFlash()
{
#if USE_FLASH
    prefs.begin(REGISTRY_NAMESPACE, false);
    prefs.putBytes(REGISTRY_KEY, (const uint8_t *)&registry, sizeof(registry));
    prefs.end();
#endif
}

REGISTRY_TEMPLATE
void REGISTRY_PARAMS::readFromFlash()
{
#if USE_FLASH
    prefs.begin(REGISTRY_NAMESPACE, true);
    if (prefs.getBytes(REGISTRY_KEY, (uint8_t *)&registry, sizeof(registry)) == 0)
    {
    }
    prefs.end();
#endif
}

REGISTRY_TEMPLATE
constexpr size_t REGISTRY_PARAMS::toIndex(DeviceID deviceID)
{
    return static_cast<size_t>(deviceID);
}

#endif