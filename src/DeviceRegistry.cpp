#include <DeviceRegistry.h>

DeviceRegistry::DeviceRegistry()
{
#if USE_FLASH
    DeviceRegistry::readFromFlash();
#endif
}

bool DeviceRegistry::addDevice(uint8_t deviceID, const uint8_t *macPtr)
{
    if (deviceID >= REGISTRY_ARRAY_SIZE)
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

bool DeviceRegistry::removeDevice(uint8_t deviceID)
{
    return addDevice(deviceID, BroadCastMac);
}

const uint8_t *DeviceRegistry::getDeviceMac(uint8_t deviceID) const
{
    if (deviceID >= REGISTRY_ARRAY_SIZE)
    {
        return nullptr; // ID out of bounds
    }

    if (memcmp(registry[deviceID].macData, BroadCastMac, 6) == 0)
    {
        return nullptr; // Device not registered
    }

    return registry[deviceID].macData;
}

bool DeviceRegistry::updateDeviceMac(uint8_t deviceID, const uint8_t *newMacPtr)
{
    if (deviceID >= REGISTRY_ARRAY_SIZE)
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

void DeviceRegistry::saveToFlash()
{
#if USE_FLASH
    prefs.begin(REGISTRY_NAMESPACE, false);
    prefs.putBytes(REGISTRY_KEY, (const uint8_t *)&registry, sizeof(registry));
    prefs.end();
#endif
}

void DeviceRegistry::readFromFlash()
{
#if USE_FLASH
    prefs.begin(REGISTRY_NAMESPACE, false);
    if (prefs.getBytes(REGISTRY_KEY, (uint8_t *)&registry, sizeof(registry)) == 0)
    {
    }
    prefs.end();
#endif
}