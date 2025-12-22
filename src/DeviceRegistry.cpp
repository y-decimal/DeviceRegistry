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

    if (memcmp(registry[deviceID], BroadCastMac, 6) == 0)
    {
        return false; // Device already exists
    }

    memcpy(registry[deviceID], macPtr, 6);

    if (memcmp(registry[deviceID], macPtr, 6) != 0)
    {
        return false; // Copying failed
    }

    return true;
}

bool DeviceRegistry::removeDevice(uint8_t deviceID)
{
    if (addDevice(deviceID, BroadCastMac) == true)
    {
        return true;
    }
    return false;
}

const uint8_t *DeviceRegistry::getDeviceMac(uint8_t deviceID) const
{
    if (deviceID >= REGISTRY_ARRAY_SIZE)
    {
        return nullptr; // ID out of bounds
    }

    if (memcmp(registry[deviceID], BroadCastMac, 6) == 0)
    {
        return nullptr; // Device not registered
    }

    return registry[deviceID];
}

bool DeviceRegistry::updateDeviceMac(uint8_t deviceID, const uint8_t *newMacPtr)
{
    if (deviceID >= REGISTRY_ARRAY_SIZE)
    {
        return false; // ID out of bounds
    }

    if (memcmp(registry[deviceID], BroadCastMac, 6) == 0)
    {
        return false; // Device not registered
    }

    memcpy(registry[deviceID], newMacPtr, 6);
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
        for (uint8_t iD = 0; iD <= REGISTRY_ARRAY_SIZE; iD++)
        {
            memcpy(registry[iD], BroadCastMac, 6);
        }
    }
    prefs.end();
#endif
}