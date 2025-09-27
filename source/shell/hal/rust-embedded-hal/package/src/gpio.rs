//! General purpose input/output (GPIO) driver.
#![macro_use]
#![allow(dead_code)]

use core::convert::Infallible;
use paste::paste;

use embassy_hal_internal::{impl_peripheral, Peri, PeripheralType};

use crate::vsf_hal::{*};

pub type PinPortType = u16;
pub type AfNumType = u16;

/// Pull setting for an input.
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Pull {
    /// No pull.
    None = into_vsf_gpio_mode_t!(VSF_GPIO_NO_PULL_UP_DOWN) as isize,
    /// Internal pull-up resistor.
    #[cfg(VSF_GPIO_PULL_UP)]
    Up = into_vsf_gpio_mode_t!(VSF_GPIO_PULL_UP) as isize,
    /// Internal pull-down resistor.
    #[cfg(VSF_GPIO_PULL_DOWN)]
    Down = into_vsf_gpio_mode_t!(VSF_GPIO_PULL_DOWN) as isize,
}

/// Speed setting for an output.
#[derive(Debug, Copy, Clone)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Speed {
    #[cfg(not(any(VSF_GPIO_SPEED_LOW, VSF_GPIO_SPEED_MEDIUM, VSF_GPIO_SPEED_HIGH, VSF_GPIO_SPEED_VERY_HIGH)))]
    None = 0,
    #[cfg(VSF_GPIO_SPEED_LOW)]
    Low = into_vsf_gpio_mode_t!(VSF_GPIO_SPEED_LOW) as isize,
    #[cfg(VSF_GPIO_SPEED_MEDIUM)]
    Medium = into_vsf_gpio_mode_t!(VSF_GPIO_SPEED_MEDIUM) as isize,
    #[cfg(VSF_GPIO_SPEED_HIGH)]
    High = into_vsf_gpio_mode_t!(VSF_GPIO_SPEED_HIGH) as isize,
    #[cfg(VSF_GPIO_SPEED_VERY_HIGH)]
    VeryHigh = into_vsf_gpio_mode_t!(VSF_GPIO_SPEED_VERY_HIGH) as isize,
}

impl Speed {
    pub const fn lowest() -> Self {
        #[cfg(VSF_GPIO_SPEED_LOW)]
        let result = Speed::Low;
        #[cfg(all(not(VSF_GPIO_SPEED_LOW), VSF_GPIO_SPEED_MEDIUM))]
        let result = Speed::Medium;
        #[cfg(all(not(VSF_GPIO_SPEED_LOW), not(VSF_GPIO_SPEED_MEDIUM), VSF_GPIO_SPEED_HIGH))]
        let result = Speed::High;
        #[cfg(all(not(VSF_GPIO_SPEED_LOW), not(VSF_GPIO_SPEED_MEDIUM), not(VSF_GPIO_SPEED_HIGH), VSF_GPIO_SPEED_VERY_HIGH))]
        let result = Speed::VeryHigh;
        #[cfg(not(any(VSF_GPIO_SPEED_LOW, VSF_GPIO_SPEED_MEDIUM, VSF_GPIO_SPEED_HIGH, VSF_GPIO_SPEED_VERY_HIGH)))]
        let result = Speed::None;

        result
    }
    pub const fn highest() -> Self {
        #[cfg(VSF_GPIO_SPEED_VERY_HIGH)]
        let result = Speed::VeryHigh;
        #[cfg(all(not(VSF_GPIO_SPEED_VERY_HIGH), VSF_GPIO_SPEED_HIGH))]
        let result = Speed::High;
        #[cfg(all(not(VSF_GPIO_SPEED_VERY_HIGH), not(VSF_GPIO_SPEED_HIGH), VSF_GPIO_SPEED_MEDIUM))]
        let result = Speed::Medium;
        #[cfg(all(not(VSF_GPIO_SPEED_VERY_HIGH), not(VSF_GPIO_SPEED_HIGH), not(VSF_GPIO_SPEED_MEDIUM), VSF_GPIO_SPEED_LOW))]
        let result = Speed::Low;
        #[cfg(not(any(VSF_GPIO_SPEED_VERY_HIGH, VSF_GPIO_SPEED_HIGH, VSF_GPIO_SPEED_MEDIUM, VSF_GPIO_SPEED_LOW)))]
        let result = Speed::None;

        result
    }
}

impl Default for Speed {
    fn default() -> Self {
        Speed::lowest()
    }
}

/// Drive strength settings for an output pin.
///
/// This is a combination of two drive levels, used when the pin is set
/// low and high respectively.
#[derive(Clone, Copy, Debug, PartialEq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum OutputDrive {
    #[cfg(not(any(VSF_GPIO_DRIVE_STRENGTH_LOW, VSF_GPIO_DRIVE_STRENGTH_MEDIUM, VSF_GPIO_DRIVE_STRENGTH_HIGH, VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)))]
    None = 0,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_LOW)]
    Low = into_vsf_gpio_mode_t!(VSF_GPIO_DRIVE_STRENGTH_LOW) as isize,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_MEDIUM)]
    Medium = into_vsf_gpio_mode_t!(VSF_GPIO_DRIVE_STRENGTH_MEDIUM) as isize,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_HIGH)]
    High = into_vsf_gpio_mode_t!(VSF_GPIO_DRIVE_STRENGTH_HIGH) as isize,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)]
    VeryHigh = into_vsf_gpio_mode_t!(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH) as isize,
}

impl OutputDrive {
    pub const fn lowest() -> Self {
        #[cfg(VSF_GPIO_DRIVE_STRENGTH_LOW)]
        let result = OutputDrive::Low;
        #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_LOW), VSF_GPIO_DRIVE_STRENGTH_MEDIUM))]
        let result = OutputDrive::Medium;
        #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_LOW), not(VSF_GPIO_DRIVE_STRENGTH_MEDIUM), VSF_GPIO_DRIVE_STRENGTH_HIGH))]
        let result = OutputDrive::High;
        #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_LOW), not(VSF_GPIO_DRIVE_STRENGTH_MEDIUM), not(VSF_GPIO_DRIVE_STRENGTH_HIGH), VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH))]
        let result = OutputDrive::VeryHigh;
        #[cfg(not(any(VSF_GPIO_DRIVE_STRENGTH_LOW, VSF_GPIO_DRIVE_STRENGTH_MEDIUM, VSF_GPIO_DRIVE_STRENGTH_HIGH, VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)))]
        let result = OutputDrive::None;

        result
    }
    pub const fn highest() -> Self {
        #[cfg(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)]
        let result = OutputDrive::VeryHigh;
        #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH), VSF_GPIO_DRIVE_STRENGTH_HIGH))]
        let result = OutputDrive::High;
        #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH), not(VSF_GPIO_DRIVE_STRENGTH_HIGH), VSF_GPIO_DRIVE_STRENGTH_MEDIUM))]
        let result = OutputDrive::Medium;
        #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH), not(VSF_GPIO_DRIVE_STRENGTH_HIGH), not(VSF_GPIO_DRIVE_STRENGTH_MEDIUM), VSF_GPIO_DRIVE_STRENGTH_LOW))]
        let result = OutputDrive::Low;
        #[cfg(not(any(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH, VSF_GPIO_DRIVE_STRENGTH_HIGH, VSF_GPIO_DRIVE_STRENGTH_MEDIUM, VSF_GPIO_DRIVE_STRENGTH_LOW)))]
        let result = OutputDrive::None;

        result
    }
}

impl Default for OutputDrive {
    fn default() -> Self {
        OutputDrive::lowest()
    }
}

/// Alternate function type settings.
#[derive(Copy, Clone)]
#[cfg(any(VSF_GPIO_AF_INPUT, VSF_GPIO_AF_PUSH_PULL, VSF_GPIO_AF_OPEN_DRAIN))]
pub enum AfMode {
    #[cfg(VSF_GPIO_AF_INPUT)]
    AfInput = into_vsf_gpio_mode_t!(VSF_GPIO_AF_INPUT) as isize,
    #[cfg(VSF_GPIO_AF_PUSH_PULL)]
    AfPushPull = into_vsf_gpio_mode_t!(VSF_GPIO_AF_PUSH_PULL) as isize,
    #[cfg(VSF_GPIO_AF_OPEN_DRAIN)]
    AfOpenDrain = into_vsf_gpio_mode_t!(VSF_GPIO_AF_OPEN_DRAIN) as isize,
    #[cfg(not(any(VSF_GPIO_AF, VSF_GPIO_AF_PUSH_PULL, VSF_GPIO_AF_OPEN_DRAIN)))]
    None = 0,
}

#[derive(Copy, Clone)]
#[cfg(any(VSF_GPIO_AF, VSF_GPIO_AF_PUSH_PULL, VSF_GPIO_AF_OPEN_DRAIN))]
pub struct AfType {
    mode: u32,
    pull: Pull,
    speed: Speed,
    drive: OutputDrive,
}

#[cfg(any(VSF_GPIO_AF, VSF_GPIO_AF_PUSH_PULL, VSF_GPIO_AF_OPEN_DRAIN))]
impl AfType {
    /// Input with optional pullup or pulldown.
    #[cfg(all(VSF_GPIO_AF, VSF_GPIO_INPUT))]
    pub const fn input(pull: Pull) -> Self {
        Self {
            #[cfg(VSF_GPIO_AF_INPUT)]
            mode: AfMode::AfInput as u32,
            #[cfg(all(not(VSF_GPIO_AF_INPUT), VSF_GPIO_AF))]
            mode: (into_vsf_gpio_mode_t!(VSF_GPIO_AF) | into_vsf_gpio_mode_t!(VSF_GPIO_INPUT)) as u32,

            pull: pull,

            #[cfg(VSF_GPIO_SPEED_LOW)]
            speed: Speed::Low,
            #[cfg(all(not(VSF_GPIO_SPEED_LOW), VSF_GPIO_SPEED_MEDIUM))]
            speed: Speed::Medium,
            #[cfg(all(not(VSF_GPIO_SPEED_LOW), not(VSF_GPIO_SPEED_MEDIUM), VSF_GPIO_SPEED_HIGH))]
            speed: Speed::High,
            #[cfg(all(not(VSF_GPIO_SPEED_LOW), not(VSF_GPIO_SPEED_MEDIUM), not(VSF_GPIO_SPEED_HIGH), VSF_GPIO_SPEED_VERY_HIGH))]
            speed: Speed::VeryHigh,
            #[cfg(not(any(VSF_GPIO_SPEED_LOW, VSF_GPIO_SPEED_MEDIUM, VSF_GPIO_SPEED_HIGH, VSF_GPIO_SPEED_VERY_HIGH)))]
            speed: Speed::None,

            #[cfg(VSF_GPIO_DRIVE_STRENGTH_LOW)]
            drive: OutputDrive::Low,
            #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_LOW), VSF_GPIO_DRIVE_STRENGTH_MEDIUM))]
            drive: OutputDrive::Medium,
            #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_LOW), not(VSF_GPIO_DRIVE_STRENGTH_MEDIUM), VSF_GPIO_DRIVE_STRENGTH_HIGH))]
            drive: OutputDrive::High,
            #[cfg(all(not(VSF_GPIO_DRIVE_STRENGTH_LOW), not(VSF_GPIO_DRIVE_STRENGTH_MEDIUM), not(VSF_GPIO_DRIVE_STRENGTH_HIGH), VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH))]
            drive: OutputDrive::VeryHigh,
            #[cfg(not(any(VSF_GPIO_DRIVE_STRENGTH_LOW, VSF_GPIO_DRIVE_STRENGTH_MEDIUM, VSF_GPIO_DRIVE_STRENGTH_HIGH, VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)))]
            drive: OutputDrive::None,
        }
    }

    /// Output with output type and speed and no pull-up or pull-down.
    #[cfg(VSF_GPIO_AF_PUSH_PULL)]
    pub const fn output(speed: Speed, drive: OutputDrive) -> Self {
        Self {
            mode: AfMode::AfPushPull as u32,
            pull: Pull::None,
            speed: speed,
            drive: drive,
        }
    }

    /// Input and output mode, commonly used for "open drain" mode.
    #[cfg(VSF_GPIO_AF_OPEN_DRAIN)]
    pub const fn input_output(pull: Pull, speed: Speed, drive: OutputDrive) -> Self {
        Self {
            mode: AfMode::AfOpenDrain as u32,
            pull: pull,
            speed: speed,
            drive: drive,
        }
    }
}

/// GPIO input driver.
pub struct Input<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> Input<'d> {
    /// Create GPIO input driver for a [Pin] with the provided [Pull] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, pull: Pull) -> Self {
        let mut pin = Flex::new(pin);
        pin.set_as_input(pull);

        Self { pin }
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        self.pin.is_high()
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        self.pin.is_low()
    }

    /// Get the pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.pin.get_level()
    }
}

impl Input<'static> {
    /// Persist the pin's configuration for the rest of the program's lifetime. This method should
    /// be preferred over [`core::mem::forget()`] because the `'static` bound prevents accidental
    /// reuse of the underlying peripheral.
    pub fn persist(self) {
        self.pin.persist()
    }
}

/// Digital input or output level.
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Level {
    /// Logical low.
    Low,
    /// Logical high.
    High,
}

impl From<bool> for Level {
    fn from(val: bool) -> Self {
        match val {
            true => Self::High,
            false => Self::Low,
        }
    }
}

impl From<Level> for bool {
    fn from(level: Level) -> bool {
        match level {
            Level::Low => false,
            Level::High => true,
        }
    }
}

/// GPIO output driver.
pub struct Output<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> Output<'d> {
    /// Create GPIO output driver for a [Pin] with the provided [Level], [Speed] and [OutputDrive] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, initial_output: Level, speed: Speed, drive: OutputDrive) -> Self {
        let mut pin = Flex::new(pin);
        match initial_output {
            Level::High => pin.set_high(),
            Level::Low => pin.set_low(),
        }
        pin.set_as_output(speed, drive);

        Self { pin }
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high()
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low()
    }

    /// Toggle the output level.
    #[inline]
    pub fn toggle(&mut self) {
        self.pin.toggle()
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        self.pin.set_level(level)
    }

    /// Get whether the output level is set to high.
    #[inline]
    pub fn is_set_high(&self) -> bool {
        self.pin.is_set_high()
    }

    /// Get whether the output level is set to low.
    #[inline]
    pub fn is_set_low(&self) -> bool {
        self.pin.is_set_low()
    }

    /// Get the current output level.
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.pin.get_output_level()
    }
}

impl Output<'static> {
    /// Persist the pin's configuration for the rest of the program's lifetime. This method should
    /// be preferred over [`core::mem::forget()`] because the `'static` bound prevents accidental
    /// reuse of the underlying peripheral.
    pub fn persist(self) {
        self.pin.persist()
    }
}

/// GPIO flexible pin.
///
/// This pin can either be a disconnected, input, or output pin, or both. The level register bit will remain
/// set while not in output mode, so the pin's level will be 'remembered' when it is not in output
/// mode.
pub struct Flex<'d> {
    pub(crate) pin: Peri<'d, AnyPin>,
}

impl<'d> Flex<'d> {
    /// Wrap the pin in a `Flex`.
    ///
    /// The pin remains disconnected. The initial output level is unspecified, but can be changed
    /// before the pin is put into output mode.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>) -> Self {
        // Pin will be in disconnected state.
        Self { pin: pin.into() }
    }

    /// Put the pin into input mode.
    #[inline]
    pub fn set_as_input(&mut self, pull: Pull) {
        self.pin.config(into_vsf_gpio_mode_t!(VSF_GPIO_INPUT) | pull as u32, 0);
    }

    /// Put the pin into output mode.
    ///
    /// The pin level will be whatever was set before (or low by default). If you want it to begin
    /// at a specific level, call `set_high`/`set_low` on the pin first.
    #[inline]
    pub fn set_as_output(&mut self, speed: Speed, drive: OutputDrive) {
        self.pin.config(into_vsf_gpio_mode_t!(VSF_GPIO_OUTPUT_PUSH_PULL) | speed as u32 | drive as u32, 0);
    }

    /// Put the pin into input + output mode.
    ///
    /// This is commonly used for "open drain" mode. If you set `drive = Standard0Disconnect1`,
    /// the hardware will drive the line low if you set it to low, and will leave it floating if you set
    /// it to high, in which case you can read the input to figure out whether another device
    /// is driving the line low.
    ///
    /// The pin level will be whatever was set before (or low by default). If you want it to begin
    /// at a specific level, call `set_high`/`set_low` on the pin first.
    #[inline]
    pub fn set_as_input_output(&mut self, pull: Pull, speed: Speed, drive: OutputDrive) {
        self.pin.config(into_vsf_gpio_mode_t!(VSF_GPIO_OUTPUT_OPEN_DRAIN) | pull as u32 | speed as u32 | drive as u32, 0);
    }

    /// Put the pin into analog mode
    ///
    /// This mode is used by ADC and COMP but usually there is no need to set this manually
    /// as the mode change is handled by the driver.
    #[inline]
    #[cfg(VSF_GPIO_ANALOG)]
    pub fn set_as_analog(&mut self) {
        self.pin.config(into_vsf_gpio_mode_t!(VSF_GPIO_ANALOG), 0);
    }

    /// Put the pin into AF mode, unchecked.
    ///
    /// This puts the pin into the AF mode, with the requested number and AF type. This is
    /// completely unchecked, it can attach the pin to literally any peripheral, so use with care.
    #[inline]
    #[cfg(any(VSF_GPIO_AF, VSF_GPIO_AF_PUSH_PULL, VSF_GPIO_AF_OPEN_DRAIN))]
    pub fn set_as_af(&mut self, af_num: AfNumType, af_type: AfType) {
        self.pin.config(into_vsf_gpio_mode_t!(VSF_GPIO_AF) | af_type.mode as u32 | af_type.pull as u32 | af_type.speed as u32 | af_type.drive as u32, af_num);
    }

    /// Put the pin into disconnected mode.
    #[inline]
    pub fn set_as_disconnected(&mut self) {
        // TODO
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        self.pin.is_high()
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        self.pin.is_low()
    }

    /// Get the pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.is_high().into()
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high()
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low()
    }

    /// Get whether the output level is set to high.
    #[inline]
    pub fn is_set_high(&self) -> bool {
        self.pin.is_set_high()
    }

    /// Get whether the output level is set to low.
    #[inline]
    pub fn is_set_low(&self) -> bool {
        self.pin.is_set_low()
    }

    /// Toggle the output level.
    #[inline]
    pub fn toggle(&mut self) {
        self.pin.toggle();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        match level {
            Level::Low => self.pin.set_low(),
            Level::High => self.pin.set_high(),
        }
    }

    /// Get the current output level.
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.pin.is_set_high().into()
    }
}

impl Flex<'static> {
    /// Persist the pin's configuration for the rest of the program's lifetime. This method should
    /// be preferred over [`core::mem::forget()`] because the `'static` bound prevents accidental
    /// reuse of the underlying peripheral.
    pub fn persist(self) {
        core::mem::forget(self);
    }
}

impl<'d> Drop for Flex<'d> {
    fn drop(&mut self) {
        self.set_as_disconnected();
    }
}

pub(crate) trait SealedPin {
    fn pin_port(&self) -> PinPortType;

    #[inline]
    fn _pin(&self) -> u8 {
        (self.pin_port() & 0xFF) as u8
    }

    #[inline]
    fn _port(&self) -> u8 {
        (self.pin_port() >> 8) as u8
    }

    /// Put the pin into input mode.
    #[inline]
    fn config(&self, mode: u32, af: AfNumType) {
        let mut mode = vsf_gpio_cfg_t {
            mode: mode,
            alternate_function: af,
        };
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_port_config_pins(gpio_port, (1 as vsf_gpio_pin_mask_t) << self._pin(), &mut mode);
        }
    }

    /// Set the output as high.
    #[inline]
    fn set_high(&self) {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_set(gpio_port, (1 as vsf_gpio_pin_mask_t) << self._pin());
        }
    }

    /// Set the output as low.
    #[inline]
    fn set_low(&self) {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_clear(gpio_port, (1 as vsf_gpio_pin_mask_t) << self._pin());
        }
    }

    /// Get whether the output level is set to high.
    #[inline]
    fn is_set_high(&self) -> bool {
        !self.is_set_low()
    }

    /// Get whether the output level is set to low.
    #[inline]
    fn is_set_low(&self) -> bool {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_read_output_register(gpio_port) & ((1 as vsf_gpio_pin_mask_t) << self._pin()) == 0
        }
    }

    /// Toggle the output level.
    #[inline]
    fn toggle(&self) {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_toggle(gpio_port, (1 as vsf_gpio_pin_mask_t) << self._pin());
        }
    }

    /// Get whether the pin input level is high.
    #[inline]
    fn is_high(&self) -> bool {
        !self.is_low()
    }

    /// Get whether the pin input level is low.
    #[inline]
    fn is_low(&self) -> bool {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_read(gpio_port) & ((1 as vsf_gpio_pin_mask_t) << self._pin()) == 0
        }
    }
}

/// Interface for a Pin that can be configured by an [Input] or [Output] driver, or converted to an [AnyPin].
#[allow(private_bounds)]
pub trait Pin: PeripheralType + Into<AnyPin> + SealedPin + Sized + 'static {
    /// Number of the pin within the port (0..31)
    #[inline]
    fn pin(&self) -> u8 {
        self._pin()
    }

    /// Port of the pin
    #[inline]
    fn port(&self) -> u8 {
        self._port()
    }
}

/// Type-erased GPIO pin
pub struct AnyPin {
    pub(crate) pin_port: PinPortType,
}

impl AnyPin {
    /// Create an [AnyPin] for a specific pin.
    ///
    /// # Safety
    /// - `pin_port` should not in use by another driver.
    #[inline]
    pub unsafe fn steal(pin_port: PinPortType) -> Peri<'static, Self> {
        unsafe {
            Peri::new_unchecked(Self { pin_port })
        }
    }
}

impl_peripheral!(AnyPin);
impl Pin for AnyPin {}
impl SealedPin for AnyPin {
    #[inline]
    fn pin_port(&self) -> PinPortType {
        self.pin_port
    }
}

// ====================

use crate::peripherals;
vsf_hal_macros::bind_vsf_gpio_pins!{}

pub(crate) unsafe fn init() {
    #[cfg(all(vsf_hw_clkrst_region_set_bit, vsf_hw_peripheral_en_t))]
    {
        unsafe {
            #[cfg(VSF_HW_EN_GPIO0)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO0) as u32);
            #[cfg(VSF_HW_EN_GPIO1)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO1) as u32);
            #[cfg(VSF_HW_EN_GPIO2)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO2) as u32);
            #[cfg(VSF_HW_EN_GPIO3)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO3) as u32);
            #[cfg(VSF_HW_EN_GPIO4)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO4) as u32);
            #[cfg(VSF_HW_EN_GPIO5)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO5) as u32);
            #[cfg(VSF_HW_EN_GPIO6)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO6) as u32);
            #[cfg(VSF_HW_EN_GPIO7)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO7) as u32);
            #[cfg(VSF_HW_EN_GPIO8)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO8) as u32);
            #[cfg(VSF_HW_EN_GPIO9)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO9) as u32);
            #[cfg(VSF_HW_EN_GPIO10)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO10) as u32);
            #[cfg(VSF_HW_EN_GPIO11)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO11) as u32);
            #[cfg(VSF_HW_EN_GPIO12)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO12) as u32);
            #[cfg(VSF_HW_EN_GPIO13)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO13) as u32);
            #[cfg(VSF_HW_EN_GPIO14)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO14) as u32);
            #[cfg(VSF_HW_EN_GPIO15)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO15) as u32);
            #[cfg(VSF_HW_EN_GPIO16)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO16) as u32);
            #[cfg(VSF_HW_EN_GPIO17)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO17) as u32);
            #[cfg(VSF_HW_EN_GPIO18)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO18) as u32);
            #[cfg(VSF_HW_EN_GPIO19)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO19) as u32);
            #[cfg(VSF_HW_EN_GPIO20)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO20) as u32);
            #[cfg(VSF_HW_EN_GPIO21)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO21) as u32);
            #[cfg(VSF_HW_EN_GPIO22)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO22) as u32);
            #[cfg(VSF_HW_EN_GPIO23)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO23) as u32);
            #[cfg(VSF_HW_EN_GPIO24)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO24) as u32);
            #[cfg(VSF_HW_EN_GPIO25)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO25) as u32);
            #[cfg(VSF_HW_EN_GPIO26)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO26) as u32);
            #[cfg(VSF_HW_EN_GPIO27)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO27) as u32);
            #[cfg(VSF_HW_EN_GPIO28)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO28) as u32);
            #[cfg(VSF_HW_EN_GPIO29)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO29) as u32);
            #[cfg(VSF_HW_EN_GPIO30)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO30) as u32);
            #[cfg(VSF_HW_EN_GPIO31)]
            vsf_hw_clkrst_region_set_bit(into_vsf_hw_peripheral_en_t!(VSF_HW_EN_GPIO31) as u32);
        }
    }
}

// ====================

mod eh02 {
    use super::*;

    impl<'d> embedded_hal_02::digital::v2::InputPin for Input<'d> {
        type Error = Infallible;

        fn is_high(&self) -> Result<bool, Self::Error> {
            Ok(self.is_high())
        }

        fn is_low(&self) -> Result<bool, Self::Error> {
            Ok(self.is_low())
        }
    }

    impl<'d> embedded_hal_02::digital::v2::OutputPin for Output<'d> {
        type Error = Infallible;

        fn set_high(&mut self) -> Result<(), Self::Error> {
            self.set_high();
            Ok(())
        }

        fn set_low(&mut self) -> Result<(), Self::Error> {
            self.set_low();
            Ok(())
        }
    }

    impl<'d> embedded_hal_02::digital::v2::StatefulOutputPin for Output<'d> {
        fn is_set_high(&self) -> Result<bool, Self::Error> {
            Ok(self.is_set_high())
        }

        fn is_set_low(&self) -> Result<bool, Self::Error> {
            Ok(self.is_set_low())
        }
    }

    impl<'d> embedded_hal_02::digital::v2::ToggleableOutputPin for Output<'d> {
        type Error = Infallible;
        #[inline]
        fn toggle(&mut self) -> Result<(), Self::Error> {
            self.toggle();
            Ok(())
        }
    }

    /// Implement [`embedded_hal_02::digital::v2::InputPin`] for [`Flex`];
    ///
    /// If the pin is not in input mode the result is unspecified.
    impl<'d> embedded_hal_02::digital::v2::InputPin for Flex<'d> {
        type Error = Infallible;

        fn is_high(&self) -> Result<bool, Self::Error> {
            Ok(self.is_high())
        }

        fn is_low(&self) -> Result<bool, Self::Error> {
            Ok(self.is_low())
        }
    }

    impl<'d> embedded_hal_02::digital::v2::OutputPin for Flex<'d> {
        type Error = Infallible;

        fn set_high(&mut self) -> Result<(), Self::Error> {
            self.set_high();
            Ok(())
        }

        fn set_low(&mut self) -> Result<(), Self::Error> {
            self.set_low();
            Ok(())
        }
    }

    impl<'d> embedded_hal_02::digital::v2::StatefulOutputPin for Flex<'d> {
        fn is_set_high(&self) -> Result<bool, Self::Error> {
            Ok(self.is_set_high())
        }

        fn is_set_low(&self) -> Result<bool, Self::Error> {
            Ok(self.is_set_low())
        }
    }

    impl<'d> embedded_hal_02::digital::v2::ToggleableOutputPin for Flex<'d> {
        type Error = Infallible;
        #[inline]
        fn toggle(&mut self) -> Result<(), Self::Error> {
            self.toggle();
            Ok(())
        }
    }
}

impl<'d> embedded_hal_1::digital::ErrorType for Input<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal_1::digital::InputPin for Input<'d> {
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_high())
    }

    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_low())
    }
}

impl<'d> embedded_hal_1::digital::ErrorType for Output<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal_1::digital::OutputPin for Output<'d> {
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_high();
        Ok(())
    }

    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.set_low();
        Ok(())
    }
}

impl<'d> embedded_hal_1::digital::StatefulOutputPin for Output<'d> {
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_high())
    }

    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_low())
    }
}

impl<'d> embedded_hal_1::digital::ErrorType for Flex<'d> {
    type Error = Infallible;
}

/// Implement [embedded_hal_1::digital::InputPin] for [`Flex`];
///
/// If the pin is not in input mode the result is unspecified.
impl<'d> embedded_hal_1::digital::InputPin for Flex<'d> {
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_high())
    }

    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_low())
    }
}

impl<'d> embedded_hal_1::digital::OutputPin for Flex<'d> {
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_high();
        Ok(())
    }

    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.set_low();
        Ok(())
    }
}

impl<'d> embedded_hal_1::digital::StatefulOutputPin for Flex<'d> {
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_high())
    }

    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_low())
    }
}