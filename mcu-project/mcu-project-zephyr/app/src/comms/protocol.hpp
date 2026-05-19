#pragma once

#include "comms_request_type.hpp"

#include <cstdint>
#include <stddef.h>

// TODO: This does not have any error recovery yet if a byte is missed
namespace protocol
{
    /// Looks for header and checksum of incoming packet, if correct header and checksum are
    /// present then determines what kind of message was requested.
    /// Returns CommsRequestType::Error if packet failed to process.
    /// TODO: figure out way to discard data if checksum fails (need fixed packet sizes)
    CommsRequestType process_incoming_bytes(const uint8_t* raw_bytes_in, size_t length_in);

    size_t build_version_packet(uint8_t* packet_out);

    size_t build_wind_speed_packet(uint16_t wind_speed, uint16_t angle, uint8_t* packet_out);
}