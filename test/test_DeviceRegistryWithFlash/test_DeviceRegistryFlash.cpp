#include <DeviceRegistry.h>
#include <unity.h>

enum class DeviceID : uint8_t {
  TestDevice1,
  TestDevice2,
  TestDevice3,
  TestDevice4,
  TestDeviceSelf,
  Count
};

DeviceRegistry<DeviceID> *registry;
Preferences unitPrefs;

void setUp(void) {
  unitPrefs.begin("dReg", false);
  unitPrefs.clear();
  unitPrefs.end();
  registry = new DeviceRegistry<DeviceID>(
      DeviceID::TestDeviceSelf,
      (const uint8_t[]){0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00});
}

void tearDown(void) { delete registry; }

bool check_FLASH_empty() {
  unitPrefs.begin("dReg", false);
  size_t len = unitPrefs.getBytesLength("val");
  unitPrefs.end();
  delay(100); // Ensure flash operations complete
  return len == 0;
}

bool check_FLASH_not_empty() {
  unitPrefs.begin("dReg", false);
  size_t len = unitPrefs.getBytesLength("val");
  unitPrefs.end();
  delay(100); // Ensure flash operations complete
  return len > 0;
}

void test_USE_FLASH_true(void) {
#if USE_FLASH
  TEST_PASS();
#else
  TEST_FAIL_MESSAGE("USE_FLASH is not true");
#endif
}

void test_Registry_flash_initially_empty(void) {
  TEST_ASSERT_TRUE(check_FLASH_empty());
}

void test_Registry_flash_not_empty_after_save(void) {
  uint8_t testMac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
  DeviceID deviceID = DeviceID::TestDevice2;
  bool success = registry->addDevice(deviceID, testMac);
  TEST_ASSERT_TRUE(success);

  registry->saveToFlash();

  TEST_ASSERT_TRUE(check_FLASH_not_empty());
}

void test_Registry_still_empty_after_test(void) {
  TEST_ASSERT_TRUE(check_FLASH_empty());
}

void setup() {
  delay(2000); // Wait for serial monitor
  UNITY_BEGIN();
  RUN_TEST(test_USE_FLASH_true);
  RUN_TEST(test_Registry_flash_initially_empty);
  RUN_TEST(test_Registry_flash_not_empty_after_save);
  RUN_TEST(test_Registry_still_empty_after_test);
  UNITY_END();
}

void loop() {}