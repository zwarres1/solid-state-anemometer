use anyhow::Result;

/// Pads a given packet with 0xAA at the start of the packet and checksum at the end of the packet
pub fn create_packet(payload: &[u8], buffer: &mut [u8]) -> Result<usize> {
    let buffer_size_needed = payload.len() + 2; // 1 for header, 1 for footer
    if buffer.len() < buffer_size_needed {
        return Err(anyhow::anyhow!("Buffer size too small"));
    }
    
    buffer[0] = 0xAA; // Start Byte
    buffer[1..1+payload.len()].copy_from_slice(payload);

    let checksum_index = 1 + payload.len();
    buffer[checksum_index] = calculate_checksum(&buffer[0..checksum_index]);
    Ok(buffer_size_needed)
}

/// Strips out the starting and ending bytes (0xAA and checksum) of a message, returning
/// the remaining data in a vector of bytes, if the header is missing or checksum is invalid
/// then it returns Error
pub fn strip_packet(packet: &[u8]) -> Result<&[u8]> {
    if packet.len() < 2 {
        return Err(anyhow::anyhow!("Packet too short"));
    }
    if packet[0] != 0xAA {
        return Err(anyhow::anyhow!("Invalid Header"));
    }
    let checksum = calculate_checksum(&packet[0..packet.len() - 1]);
    // TODO: should be a checksum check
    if packet[packet.len() - 1] != checksum {
        return Err(anyhow::anyhow!("Invalid Footer"));
    }

    Ok(&packet[1..packet.len()-1])
}

/// Calculates a checksum by XORing each byte in the payload
fn calculate_checksum(payload: &[u8]) -> u8 {
    payload.iter().fold(0, |acc, &byte| acc ^ byte)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_checksum_logic() {
        let test_data: [u8; 4] = [0x1, 0x2, 0x3, 0x4];
        let calculated_checksum = calculate_checksum(&test_data);
        assert_eq!(calculated_checksum, 0x1 ^ 0x2 ^ 0x3 ^ 0x4, "Invalid checksum calculation");
    }

    #[test]
    fn test_create_packet() {
        let payload: [u8; 5] = [0x04, 0x00, 0x15, 0x01, 0x0E]; 
        let mut buffer: [u8; 64] = [0; 64];

        let total_size = create_packet(&payload, &mut buffer).expect("Failed to create packet.");
        assert_eq!(total_size, 7, "Packet size is incorrect");

        assert_eq!(buffer[0], 0xAA, "Missing 0xAA header byte");
        assert_eq!(&buffer[1..6], &payload[..], "Payload was not copied correctly");
        let expected_checksum = 0xAA ^ 0x04 ^ 0x00 ^ 0x15 ^ 0x01 ^ 0x0E;
        assert_eq!(buffer[6], expected_checksum, "Checksum calculation or placement failed");
    }

    #[test]
    fn strip_packet_happy_path() {
        let mut valid_packet: [u8; 7] = [0xAA, 0x04, 0x00, 0x15, 0x01, 0x0E, 0x00];
        valid_packet[6] = calculate_checksum(&valid_packet[0..6]);
        let result = strip_packet(&valid_packet);

        // Assert it returned Ok()
        assert!(result.is_ok(), "Valid packet was rejected!");
        
        // Unwrap the payload and check the bytes
        let payload = result.unwrap();
        assert_eq!(payload, &[0x04, 0x00, 0x15, 0x01, 0x0E], "Extracted payload bytes are incorrect");
    }

        #[test]
    fn strip_packet_unhappy_path() {
        let mut valid_packet: [u8; 7] = [0xAA, 0x04, 0x00, 0x15, 0x01, 0x0E, 0x00];
        let result = strip_packet(&valid_packet);

        // Assert is returned error
        assert!(result.is_err(), "Packet should've been rejected but wasn't.");
    }
}