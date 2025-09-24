#![cfg_attr(not(test), no_std)]
#![allow(async_fn_in_trait)]

// This must go FIRST so that all the other modules see its macros.
mod fmt;

// Utilities
mod macros;

// vsf_hal
mod vsf_hal;

pub mod mode {
    trait SealedMode {}

    /// Operating mode for a peripheral.
    #[allow(private_bounds)]
    pub trait Mode: SealedMode {}

    macro_rules! impl_mode {
        ($name:ident) => {
            impl SealedMode for $name {}
            impl Mode for $name {}
        };
    }

    /// Blocking mode.
    pub struct Blocking;
    /// Async mode.
    pub struct Async;

    impl_mode!(Blocking);
    impl_mode!(Async);
}

#[cfg(vsf_gpio_enabled)]
pub mod gpio;
#[cfg(vsf_usart_enabled)]
pub mod usart;

vsf_hal_macros::bind_vsf_peripherials!{}
vsf_hal_macros::bind_vsf_interrupts!{}

#[non_exhaustive]
#[derive(Clone, Copy)]
pub struct Config {

}

impl Default for Config {
    fn default() -> Self {
        Self {

        }
    }
}

pub fn init(config: Config) -> Peripherals {
    critical_section::with(|cs| {
        let p = Peripherals::take_with_cs(cs);

        unsafe {
            gpio::init();
        }

        p
    })
}
