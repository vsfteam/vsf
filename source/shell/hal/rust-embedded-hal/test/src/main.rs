#![cfg_attr(target_mcu, no_std)]
#![cfg_attr(target_mcu, no_main)]

use embassy_executor::Spawner;
#[cfg(vsf_gpio_enabled)]
use embassy_vsf::gpio::{Level, Output, Speed, OutputDrive};
use embassy_vsf::info;

#[embassy_executor::main]
async fn main(_spawner: Spawner) {
    let p = embassy_vsf::init(Default::default());

    info!("Hello World!");

    #[cfg(vsf_gpio_enabled)]
    let mut led = Output::new(p.P0_13, Level::High, Speed::default(), OutputDrive::default());
    loop {
        info!("high");

        #[cfg(vsf_gpio_enabled)]
        led.set_high();

        info!("low");

        #[cfg(vsf_gpio_enabled)]
        led.set_low();
    }
}
