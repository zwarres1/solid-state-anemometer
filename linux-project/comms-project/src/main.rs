use comms_project::request_wind_speed;
use anyhow::Result;

fn main() -> Result<()> {
    request_wind_speed()?;
    Ok(())
}
