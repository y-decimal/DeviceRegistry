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

#include <Preferences.h>
#include <array>
#include <map>
#include <type_traits>

#define REGISTRY_TEMPLATE template <typename UniqueID>
#define REGISTRY_PARAMS DeviceRegistry<UniqueID>

using MacArray = uint8_t[6];
constexpr uint8_t BroadCastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

REGISTRY_TEMPLATE
class DeviceRegistry {

public:
  DeviceRegistry(UniqueID selfID, const uint8_t *selfMacPtr);

  bool addDevice(UniqueID targetID, const uint8_t *macPtr);
  bool removeDevice(UniqueID targetID);

  const uint8_t *getDeviceMac(UniqueID targetID) const;
  bool updateDeviceMac(UniqueID targetID, const uint8_t *newMacPtr);

  void saveToFlash();
  void deleteFlash();

#ifdef UNIT_TEST
  void readFromFlash();
#endif

private:
  struct MacEntry {
    uint8_t macData[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  };

  // Compile-time constraints for UniqueID
  static_assert(std::is_enum<UniqueID>::value, "UniqueID must be an enum type");
  static_assert(std::is_same<typename std::underlying_type<UniqueID>::type,
                             uint8_t>::value,
                "UniqueID underlying type must be uint8_t");

  static constexpr const char *REGISTRY_NAMESPACE = "dReg";
  static constexpr const char *REGISTRY_KEY = "val";
  static constexpr size_t Count = static_cast<size_t>(UniqueID::Count);
  UniqueID selfID;
  MacEntry selfMac;

  MacEntry registry[Count]{};

  Preferences prefs;

  static constexpr size_t toIndex(UniqueID id);

#ifndef UNIT_TEST
  void readFromFlash();
#endif
};

// Full definitions

// Template implementation
REGISTRY_TEMPLATE
REGISTRY_PARAMS::DeviceRegistry(UniqueID selfID, const uint8_t *selfMacPtr) {
  this->selfID = selfID;
  memcpy(this->selfMac.macData, selfMacPtr, 6);
#if USE_FLASH
  DeviceRegistry::readFromFlash();
#endif
}

REGISTRY_TEMPLATE
bool REGISTRY_PARAMS::addDevice(UniqueID targetID, const uint8_t *macPtr) {
  size_t index = toIndex(targetID);
  if (index >= Count) {
    printf("[DeviceRegistry] Device ID out of bounds: %d\n", index);
    return false; // ID out of bounds
  }

  if (targetID == selfID) {
    printf("[DeviceRegistry] Cannot add self device ID: %d\n", index);
    return false; // Cannot add self
  }
  if (memcmp(macPtr, selfMac.macData, 6) == 0) {
    printf("[DeviceRegistry] Cannot add self MAC for device ID: %d\n", index);
    return false; // Cannot add self MAC
  }

  if (!memcmp(registry[index].macData, BroadCastMac, 6) == 0)
  // memcmp returns 0 if macData = BroadCastMac
  // we invert to check if macData != BroadCastMac
  {
    printf("[DeviceRegistry] Device already exists: %d\n", index);
    return false; // Device already exists
  }

  memcpy(registry[index].macData, macPtr, 6);

  if (!memcmp(registry[index].macData, macPtr, 6) == 0)
  // memcmp returns 0 if macData = macPtr
  // we invert to check if macData != macPtr
  {
    printf("[DeviceRegistry] Copying MAC failed for device ID: %d\n", index);
    return false; // Copying failed
  }

  return true;
}

REGISTRY_TEMPLATE
bool REGISTRY_PARAMS::removeDevice(UniqueID targetID) {
  return updateDeviceMac(targetID, BroadCastMac);
}

REGISTRY_TEMPLATE
const uint8_t *REGISTRY_PARAMS::getDeviceMac(UniqueID targetID) const {
  size_t index = toIndex(targetID);
  if (index >= Count) {
    printf("[DeviceRegistry] Device ID out of bounds: %d\n", index);
    return nullptr; // ID out of bounds
  }

  if (targetID == selfID) {
    printf("[DeviceRegistry] Returning self MAC for self device ID: %d\n",
           index);
    return selfMac.macData; // Return self MAC
  }

  if (memcmp(registry[index].macData, BroadCastMac, 6) == 0) {
    printf("[DeviceRegistry] Device not registered: %d\n", index);
    return nullptr; // Device not registered
  }

  return registry[index].macData;
}

REGISTRY_TEMPLATE
bool REGISTRY_PARAMS::updateDeviceMac(UniqueID targetID,
                                      const uint8_t *newMacPtr) {
  size_t index = toIndex(targetID);

  if (index >= Count) {
    printf("[DeviceRegistry] Device ID out of bounds: %d\n", index);
    return false; // ID out of bounds
  }
  if (targetID == selfID) {
    printf("[DeviceRegistry] Cannot update self device ID: %d\n", index);
    return false; // Cannot update self
  }
  if (memcmp(newMacPtr, selfMac.macData, 6) == 0) {
    printf("[DeviceRegistry] Cannot set self MAC for device ID: %d\n", index);
    return false; // Cannot set self MAC
  }

  if (memcmp(registry[index].macData, BroadCastMac, 6) == 0) {
    printf("[DeviceRegistry] Device not registered: %d\n", index);
    return false; // Device not registered
  }

  memcpy(registry[index].macData, newMacPtr, 6);
  return true;
}

REGISTRY_TEMPLATE
void REGISTRY_PARAMS::saveToFlash() {
#if USE_FLASH
  prefs.begin(REGISTRY_NAMESPACE, false);
  prefs.putBytes(REGISTRY_KEY, (const uint8_t *)&registry, sizeof(registry));
  prefs.end();
#endif
}

REGISTRY_TEMPLATE
void REGISTRY_PARAMS::readFromFlash() {
#if USE_FLASH
  prefs.begin(REGISTRY_NAMESPACE, false);
  // Check if key exists before reading to avoid error messages
  if (prefs.isKey(REGISTRY_KEY)) {
    prefs.getBytes(REGISTRY_KEY, (uint8_t *)&registry, sizeof(registry));
  }
  prefs.end();
#endif
}

REGISTRY_TEMPLATE
void REGISTRY_PARAMS::deleteFlash() {
#if USE_FLASH
  prefs.begin(REGISTRY_NAMESPACE, false);
  if (prefs.isKey(REGISTRY_KEY)) {
    prefs.remove(REGISTRY_KEY);
  }
  prefs.end();
#endif
}

REGISTRY_TEMPLATE
constexpr size_t REGISTRY_PARAMS::toIndex(UniqueID targetID) {
  return static_cast<size_t>(targetID);
}

#endif