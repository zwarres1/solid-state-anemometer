#include <zephyr/ztest.h>

#include <protocol.hpp>

#include <stddef.h>

// Define the test suite
ZTEST_SUITE(comms_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(comms_suite, test_checksum_calc)
{
    uint8_t packet[] = {0x1, 0x3, 0x5};
    auto checksum = protocol::calculate_checksum(packet, 3);
    zassert_equal(checksum, 0x1 ^ 0x3 ^ 0x5);

    checksum = protocol::calculate_checksum(packet, 2);
    zassert_equal(checksum, 0x1 ^ 0x3);
}

// TODO: Need to update these tests to match enumeration return type
ZTEST(comms_suite, test_packet_happy_path)
{
    uint8_t packet[] = { 0xAA, 0x04, 0x00, 0x15, 0x01, 0x0E, 0x00 };
    
    // Calculate the expected checksum dynamically for the test
    packet[6] = protocol::calculate_checksum(packet, 6);

    // Feed it to parser
    uint8_t final_packet[5];
    const auto final_packet_size = protocol::process_incoming_bytes(packet, sizeof(packet), final_packet);

    zassert_equal(final_packet_size, 5, "Header and checksum weren't stripped");
}

ZTEST(comms_suite, test_packet_bad_checksum)
{
    // Deliberately put a wrong checksum (0xFF) at the end
    uint8_t packet[] = { 0xAA, 0x04, 0x00, 0x15, 0x01, 0x0E, 0xFF };

    // Feed it to parser
    uint8_t final_packet[5];
    const auto final_packet_size = protocol::process_incoming_bytes(packet, sizeof(packet), final_packet);

    zassert_equal(final_packet_size, 0, "Checksum check should fail");
}

ZTEST(comms_suite, test_packet_bad_header)
{
    // Deliberately put a wrong header
    uint8_t packet[] = { 0x11, 0x04, 0x00, 0x15, 0x01, 0x0E, 0x00 };
    
    // Calculate the expected checksum dynamically for the test
    packet[6] = protocol::calculate_checksum(packet, 6);

    // Feed it to parser
    uint8_t final_packet[5];
    const auto final_packet_size = protocol::process_incoming_bytes(packet, sizeof(packet), final_packet);

    zassert_equal(final_packet_size, 0, "Header check should fail");
}

ZTEST(comms_suite, test_build_packet_happy_path)
{
    uint8_t packet[] = { 0x04, 0x00, 0x15, 0x01, 0x0E };
    
    // Feed it to parser
    uint8_t final_packet[7]{};
    const auto final_packet_size = protocol::build_packet(packet, sizeof(packet), final_packet);

    zassert_equal(final_packet_size, 7, "Final packet size incorrect");
    zassert_equal(final_packet[6], 0xAA ^ 0x04 ^ 0x00 ^ 0x15 ^ 0x01 ^ 0x0E, "Invalid checksum");
}
