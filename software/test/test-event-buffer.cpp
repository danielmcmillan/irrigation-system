#include <unity.h>
#include "event-buffer.h"
#include "crc16.h"
#include "binary-util.h"

using namespace IrrigationSystem;

void setup()
{
}

void tearDown()
{
}

void test_event_buffer_push_pop()
{
    EventBuffer buffer;
    uint8_t type = 123;
    uint8_t payload[] = {10};
    uint8_t result[64];
    uint8_t expectedSize = 5;

    for (int i = 1; i <= 52; ++i)
    {
        uint8_t idLow = (uint8_t)i;
        uint8_t idHigh = (uint8_t)(i >> 8);
        buffer.handleEvent(type, 1, payload);
        TEST_ASSERT_EQUAL(expectedSize, buffer.peek());
        TEST_ASSERT_EQUAL(expectedSize, buffer.pop(result));
        TEST_ASSERT_EQUAL(0, buffer.peek());
        TEST_ASSERT_EQUAL(0, buffer.pop(result));
        TEST_ASSERT_EQUAL(idLow, result[0]);
        TEST_ASSERT_EQUAL(idHigh, result[1]);
        TEST_ASSERT_EQUAL(type, result[2]);
        TEST_ASSERT_EQUAL(1, result[3]); // payload size
        TEST_ASSERT_EQUAL(payload[0], result[4]);
    }
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_event_buffer_push_pop);
    UNITY_END();
}
