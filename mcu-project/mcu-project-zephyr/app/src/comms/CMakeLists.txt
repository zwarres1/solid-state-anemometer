target_sources(app PRIVATE
    backend_uart.cpp
    comms_thread.cpp
    protocol.cpp
)

target_include_directories(app PRIVATE .)