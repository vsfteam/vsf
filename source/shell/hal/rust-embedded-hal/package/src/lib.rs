#![cfg_attr(not(test), no_std)]
#![allow(async_fn_in_trait)]

// This must go FIRST so that all the other modules see its macros.
mod fmt;

// Utilities
mod macros;

// vsf_hal
mod vsf_hal;

#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_enum_type {($type:ident) => { $type::Type }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_gpio_mode_t {($mode:ident) => { vsf_gpio_mode_t::$mode }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_hw_peripheral_en_t {($mode:ident) => { vsf_hw_peripheral_en_t::$mode }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_io_port_pin_no_t {($pin:ident) => { vsf_io_port_pin_no_t::$pin }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_usart_mode_t {($mode:ident) => { vsf_usart_mode_t::$mode }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_usart_irq_mask_t {($mode:ident) => { vsf_usart_irq_mask_t::$mode }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_usart_ctrl_t {($mode:ident) => { vsf_usart_ctrl_t::$mode }}

#[cfg(bindgen_enum_type_consts)]
macro_rules! into_enum_type {($type:ident) => { $type }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_gpio_mode_t {($mode:ident) => { paste!{[<vsf_gpio_mode_t_ $mode>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_hw_peripheral_en_t {($mode:ident) => { paste!{[<vsf_hw_peripheral_en_t_ $mode>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_io_port_pin_no_t {($pin:ident) => { paste!{[<vsf_io_port_pin_no_t_ $pin>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_usart_mode_t {($mode:ident) => { paste!{[<vsf_usart_mode_t_ $mode>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_usart_irq_mask_t {($mode:ident) => { paste!{[<vsf_usart_irq_mask_t_ $mode>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_usart_ctrl_t {($mode:ident) => { paste!{[<vsf_usart_ctrl_t_ $mode>]} }}

#[cfg(vsf_gpio_enabled)]
pub mod gpio;
#[cfg(vsf_usart_enabled)]
pub mod usart;

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
