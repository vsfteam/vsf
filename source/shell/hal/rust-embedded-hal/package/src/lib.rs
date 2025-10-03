#![cfg_attr(not(test), no_std)]
#![allow(async_fn_in_trait)]

// This must go FIRST so that all the other modules see its macros.
mod fmt;

// Utilities
mod macros;

/// Operating modes for peripherals.
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

// Hertz
#[derive(Eq, PartialEq, Ord, PartialOrd, Clone, Copy, Debug)]
pub struct Hertz(pub u32);

impl core::fmt::Display for Hertz {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        write!(f, "{} Hz", self.0)
    }
}

#[cfg(feature = "defmt")]
impl defmt::Format for Hertz {
    fn format(&self, f: defmt::Formatter) {
        defmt::write!(f, "{=u32} Hz", self.0)
    }
}

impl Hertz {
    /// Create a `Hertz` from the given hertz.
    pub const fn hz(hertz: u32) -> Self {
        Self(hertz)
    }

    /// Create a `Hertz` from the given kilohertz.
    pub const fn khz(kilohertz: u32) -> Self {
        Self(kilohertz * 1_000)
    }

    /// Create a `Hertz` from the given megahertz.
    pub const fn mhz(megahertz: u32) -> Self {
        Self(megahertz * 1_000_000)
    }
}

/// This is a convenience shortcut for [`Hertz::hz`]
pub const fn hz(hertz: u32) -> Hertz {
    Hertz::hz(hertz)
}

/// This is a convenience shortcut for [`Hertz::khz`]
pub const fn khz(kilohertz: u32) -> Hertz {
    Hertz::khz(kilohertz)
}

/// This is a convenience shortcut for [`Hertz::mhz`]
pub const fn mhz(megahertz: u32) -> Hertz {
    Hertz::mhz(megahertz)
}

use core::ops::{Div, Mul};

impl Mul<u32> for Hertz {
    type Output = Hertz;
    fn mul(self, rhs: u32) -> Self::Output {
        Hertz(self.0 * rhs)
    }
}

impl Div<u32> for Hertz {
    type Output = Hertz;
    fn div(self, rhs: u32) -> Self::Output {
        Hertz(self.0 / rhs)
    }
}

impl Mul<u16> for Hertz {
    type Output = Hertz;
    fn mul(self, rhs: u16) -> Self::Output {
        self * (rhs as u32)
    }
}

impl Div<u16> for Hertz {
    type Output = Hertz;
    fn div(self, rhs: u16) -> Self::Output {
        self / (rhs as u32)
    }
}

impl Mul<u8> for Hertz {
    type Output = Hertz;
    fn mul(self, rhs: u8) -> Self::Output {
        self * (rhs as u32)
    }
}

impl Div<u8> for Hertz {
    type Output = Hertz;
    fn div(self, rhs: u8) -> Self::Output {
        self / (rhs as u32)
    }
}

impl Div<Hertz> for Hertz {
    type Output = u32;
    fn div(self, rhs: Hertz) -> Self::Output {
        self.0 / rhs.0
    }
}

// vsf_hal
mod vsf_hal;

#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_enum_type {($type:ident) => { $type::Type }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_fsm_rt_t {($state:ident) => { fsm_rt_t::$state }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_stream_evt_t {($evt:ident) => { vsf_stream_evt_t::$evt }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_arch_prio_t {($prio:ident) => { vsf_arch_prio_t::$prio }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_hw_peripheral_en_t {($mode:ident) => { vsf_hw_peripheral_en_t::$mode }}

#[cfg(bindgen_enum_type_consts)]
macro_rules! into_enum_type {($type:ident) => { $type }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_fsm_rt_t {($state:ident) => { paste!{[<fsm_rt_t_ $state>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_stream_evt_t {($evt:ident) => { paste!{[<vsf_stream_evt_t_ $evt>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_arch_prio_t {($prio:ident) => { paste!{[<vsf_arch_prio_t_ $prio>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_hw_peripheral_en_t {($mode:ident) => { paste!{[<vsf_hw_peripheral_en_t_ $mode>]} }}

#[cfg(vsf_gpio_enabled)]
pub mod gpio;
#[cfg(vsf_usart_enabled)]
pub mod usart;
#[cfg(vsf_spi_enabled)]
pub mod spi;
#[cfg(vsf_i2c_enabled)]
pub mod i2c;

// This must go last, so that it sees all the impl_foo! macros defined earlier.
pub(crate) mod _generated {
    #![allow(dead_code)]
    #![allow(unused_imports)]
    #![allow(non_snake_case)]
    #![allow(missing_docs)]

    include!("./_generated.rs");
}

pub use crate::_generated::interrupt;

pub use _generated::{peripherals, Peripherals};
pub use embassy_hal_internal::{Peri, PeripheralType};
pub mod pac;

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
