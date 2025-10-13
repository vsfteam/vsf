#![cfg_attr(target_mcu, no_std)]
#![cfg_attr(target_mcu, no_main)]

use embassy_executor::Spawner;
use embassy_vsf::gpio::{Level, Output, Speed, OutputDrive};
use embassy_vsf::info;

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let p = embassy_vsf::init(Default::default());

    info!("Hello World!");

    let mut led = Output::new(p.P0_13, Level::High, Speed::default(), OutputDrive::default());
    loop {
        info!("high");
        led.set_high();
        info!("low");
        led.set_low();
    }
}
