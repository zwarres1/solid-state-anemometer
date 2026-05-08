#pragma once

#include <cstdint>

class ICommsBackend
{
public:
    virtual ~ICommsBackend() = default;

    // TODO: add return for error case?
    virtual void init() = 0;

    virtual void send_packet(const uint8_t* data, int length) = 0;

    virtual int read_bytes(uint8_t* buffer, int max_length) = 0;
};
