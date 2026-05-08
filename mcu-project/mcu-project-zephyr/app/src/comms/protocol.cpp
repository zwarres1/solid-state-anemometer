#include "protocol.hpp"

#include <string.h>

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

    if(raw_bytes_in[1] == static_cast<uint8_t>(CommsRequestType::WindSpeed))
    {
        return CommsRequestType::WindSpeed;
    }
    return CommsRequestType::Error;
}

int protocol::build_packet(const uint8_t* raw_bytes_in, size_t length_in, uint8_t* packet_out)
{
    int final_packet_size = length_in + 2;
    packet_out[0] = 0xAA;
    memcpy(&packet_out[1], raw_bytes_in, length_in);

    const auto checksum = calculate_checksum(packet_out, final_packet_size - 1);
    packet_out[final_packet_size - 1] = checksum;
    return final_packet_size;
}

uint8_t protocol::calculate_checksum(const uint8_t* payload, size_t length)
{
    uint8_t checksum = 0;
    for(size_t i = 0; i < length; i++)
    {
        checksum ^= payload[i];
    }
    return checksum;
}
