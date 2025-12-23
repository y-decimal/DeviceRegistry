#include <unity.h>
#include <DeviceRegistry.h>
#include <DeviceRegistry.cpp>

DeviceRegistry *registry;

void setUp(void)
{
    registry = new DeviceRegistry();
}

void tearDown(void)
{
    delete registry;
}

void test_USE_FLASH_false(void)
{
#if USE_FLASH
    TEST_FAIL_MESSAGE("USE_FLASH is true");
#else
    TEST_PASS();
#endif
}

void test_all_Macs_initially_broadcast(void)
{
    bool isBroadcast = false;
    for (int i = 0; i < REGISTRY_ARRAY_SIZE; i++)
    {
        isBroadcast = (memcmp(registry->getDeviceMac(i), BroadCastMac, 6) == 0);
        if (!isBroadcast)
        {
            break;
        }
        TEST_ASSERT_TRUE(isBroadcast);
    }
}

void test_getDeviceMac_found(void)
{
    uint8_t testMac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t deviceID = 194;
    registry->addDevice(deviceID, testMac);

    const uint8_t *returnedMac = registry->getDeviceMac(deviceID);
    TEST_ASSERT_NOT_NULL(returnedMac);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testMac, returnedMac, 6);
}

void test_getDeviceMac_not_found(void)
{
    const uint8_t *returnedMac = registry->getDeviceMac(86);
    TEST_ASSERT_NULL(returnedMac);
    if (returnedMac == nullptr)
    {
        return;
    }
    if (memcmp(returnedMac, BroadCastMac, 6) == 0)
    {
        TEST_FAIL_MESSAGE("Returned Broadcast MAC");
    }
}

void test_getUpdateDeviceMac_found(void)
{
    uint8_t initialMac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    uint8_t newMac[6] = {0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB};
    uint8_t deviceID = 31;

    bool addResult = registry->addDevice(deviceID, initialMac);
    TEST_ASSERT_TRUE(addResult);
    const uint8_t *returnedMac = registry->getDeviceMac(deviceID);
    TEST_ASSERT_NOT_NULL(returnedMac);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(initialMac, returnedMac, 6);
    bool updateResult = registry->updateDeviceMac(deviceID, newMac);
    TEST_ASSERT_TRUE(updateResult);

    returnedMac = registry->getDeviceMac(deviceID);
    TEST_ASSERT_NOT_NULL(returnedMac);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(newMac, returnedMac, 6);
}

void test_getUpdateDeviceMac_not_found(void)
{
    uint8_t newMac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
    uint8_t deviceID = 64;

    bool updateResult = registry->updateDeviceMac(deviceID, newMac);
    TEST_ASSERT_FALSE(updateResult);
}

void test_getDeviceMac_multiple_devices(void)
{
    uint8_t mac1[6] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};
    uint8_t mac2[6] = {0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0};
    uint8_t deviceID1 = 23;
    uint8_t deviceID2 = 24;

    registry->addDevice(deviceID1, mac1);
    registry->addDevice(deviceID2, mac2);

    const uint8_t *returnedMac1 = registry->getDeviceMac(deviceID1);
    TEST_ASSERT_NOT_NULL(returnedMac1);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(mac1, returnedMac1, 6);

    const uint8_t *returnedMac2 = registry->getDeviceMac(deviceID2);
    TEST_ASSERT_NOT_NULL(returnedMac2);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(mac2, returnedMac2, 6);
}

void test_removeDevice_found(void)
{
    uint8_t testMac[6] = {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56};
    uint8_t deviceID = 75;

    registry->addDevice(deviceID, testMac);
    bool removeResult = registry->removeDevice(deviceID);
    TEST_ASSERT_TRUE(removeResult);

    const uint8_t *returnedMac = registry->getDeviceMac(deviceID);
    TEST_ASSERT_NULL(returnedMac);
}

void test_removeDevice_not_found(void)
{
    uint8_t deviceID = 82;

    bool removeResult = registry->removeDevice(deviceID);
    TEST_ASSERT_FALSE(removeResult);
}

void setup()
{
    delay(2000); // Wait for serial monitor
    UNITY_BEGIN();
    RUN_TEST(test_USE_FLASH_false);
    RUN_TEST(test_all_Macs_initially_broadcast);
    RUN_TEST(test_getDeviceMac_found);
    RUN_TEST(test_getDeviceMac_not_found);
    RUN_TEST(test_getUpdateDeviceMac_found);
    RUN_TEST(test_getUpdateDeviceMac_not_found);
    RUN_TEST(test_getDeviceMac_multiple_devices);
    RUN_TEST(test_removeDevice_found);
    RUN_TEST(test_removeDevice_not_found);
    UNITY_END();
}

void loop()
{
    // Empty loop
}