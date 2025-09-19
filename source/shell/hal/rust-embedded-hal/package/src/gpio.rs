//! General-purpose Input/Output (GPIO)

#![macro_use]
use core::convert::Infallible;

use critical_section::CriticalSection;
use embassy_hal_internal::{impl_peripheral, Peri, PeripheralType};

use super::vsf_hal::{vsf_gpio_mode_t::*, *};

vsf_hal_macros::bind_vsf_gpios!{}

pub type PinPortType = u16;

/// Mode setting for an gpio.
#[derive(Copy, Clone, Debug, Eq, PartialEq, Ord, PartialOrd)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Mode {
    #[cfg(VSF_GPIO_INPUT)]
    Input = VSF_GPIO_INPUT as isize,

    #[cfg(VSF_GPIO_OUTPUT_OPEN_DRAIN)]
    OutputOd = VSF_GPIO_OUTPUT_OPEN_DRAIN as isize,
    #[cfg(VSF_GPIO_OUTPUT_PUSH_PULL)]
    OutputPushPull = VSF_GPIO_OUTPUT_PUSH_PULL as isize,

    #[cfg(VSF_GPIO_ANALOG)]
    Analog = VSF_GPIO_ANALOG as isize,
}

/// GPIO output type
#[derive(Debug, Copy, Clone)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum OutputType {
    /// Drive the pin both high or low.
    PushPull = VSF_GPIO_OUTPUT_PUSH_PULL as isize,
    /// Drive the pin low, or don't drive it at all if the output level is high.
    OpenDrain = VSF_GPIO_OUTPUT_OPEN_DRAIN as isize,
}

/// Pull setting for an input.
#[derive(Debug, Eq, PartialEq, Copy, Clone)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Pull {
    /// No pull
    #[cfg(VSF_GPIO_NO_PULL_UP_DOWN)]
    None = VSF_GPIO_NO_PULL_UP_DOWN as isize,
    /// Pull up
    #[cfg(VSF_GPIO_PULL_UP)]
    Up = VSF_GPIO_PULL_UP as isize,
    /// Pull down
    #[cfg(VSF_GPIO_PULL_DOWN)]
    Down = VSF_GPIO_PULL_DOWN as isize,
}

/// Speed setting for an output.
#[derive(Debug, Copy, Clone)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Speed {
    #[cfg(not(any(VSF_GPIO_SPEED_LOW, VSF_GPIO_SPEED_MEDIUM, VSF_GPIO_SPEED_HIGH, VSF_GPIO_SPEED_VERY_HIGH)))]
    None = 0,
    #[cfg(VSF_GPIO_SPEED_LOW)]
    Low = VSF_GPIO_SPEED_LOW as isize,
    #[cfg(VSF_GPIO_SPEED_MEDIUM)]
    Medium = VSF_GPIO_SPEED_MEDIUM as isize,
    #[cfg(VSF_GPIO_SPEED_HIGH)]
    High = VSF_GPIO_SPEED_HIGH as isize,
    #[cfg(VSF_GPIO_SPEED_VERY_HIGH)]
    VeryHigh = VSF_GPIO_SPEED_VERY_HIGH as isize,
}

/// Drive strength setting for an output.
pub enum DriveStrength {
    #[cfg(not(any(VSF_GPIO_DRIVE_STRENGTH_LOW, VSF_GPIO_DRIVE_STRENGTH_MEDIUM, VSF_GPIO_DRIVE_STRENGTH_HIGH, VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)))]
    None = 0,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_LOW)]
    Low = VSF_GPIO_DRIVE_STRENGTH_LOW as isize,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_MEDIUM)]
    Medium = VSF_GPIO_DRIVE_STRENGTH_MEDIUM as isize,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_HIGH)]
    High = VSF_GPIO_DRIVE_STRENGTH_HIGH as isize,
    #[cfg(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH)]
    VeryHigh = VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH as isize,
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
    ///
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>) -> Self {
        // Pin will be in disconnected state.
        Self { pin: pin.into() }
    }

    /// Put the pin into input mode.
    ///
    /// The internal weak pull-up and pull-down resistors will be enabled according to `pull`.
    #[inline(never)]
    pub fn set_as_input(&mut self, pull: Pull) {
        let mut mode = vsf_gpio_cfg_t {
            mode: match pull {
                Pull::Up => VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
                Pull::Down => VSF_GPIO_INPUT | VSF_GPIO_PULL_DOWN,
                Pull::None => VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
            },
            alternate_function: 0
        };
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_port_config_pins(gpio_port, 1u32 << self.pin._pin(), &mut mode);
        }
    }

    /// Put the pin into push-pull output mode.
    ///
    /// The pin level will be whatever was set before (or low by default). If you want it to begin
    /// at a specific level, call `set_high`/`set_low` on the pin first.
    ///
    /// The internal weak pull-up and pull-down resistors will be disabled.
    #[inline(never)]
    pub fn set_as_output(&mut self, speed: Speed, strength: DriveStrength) {
        let mut mode = vsf_gpio_cfg_t {
            mode: VSF_GPIO_OUTPUT_PUSH_PULL | speed as u32 | strength as u32,
            alternate_function: 0
        };
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_port_config_pins(gpio_port, 1u32 << self.pin._pin(), &mut mode);
        }
    }

    /// Put the pin into input + open-drain output mode.
    ///
    /// The hardware will drive the line low if you set it to low, and will leave it floating if you set
    /// it to high, in which case you can read the input to figure out whether another device
    /// is driving the line low.
    ///
    /// The pin level will be whatever was set before (or low by default). If you want it to begin
    /// at a specific level, call `set_high`/`set_low` on the pin first.
    ///
    /// The internal weak pull-up and pull-down resistors will be disabled.
    #[inline(never)]
    pub fn set_as_input_output(&mut self, speed: Speed) {
        let mut mode = vsf_gpio_cfg_t {
            mode: VSF_GPIO_OUTPUT_OPEN_DRAIN | speed as u32,
            alternate_function: 0
        };
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_port_config_pins(gpio_port, 1u32 << self.pin._pin(), &mut mode);
        }
    }

    /// Put the pin into input + open-drain output mode with internal pullup or pulldown.
    ///
    /// This works like [`Self::set_as_input_output()`], but it also allows to enable the internal
    /// weak pull-up or pull-down resistors.
    #[inline(never)]
    pub fn set_as_input_output_pull(&mut self, speed: Speed, pull: Pull) {
        let mut mode = vsf_gpio_cfg_t {
            mode: VSF_GPIO_OUTPUT_OPEN_DRAIN | speed as u32 | pull as u32,
            alternate_function: 0
        };
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_port_config_pins(gpio_port, 1u32 << self.pin._pin(), &mut mode);
        }
    }

    /// Put the pin into analog mode
    ///
    /// This mode is used by ADC and COMP but usually there is no need to set this manually
    /// as the mode change is handled by the driver.
    #[inline]
    pub fn set_as_analog(&mut self) {
        let mut mode = vsf_gpio_cfg_t {
            mode: VSF_GPIO_ANALOG,
            alternate_function: 0
        };
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_port_config_pins(gpio_port, 1u32 << self.pin._pin(), &mut mode);
        }
    }

    /// Put the pin into AF mode, unchecked.
    ///
    /// This puts the pin into the AF mode, with the requested number and AF type. This is
    /// completely unchecked, it can attach the pin to literally any peripheral, so use with care.
    #[inline]
    pub fn set_as_af_unchecked(&mut self, af_num: u8, af_type: AfType) {
        self.pin.set_as_af(
            af_num,
            af_type,
        );
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        !self.is_low()
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_read(gpio_port) & (1u32 << self.pin._pin()) == 0
        }
    }

    /// Get the current pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.is_high().into()
    }

    /// Get whether the output level is set to high.
    #[inline]
    pub fn is_set_high(&self) -> bool {
        !self.is_set_low()
    }

    /// Get whether the output level is set to low.
    #[inline]
    pub fn is_set_low(&self) -> bool {
        // TODO: read saved output mask
        false
    }

    /// Get the current output level.
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.is_set_high().into()
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high();
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        match level {
            Level::Low => self.pin.set_low(),
            Level::High => self.pin.set_high(),
        }
    }

    /// Toggle the output level.
    #[inline]
    pub fn toggle(&mut self) {
        unsafe {
            let gpio_port = vsf_hw_gpios[self.pin._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_toggle(gpio_port, 1u32 << self.pin._pin());
        }
    }
}

impl<'d> Drop for Flex<'d> {
    #[inline]
    fn drop(&mut self) {
        self.pin.set_as_disconnected();
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

    /// Get the current pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.pin.get_level()
    }
}

/// Digital input or output level.
#[derive(Debug, Eq, PartialEq, Copy, Clone)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Level {
    /// Low
    Low,
    /// High
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
///
/// Note that pins will **return to their floating state** when `Output` is dropped.
/// If pins should retain their state indefinitely, either keep ownership of the
/// `Output`, or pass it to [`core::mem::forget`].
pub struct Output<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> Output<'d> {
    /// Create GPIO output driver for a [Pin] with the provided [Level] and [Speed] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, initial_output: Level, speed: Speed, strength: DriveStrength) -> Self {
        let mut pin = Flex::new(pin);
        match initial_output {
            Level::High => pin.set_high(),
            Level::Low => pin.set_low(),
        }
        pin.set_as_output(speed, strength);
        Self { pin }
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high();
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        self.pin.set_level(level)
    }

    /// Is the output pin set as high?
    #[inline]
    pub fn is_set_high(&self) -> bool {
        self.pin.is_set_high()
    }

    /// Is the output pin set as low?
    #[inline]
    pub fn is_set_low(&self) -> bool {
        self.pin.is_set_low()
    }

    /// What level output is set to
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.pin.get_output_level()
    }

    /// Toggle pin output
    #[inline]
    pub fn toggle(&mut self) {
        self.pin.toggle();
    }
}

/// GPIO output open-drain driver.
///
/// Note that pins will **return to their floating state** when `OutputOpenDrain` is dropped.
/// If pins should retain their state indefinitely, either keep ownership of the
/// `OutputOpenDrain`, or pass it to [`core::mem::forget`].
pub struct OutputOpenDrain<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> OutputOpenDrain<'d> {
    /// Create a new GPIO open drain output driver for a [Pin] with the provided [Level] and [Speed].
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, initial_output: Level, speed: Speed) -> Self {
        let mut pin = Flex::new(pin);
        match initial_output {
            Level::High => pin.set_high(),
            Level::Low => pin.set_low(),
        }
        pin.set_as_input_output(speed);
        Self { pin }
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        !self.pin.is_low()
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        self.pin.is_low()
    }

    /// Get the current pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.pin.get_level()
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high();
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        self.pin.set_level(level);
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

    /// Toggle pin output
    #[inline]
    pub fn toggle(&mut self) {
        self.pin.toggle()
    }
}

/// Alternate function type settings.
#[derive(Copy, Clone)]
pub enum AfMode {
    #[cfg(VSF_GPIO_AF_PUSH_PULL)]
    AfPushPull = VSF_GPIO_AF_PUSH_PULL as isize,
    #[cfg(VSF_GPIO_AF_OPEN_DRAIN)]
    AfOpenDrain = VSF_GPIO_AF_OPEN_DRAIN as isize,
}

#[derive(Copy, Clone)]
pub struct AfType {
    mode: u32,
    pull: Pull,
}

impl AfType {
    /// Input with optional pullup or pulldown.
    pub const fn input(pull: Pull) -> Self {
        Self {
            mode: VSF_GPIO_AF | VSF_GPIO_INPUT,
            pull,
        }
    }

    /// Output with output type and speed and no pull-up or pull-down.
    pub const fn output(output_type: OutputType, speed: Speed, strength: DriveStrength) -> Self {
        Self {
            mode: AfMode::AfPushPull as u32 | speed as u32 | strength as u32 | output_type as u32,
            pull: Pull::None,
        }
    }
}

#[inline(never)]
fn set_as_af(pin_port: PinPortType, af_num: u8, af_type: AfType) {
    let mut mode = vsf_gpio_cfg_t {
        mode: af_type.pull as u32 | af_type.mode,
        alternate_function: af_num as u16,
    };
    unsafe {
        let gpio_port = vsf_hw_gpios[(pin_port >> 8) as usize] as *mut vsf_gpio_t;
        vsf_gpio_port_config_pins(gpio_port, 1u32 << (pin_port & 0xFF), &mut mode);
    }
}

#[inline(never)]
fn set_as_analog(pin_port: PinPortType) {
    let mut mode = vsf_gpio_cfg_t {
        mode: VSF_GPIO_ANALOG,
        alternate_function: 0,
    };
    unsafe {
        let gpio_port = vsf_hw_gpios[(pin_port >> 8) as usize] as *mut vsf_gpio_t;
        vsf_gpio_port_config_pins(gpio_port, 1u32 << (pin_port & 0xFF), &mut mode);
    }
}

/// Holds the AFIO remap value for a peripheral's pin
pub struct AfioRemap<const V: u8>;

/// Holds the AFIO remap value for a peripheral's pin
pub struct AfioRemapBool<const V: bool>;

/// Placeholder for a peripheral's pin which cannot be remapped via AFIO.
pub struct AfioRemapNotApplicable;

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

    /// Set the output as high.
    #[inline]
    fn set_high(&self) {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_set(gpio_port, 1u32 << self._pin());
        }
    }

    /// Set the output as low.
    #[inline]
    fn set_low(&self) {
        unsafe {
            let gpio_port = vsf_hw_gpios[self._port() as usize] as *mut vsf_gpio_t;
            vsf_gpio_clear(gpio_port, 1u32 << self._pin());
        }
    }

    #[inline]
    fn set_as_af(&self, af_num: u8, af_type: AfType) {
        set_as_af(
            self.pin_port(),
            af_num,
            af_type,
        )
    }

    #[inline]
    fn set_as_analog(&self) {
        set_as_analog(self.pin_port());
    }

    /// Set the pin as "disconnected", ie doing nothing and consuming the lowest
    /// amount of power possible.
    ///
    /// This is currently the same as [`Self::set_as_analog()`] but is semantically different
    /// really. Drivers should `set_as_disconnected()` pins when dropped.
    ///
    /// Note that this also disables the internal weak pull-up and pull-down resistors.
    #[inline]
    fn set_as_disconnected(&self) {
        self.set_as_analog();
    }
}

/// GPIO pin trait.
#[allow(private_bounds)]
pub trait Pin: PeripheralType + Into<AnyPin> + SealedPin + Sized + 'static {
    /// EXTI channel assigned to this pin.
    ///
    /// For example, PC4 uses EXTI4.
// TODO: add exti support
//    #[cfg(feature = "exti")]
//    type ExtiChannel: crate::exti::Channel;

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
    pin_port: PinPortType,
}

impl AnyPin {
    /// Unsafely create an `AnyPin` from a pin+port number.
    ///
    /// `pin_port` is `port_num << 8 + pin_num`, where `port_num` is 0 for port `A`, 1 for port `B`, etc...
    #[inline]
    pub unsafe fn steal(pin_port: PinPortType) -> Peri<'static, Self> {
        unsafe{
            Peri::new_unchecked(Self { pin_port })
        }
    }

    #[inline]
    fn _port(&self) -> u8 {
        (self.pin_port >> 8) as u8
    }
}

impl_peripheral!(AnyPin);
impl Pin for AnyPin {
//    type ExtiChannel = crate::exti::AnyChannel;
}
impl SealedPin for AnyPin {
    #[inline]
    fn pin_port(&self) -> PinPortType {
        self.pin_port
    }
}

// ====================

pub(crate) unsafe fn init(_cs: CriticalSection) {
    #[cfg(vsf_hw_clkrst_region_set_bit)]
    {
        macro_rules! enable_peripherals {
            ( $($peripheral_name:ident),* ) => {
                $(
                    #[cfg($peripheral_name)]
                    vsf_hw_clkrst_region_set_bit(concat!(VSF_HW_EN_, $peripheral_name) as u32);
                )*
            }
        }
        enable_peripherals!(GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI,
            GPIOJ, GPIOK, GPIOL, GPIOM, GPION, GPIOO, GPIOP, GPIOQ, GPIOR, GPIOS, GPIOT, GPIOU, GPIOV, GPIOW, GPIOX, GPIOY, GPIOZ);
    }
}

impl<'d> embedded_hal_02::digital::v2::InputPin for Input<'d> {
    type Error = Infallible;

    #[inline]
    fn is_high(&self) -> Result<bool, Self::Error> {
        Ok(self.is_high())
    }

    #[inline]
    fn is_low(&self) -> Result<bool, Self::Error> {
        Ok(self.is_low())
    }
}

impl<'d> embedded_hal_02::digital::v2::OutputPin for Output<'d> {
    type Error = Infallible;

    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_high();
        Ok(())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.set_low();
        Ok(())
    }
}

impl<'d> embedded_hal_02::digital::v2::StatefulOutputPin for Output<'d> {
    #[inline]
    fn is_set_high(&self) -> Result<bool, Self::Error> {
        Ok(self.is_set_high())
    }

    /// Is the output pin set as low?
    #[inline]
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

impl<'d> embedded_hal_02::digital::v2::InputPin for OutputOpenDrain<'d> {
    type Error = Infallible;

    fn is_high(&self) -> Result<bool, Self::Error> {
        Ok(self.is_high())
    }

    fn is_low(&self) -> Result<bool, Self::Error> {
        Ok(self.is_low())
    }
}

impl<'d> embedded_hal_02::digital::v2::OutputPin for OutputOpenDrain<'d> {
    type Error = Infallible;

    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_high();
        Ok(())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.set_low();
        Ok(())
    }
}

impl<'d> embedded_hal_02::digital::v2::StatefulOutputPin for OutputOpenDrain<'d> {
    #[inline]
    fn is_set_high(&self) -> Result<bool, Self::Error> {
        Ok(self.is_set_high())
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&self) -> Result<bool, Self::Error> {
        Ok(self.is_set_low())
    }
}

impl<'d> embedded_hal_02::digital::v2::ToggleableOutputPin for OutputOpenDrain<'d> {
    type Error = Infallible;
    #[inline]
    fn toggle(&mut self) -> Result<(), Self::Error> {
        self.toggle();
        Ok(())
    }
}

impl<'d> embedded_hal_02::digital::v2::InputPin for Flex<'d> {
    type Error = Infallible;

    #[inline]
    fn is_high(&self) -> Result<bool, Self::Error> {
        Ok(self.is_high())
    }

    #[inline]
    fn is_low(&self) -> Result<bool, Self::Error> {
        Ok(self.is_low())
    }
}

impl<'d> embedded_hal_02::digital::v2::OutputPin for Flex<'d> {
    type Error = Infallible;

    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_high();
        Ok(())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.set_low();
        Ok(())
    }
}

impl<'d> embedded_hal_02::digital::v2::StatefulOutputPin for Flex<'d> {
    #[inline]
    fn is_set_high(&self) -> Result<bool, Self::Error> {
        Ok(self.is_set_high())
    }

    /// Is the output pin set as low?
    #[inline]
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

impl<'d> embedded_hal_1::digital::ErrorType for Input<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal_1::digital::InputPin for Input<'d> {
    #[inline]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_high())
    }

    #[inline]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_low())
    }
}

impl<'d> embedded_hal_1::digital::ErrorType for Output<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal_1::digital::OutputPin for Output<'d> {
    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        Ok(self.set_high())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        Ok(self.set_low())
    }
}

impl<'d> embedded_hal_1::digital::StatefulOutputPin for Output<'d> {
    #[inline]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_high())
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_low())
    }
}

impl<'d> embedded_hal_1::digital::ErrorType for OutputOpenDrain<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal_1::digital::InputPin for OutputOpenDrain<'d> {
    #[inline]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_high())
    }

    #[inline]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_low())
    }
}

impl<'d> embedded_hal_1::digital::OutputPin for OutputOpenDrain<'d> {
    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        Ok(self.set_high())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        Ok(self.set_low())
    }
}

impl<'d> embedded_hal_1::digital::StatefulOutputPin for OutputOpenDrain<'d> {
    #[inline]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_high())
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_low())
    }
}

impl<'d> embedded_hal_1::digital::InputPin for Flex<'d> {
    #[inline]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_high())
    }

    #[inline]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_low())
    }
}

impl<'d> embedded_hal_1::digital::OutputPin for Flex<'d> {
    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        Ok(self.set_high())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        Ok(self.set_low())
    }
}

impl<'d> embedded_hal_1::digital::ErrorType for Flex<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal_1::digital::StatefulOutputPin for Flex<'d> {
    #[inline]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_high())
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok((*self).is_set_low())
    }
}