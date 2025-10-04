#![no_std]
#![no_main]

use embassy_executor::Spawner;
use embassy_vsf::gpio::{Level, Output, Speed, OutputDrive};

#[cfg(debug_assertions)]
use {defmt_rtt as _};
#[cfg(debug_assertions)]
use defmt::*;
#[cfg(debug_assertions)]
use {panic_probe as _};

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let p = embassy_vsf::init(Default::default());

    #[cfg(debug_assertions)]
    info!("Hello World!");

    let mut led = Output::new(p.P3_13, Level::High, Speed::default(), OutputDrive::default());
    loop {
        #[cfg(debug_assertions)]
        info!("high");

        led.set_high();

        #[cfg(debug_assertions)]
        info!("low");

        led.set_low();
    }
}
