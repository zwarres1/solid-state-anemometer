#[cfg(any(target_arch = "arm", target_arch = "aarch64"))]
pub mod uart;

#[cfg(not(any(target_arch = "arm", target_arch = "aarch64")))]
pub mod mock;

pub mod protocol;
use protocol::{create_packet, strip_packet};
use anyhow::{Context, Result};
use std::io::{Read, Write};
use std::{thread, time};

pub struct Communication<T> {
    transport: T,
}

impl<T: Read + Write> Communication<T> {
    pub fn new(transport: T) -> Self {
        Self { transport }
    }

    /// Sends a simple patterened message to the MCU and prints the result
    pub fn poke_comms(&mut self) -> Result<()> {
        println!("Sending test message");
        let test_data: [u8; 4] = [0x1, 0x2, 0x3, 0x4];
        let mut test_packet: [u8; 6] = [0; 6];
        create_packet(&test_data, &mut test_packet)?;

        self.transport.write(&test_packet)?;
        thread::sleep(time::Duration::from_millis(500));
        println!("Reading back message");
        let mut test_read_data: [u8; 6] = [0; 6];
        self.transport.read(&mut test_read_data)?;
        let read_payload = strip_packet(&test_read_data).with_context(|| "Failed to strip packet")?;
        println!("Message read: {:?}", read_payload);
        Ok(())
    }

    // TODO: build this from message IDs and go into structures
    /// Sends the wind speed packet and prints the result
    pub fn request_wind_speed(&mut self) -> Result<()> {
        println!("Requesting wind speed");
        // Message ID 2 for wind speed (reserving ID 1 for version information)
        let test_data: [u8; 1] = [0x2];
        let mut test_packet: [u8; 3] = [0; 3];
        create_packet(&test_data, &mut test_packet)?;

        self.transport.write(&test_packet)?;
        thread::sleep(time::Duration::from_millis(500));
        println!("Reading wind speed");
        let mut test_read_data: [u8; 6] = [0; 6];
        self.transport.read(&mut test_read_data)?;
        let read_payload = strip_packet(&test_read_data).with_context(|| "Failed to strip packet")?;
        println!("Message read: {:?}", read_payload);
        Ok(())
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use crate::comms::mock::MockComms;

    #[test]
    fn test_loopback_logic() {
        let mock_protocol = MockComms::new();
        let mut mock_comms = Communication::new(mock_protocol);
        let test_payload: [u8; 5] = [0x1, 0x2, 0x3, 0x4, 0x5];

        mock_comms.transport.write(&test_payload).expect("Send failed");
        let mut test_read_data: [u8; 5] = [0; 5];
        mock_comms.transport.read(&mut test_read_data).expect("Read failed");
        assert_eq!(test_read_data, test_payload, "Loopback failed: Data mismatch!");
    }
}