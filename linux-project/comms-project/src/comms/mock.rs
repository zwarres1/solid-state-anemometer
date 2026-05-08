use std::collections::VecDeque;
use std::io::{self, Read, Write};

pub struct MockComms {
    buffer: VecDeque<Vec<u8>>,
}

/// Implements a Mock comm setup that just does a loopback
impl MockComms {
    pub fn new() -> Self {
        Self {
            buffer: VecDeque::new(),
        }
    }
}

impl Read for MockComms {
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        match self.buffer.pop_front() {
            Some(data) => {
                let amount_to_copy = std::cmp::min(buf.len(), data.len());
                buf[..amount_to_copy].copy_from_slice(&data[..amount_to_copy]);
                Ok(amount_to_copy)
            }
            None => Err(io::Error::new(io::ErrorKind::WouldBlock, "No data in mock buffer")),
        }
    }
}

impl Write for MockComms {
    fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
        self.buffer.push_back(buf.to_vec());
        Ok(buf.len())
    }
    
    fn flush(&mut self) -> io::Result<()> {
        // NO-OP for mock
        Ok(())
    }
}