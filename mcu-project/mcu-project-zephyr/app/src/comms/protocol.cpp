#include "protocol.hpp"

#include <zephyr/sys/byteorder.h>

#include <string.h>

// TODO: add to cmake, generate own version file
constexpr uint8_t VERSION_MAJOR = 1;
constexpr uint8_t VERSION_MINOR = 0;
constexpr uint8_t VERSION_PATCH = 0;

// TODO: Move structs to their own files??
struct __attribute__((packed)) FirmwareVersionPacket
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
};

constexpr FirmwareVersionPacket FIRMWARE_VERSION = {VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH};

struct __attribute__((packed)) WindSpeedPacket
{
    uint16_t wind_speed;
    uint16_t direction;
};

uint8_t calculate_checksum(const uint8_t* payload, size_t length)
{
    uint8_t checksum = 0;
    for(size_t i = 0; i < length; i++)
    {
        checksum ^= payload[i];
    }
    return checksum;
}

int build_packet(const uint8_t* raw_bytes_in, size_t length_in, uint8_t* packet_out)
{
    int final_packet_size = length_in + 2;
    packet_out[0] = 0xAA;
    memcpy(&packet_out[1], raw_bytes_in, length_in);

    const auto checksum = calculate_checksum(packet_out, final_packet_size - 1);
    packet_out[final_packet_size - 1] = checksum;
    return final_packet_size;
}

CommsRequestType protocol::process_incoming_bytes(const uint8_t* raw_bytes_in, size_t length_in)
{
    // Must have at least header and checksum
    if(length_in < 2)
    {
        return CommsRequestType::Error;
    }    
    
    // Must have header
    if(raw_bytes_in[0] != 0xAA)
    {
        return CommsRequestType::Error;
    }

    const auto checksum = calculate_checksum(raw_bytes_in, length_in - 1);
    if(raw_bytes_in[length_in - 1] != checksum)
    {
        return CommsRequestType::Error;
    }

    const auto command_id = raw_bytes_in[1];
    switch(command_id)
    {
        case static_cast<uint8_t>(CommsRequestType::WindSpeed):
            return CommsRequestType::WindSpeed;
        case static_cast<uint8_t>(CommsRequestType::Version):
            return CommsRequestType::Version;
        default:
            return CommsRequestType::Error;
    }
}

size_t protocol::build_version_packet(uint8_t* packet_out)
{
    return build_packet(reinterpret_cast<const uint8_t*>(&FIRMWARE_VERSION), sizeof(FIRMWARE_VERSION), packet_out);
}

size_t protocol::build_wind_speed_packet(uint16_t wind_speed, uint16_t angle, uint8_t* packet_out)
{
    const WindSpeedPacket wind_speed_packet_be{
        sys_cpu_to_be16(wind_speed),
        sys_cpu_to_be16(angle)};

    return build_packet(reinterpret_cast<const uint8_t*>(&wind_speed_packet_be), sizeof(wind_speed_packet_be), packet_out);
}