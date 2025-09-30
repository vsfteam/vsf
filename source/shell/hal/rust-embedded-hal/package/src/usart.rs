//! Universal Synchronous/Asynchronous Receiver Transmitter (USART/UART) driver.
//!
//! The USART/UART driver is provided in two flavors - this one and also [crate::buffered_usarte::BufferedUsarte].
//! The [Usarte] here is useful for those use-cases where reading the USART/UART peripheral is
//! exclusively awaited on. If the [Usart/Uart] is required to be awaited on with some other future,
//! for example when using `futures_util::future::select`, then you should consider
//! [crate::buffered_usart::BufferedUsart] so that reads may continue while processing these
//! other futures. If you do not then you may lose data between reads.
//!
//! An advantage of the [Usart/Uart] has over [crate::buffered_usart::BufferedUsart] is that less
//! memory may be used given that buffers are passed in directly to its read and write
//! methods.

#![macro_use]
#![allow(non_upper_case_globals)]
#![allow(dead_code)]

use core::future::poll_fn;
use core::marker::PhantomData;
use core::sync::atomic::{compiler_fence, Ordering, AtomicPtr, AtomicBool, AtomicU32};
use core::task::Poll;
use core::ptr;
use core::slice;
use paste::paste;

use embassy_embedded_hal::SetConfig;
//use embassy_hal_internal::drop::OnDrop;
use embassy_hal_internal::{Peri, PeripheralType};
use embassy_sync::waitqueue::AtomicWaker;

use crate::gpio::{AnyPin, AfType, Pull, Speed, OutputDrive};
use crate::interrupt::typelevel::Interrupt as _;
use crate::interrupt::{self, Interrupt, InterruptExt};
use crate::mode::{Async, Blocking, Mode};

use crate::vsf_hal::{*};

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Number of data bits
pub enum DataBits {
    /// 5 Data Bits
    #[cfg(VSF_USART_5_BIT_LENGTH)]
    DataBits5 = into_vsf_usart_mode_t!(VSF_USART_5_BIT_LENGTH) as isize,
    /// 6 Data Bits
    #[cfg(VSF_USART_6_BIT_LENGTH)]
    DataBits6 = into_vsf_usart_mode_t!(VSF_USART_6_BIT_LENGTH) as isize,
    /// 7 Data Bits
    #[cfg(VSF_USART_7_BIT_LENGTH)]
    DataBits7 = into_vsf_usart_mode_t!(VSF_USART_7_BIT_LENGTH) as isize,
    /// 8 Data Bits
    #[cfg(VSF_USART_8_BIT_LENGTH)]
    DataBits8 = into_vsf_usart_mode_t!(VSF_USART_8_BIT_LENGTH) as isize,
    /// 9 Data Bits
    #[cfg(VSF_USART_9_BIT_LENGTH)]
    DataBits9 = into_vsf_usart_mode_t!(VSF_USART_9_BIT_LENGTH) as isize,
    /// 10 Data Bits
    #[cfg(VSF_USART_10_BIT_LENGTH)]
    DataBits10 = into_vsf_usart_mode_t!(VSF_USART_10_BIT_LENGTH) as isize,
}

impl DataBits {
    pub fn into_value(&self) -> u8 {
        match self {
            #[cfg(VSF_USART_5_BIT_LENGTH)]
            DataBits::DataBits5 => 5,
            #[cfg(VSF_USART_6_BIT_LENGTH)]
            DataBits::DataBits6 => 6,
            #[cfg(VSF_USART_7_BIT_LENGTH)]
            DataBits::DataBits7 => 7,
            #[cfg(VSF_USART_8_BIT_LENGTH)]
            DataBits::DataBits8 => 8,
            #[cfg(VSF_USART_9_BIT_LENGTH)]
            DataBits::DataBits9 => 9,
            #[cfg(VSF_USART_10_BIT_LENGTH)]
            DataBits::DataBits10 => 10,
        }
    }
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Parity
pub enum Parity {
    /// No parity
    #[cfg(VSF_USART_NO_PARITY)]
    ParityNone = into_vsf_usart_mode_t!(VSF_USART_NO_PARITY) as isize,
    /// Even Parity
    #[cfg(VSF_USART_EVEN_PARITY)]
    ParityEven = into_vsf_usart_mode_t!(VSF_USART_EVEN_PARITY) as isize,
    /// Odd Parity
    #[cfg(VSF_USART_ODD_PARITY)]
    ParityOdd = into_vsf_usart_mode_t!(VSF_USART_ODD_PARITY) as isize,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Number of stop bits
pub enum StopBits {
    #[doc = "1 stop bit"]
    #[cfg(VSF_USART_1_STOPBIT)]
    STOP1 = into_vsf_usart_mode_t!(VSF_USART_1_STOPBIT) as isize,
    #[doc = "0.5 stop bits"]
    #[cfg(VSF_USART_0_5_STOPBIT)]
    STOP0P5 = into_vsf_usart_mode_t!(VSF_USART_0_5_STOPBIT) as isize,
    #[doc = "2 stop bits"]
    #[cfg(VSF_USART_2_STOPBIT)]
    STOP2 = into_vsf_usart_mode_t!(VSF_USART_2_STOPBIT) as isize,
    #[doc = "1.5 stop bits"]
    #[cfg(VSF_USART_1_5_STOPBIT)]
    STOP1P5 = into_vsf_usart_mode_t!(VSF_USART_1_5_STOPBIT) as isize,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Duplex mode
pub enum Duplex {
    /// Full duplex
    #[cfg(VSF_USART_HALF_DUPLEX_DISABLE)]
    Full = into_vsf_usart_mode_t!(VSF_USART_HALF_DUPLEX_DISABLE) as isize,
    /// Half duplex
    #[cfg(VSF_USART_HALF_DUPLEX_ENABLE)]
    Half = into_vsf_usart_mode_t!(VSF_USART_HALF_DUPLEX_ENABLE) as isize,

    #[cfg(not(any(VSF_USART_HALF_DUPLEX_ENABLE, VSF_USART_HALF_DUPLEX_DISABLE)))]
    Full = 0,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
/// Half duplex IO mode
pub enum OutputConfig {
    /// Push pull allows for faster baudrates, no internal pullup
    PushPull,
    /// Open drain output (external pull up needed)
    OpenDrain,
    /// Open drain output with internal pull up resistor
    OpenDrainPullUp,
}

impl OutputConfig {
    const fn af_type(self) -> AfType {
        match self {
            OutputConfig::PushPull => AfType::output(Speed::lowest(), OutputDrive::lowest()),
            OutputConfig::OpenDrain => AfType::output(Speed::lowest(), OutputDrive::lowest()),
            OutputConfig::OpenDrainPullUp => AfType::input_output(Pull::Up, Speed::lowest(), OutputDrive::lowest()),
        }
    }
}

/// USART config.
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[non_exhaustive]
pub struct Config {
    /// Baud rate.
    pub baudrate: u32,
    /// Number of data bits
    pub data_bits: DataBits,
    /// Number of stop bits
    pub stop_bits: StopBits,
    /// Parity type
    pub parity: Parity,

    /// Set this to true to swap the RX and TX pins.
    #[cfg(VSF_USART_SWAP)]
    pub swap_rx_tx: bool,

    /// Set this to true to invert TX pin signal values (V<sub>DD</sub> = 0/mark, Gnd = 1/idle).
    #[cfg(VSF_USART_TX_INVERT)]
    pub invert_tx: bool,

    /// Set this to true to invert RX pin signal values (V<sub>DD</sub> = 0/mark, Gnd = 1/idle).
    #[cfg(VSF_USART_RX_INVERT)]
    pub invert_rx: bool,

    /// Set this to true to invert RX pin signal values (V<sub>DD</sub> = 0/mark, Gnd = 1/idle).
    #[cfg(VSF_USART_SYNC_CLOCK_ENABLE)]
    pub enable_sync_clock: bool,

    /// Set the pull configuration for the RX pin.
    pub rx_pull: Pull,

    /// Set the pull configuration for the CTS pin.
    pub cts_pull: Pull,

    /// Set the pin configuration for the TX pin.
    pub tx_config: OutputConfig,

    /// Set the pin configuration for the RTS pin.
    pub rts_config: OutputConfig,

    /// Set the pin configuration for the DE pin.
    pub de_config: OutputConfig,

    /// Set the pin configuration for the CK pin.
    pub ck_config: OutputConfig,

    rx_timeout: u32,

    #[cfg(VSF_USART_IRQ_MASK_RX_IDLE)]
    rx_idle_cnt: u32,

    // private: set by internal API, not by the user.
    #[cfg(VSF_USART_HALF_DUPLEX_ENABLE)]
    duplex: Duplex,
}

#[non_exhaustive]
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Config Error
pub enum ConfigError {
    /// Baudrate too low
    BaudrateTooLow,
    /// Baudrate too high
    BaudrateTooHigh,
    /// Data bits not supported
    DatabitsNotSupported,
    /// Parity combination not supported
    ParityNotSupported,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            baudrate: 115200,
            data_bits: DataBits::DataBits8,
            stop_bits: StopBits::STOP1,
            parity: Parity::ParityNone,
            #[cfg(VSF_USART_SWAP)]
            swap_rx_tx: false,
            #[cfg(VSF_USART_TX_INVERT)]
            invert_tx: false,
            #[cfg(VSF_USART_RX_INVERT)]
            invert_rx: false,
            #[cfg(VSF_USART_SYNC_CLOCK_ENABLE)]
            enable_sync_clock: false,
            #[cfg(VSF_USART_HALF_DUPLEX_ENABLE)]
            duplex: Duplex::Full,
            rx_pull: Pull::None,
            cts_pull: Pull::None,
            tx_config: OutputConfig::PushPull,
            rts_config: OutputConfig::PushPull,
            de_config: OutputConfig::PushPull,
            ck_config: OutputConfig::PushPull,
            rx_timeout: 0,
            #[cfg(VSF_USART_IRQ_MASK_RX_IDLE)]
            rx_idle_cnt: 0,
        }
    }
}

#[cfg(VSF_USART_SWAP)]
impl Config {
    fn tx_af(&self) -> AfType {
        if self.swap_rx_tx {
            return AfType::input(Pull::None);
        };
        self.tx_config.af_type()
    }

    fn rx_af(&self) -> AfType {
        if self.swap_rx_tx {
            return self.tx_config.af_type();
        };
        AfType::input(Pull::None)
    }
}

/// USART error.
#[derive(Debug, Eq, PartialEq, Copy, Clone)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
#[non_exhaustive]
pub enum Error {
    /// Tx Buffer overrun
    #[cfg(VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR)]
    TxOverrun = into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR) as isize,
    /// Rx Buffer overrun
    #[cfg(VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR)]
    RxOverrun = into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR) as isize,
    /// Parity error
    #[cfg(VSF_USART_IRQ_MASK_PARITY_ERR)]
    Parity = into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_PARITY_ERR) as isize,
    /// Framing error
    #[cfg(VSF_USART_IRQ_MASK_FRAME_ERR)]
    Framing = into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_FRAME_ERR) as isize,
    /// Break condition
    #[cfg(VSF_USART_IRQ_MASK_BREAK_ERR)]
    Break = into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_BREAK_ERR) as isize,
}

impl core::fmt::Display for Error {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        let message = match self {
            #[cfg(VSF_USART_IRQ_MASK_FRAME_ERR)]
            Self::Framing => "Framing Error",
            #[cfg(VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR)]
            Self::RxOverrun => "RX Buffer Overrun",
            #[cfg(VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR)]
            Self::TxOverrun => "TX Buffer Overrun",
            #[cfg(VSF_USART_IRQ_MASK_PARITY_ERR)]
            Self::Parity => "Parity Check Error",
            #[cfg(VSF_USART_IRQ_MASK_BREAK_ERR)]
            Self::Break => "Break signal received",
        };

        write!(f, "{}", message)
    }
}

/// Interrupt handler.
pub struct InterruptHandler<T: Instance> {
    _phantom: PhantomData<T>,
}
pub struct BufferedInterruptHandler<T: Instance> {
    _uart: PhantomData<T>,
}

impl<T: Instance> interrupt::typelevel::Handler<T::Interrupt> for InterruptHandler<T> {
    unsafe fn on_interrupt() {
        unsafe {
            (T::info().vsf_usart_irqhandler)();
        }
    }
}
impl<T: Instance> interrupt::typelevel::Handler<T::Interrupt> for BufferedInterruptHandler<T> {
    unsafe fn on_interrupt() {
        unsafe {
            (T::info().vsf_usart_irqhandler)();
        }
    }
}

unsafe extern "C" fn vsf_usart_on_interrupt(
    target_ptr: *mut ::core::ffi::c_void,
    vsf_usart: *mut vsf_usart_t,
    irq_mask: into_enum_type!(vsf_usart_irq_mask_t),
) {
    let s = target_ptr as *const State;
    let s = unsafe { &*s };

    let mut rx_irq_mask = VSF_USART_IRQ_MASK_ERR | into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL);
    #[cfg(VSF_USART_IRQ_MASK_RX_IDLE)]
    if s.rx_exit_on_idle.load(Ordering::Relaxed) {
        rx_irq_mask |= into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_IDLE);
    }
    if rx_irq_mask & irq_mask != 0 {
        unsafe {
            vsf_usart_irq_disable(vsf_usart, rx_irq_mask);
        }
        s.rx_irq_mask.fetch_or(rx_irq_mask, Ordering::Relaxed);
        s.rx_waker.wake();
    }

    let tx_irq_mask = irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL);
    if tx_irq_mask != 0 {
        unsafe {
            vsf_usart_irq_disable(vsf_usart, into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL));
        }
        s.tx_irq_mask.fetch_or(tx_irq_mask, Ordering::Relaxed);
        s.tx_waker.wake();
    }
}

unsafe extern "C" fn vsf_usart_stream_on_error(
    target: *mut ::core::ffi::c_void,
    _stream: *mut vsf_usart_stream_t,
    irq_mask_err: vsf_usart_irq_mask_t,
) {
    let s = unsafe { &*(target as *const BufferedState) };
    s.rx_irq_mask.store(irq_mask_err, Ordering::Relaxed);
}

unsafe extern "C" fn vsf_usart_stream_rx_evthandler(
    _stream: *mut vsf_stream_t,
    param: *mut ::core::ffi::c_void,
    evt: vsf_stream_evt_t,
) {
    let s = unsafe { &*(param as *const BufferedState) };
    match evt {
        into_vsf_stream_evt_t!(VSF_STREAM_ON_CONNECT) => s.rx_waker.wake(),
        into_vsf_stream_evt_t!(VSF_STREAM_ON_DISCONNECT) => s.rx_waker.wake(),
        into_vsf_stream_evt_t!(VSF_STREAM_ON_IN) => s.rx_waker.wake(),
        _ => (),
    }
}

unsafe extern "C" fn vsf_usart_stream_tx_evthandler(
    _stream: *mut vsf_stream_t,
    param: *mut ::core::ffi::c_void,
    evt: vsf_stream_evt_t,
) {
    let s = unsafe { &*(param as *const BufferedState) };
    match evt {
        into_vsf_stream_evt_t!(VSF_STREAM_ON_CONNECT) => s.tx_waker.wake(),
        into_vsf_stream_evt_t!(VSF_STREAM_ON_DISCONNECT) => s.tx_waker.wake(),
        into_vsf_stream_evt_t!(VSF_STREAM_ON_OUT) => s.tx_waker.wake(),
        _ => (),
    }
}

/// USART driver.
pub struct Usart<M: Mode> {
    tx: UsartTx<M>,
    rx: UsartRx<M>,
}

impl<M: Mode> SetConfig for Usart<M> {
    type Config = Config;
    type ConfigError = ConfigError;

    fn set_config(&mut self, config: &Self::Config) -> Result<(), Self::ConfigError> {
        self._set_config(config)
    }
}

/// Transmitter part of the USART driver.
///
/// This can be obtained via [`Usart::split`], or created directly.
pub struct UsartTx<M: Mode> {
    info: &'static Info,
    state: &'static State,
    p: PhantomData<M>,
}

impl<M: Mode> SetConfig for UsartTx<M> {
    type Config = Config;
    type ConfigError = ConfigError;

    fn set_config(&mut self, config: &Self::Config) -> Result<(), Self::ConfigError> {
        self._set_config(config)
    }
}

/// Receiver part of the USART driver.
///
/// This can be obtained via [`Usart::split`], or created directly.
pub struct UsartRx<M: Mode> {
    info: &'static Info,
    state: &'static State,
    p: PhantomData<M>,
}

impl<M: Mode> SetConfig for UsartRx<M> {
    type Config = Config;
    type ConfigError = ConfigError;

    fn set_config(&mut self, config: &Self::Config) -> Result<(), Self::ConfigError> {
        self._set_config(config)
    }
}

/// Buffered UART driver.
pub struct BufferedUsart {
    rx: BufferedUsartRx,
    tx: BufferedUsartTx,
}

/// Buffered UART RX handle.
pub struct BufferedUsartRx {
    info: &'static Info,
    state: &'static State,
    buffered_state: &'static BufferedState,
}

/// Buffered UART TX handle.
pub struct BufferedUsartTx {
    info: &'static Info,
    state: &'static State,
    buffered_state: &'static BufferedState,
}

/// Configure vsf_usart.
/// 
/// config_fix is set by caller with Tx/Rx related configurations
fn _vsf_usart_config(info: &Info, state: &State, config: &Config, config_fix: u32, target_ptr: *mut ::core::ffi::c_void, irqhandler: vsf_usart_isr_handler_t) -> Result<(), ConfigError> {
    unsafe {
        let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
        let cap = vsf_usart_capability(vsf_usart);
        if (cap.min_baudrate != 0) && (config.baudrate < cap.min_baudrate) {
            return Err(ConfigError::BaudrateTooLow);
        }
        if (cap.max_baudrate != 0) && (config.baudrate > cap.max_baudrate) {
            return Err(ConfigError::BaudrateTooHigh);
        }
        let data_bits_value = config.data_bits.into_value();
        if data_bits_value < cap.min_data_bits || data_bits_value > cap.max_data_bits {
            return Err(ConfigError::DatabitsNotSupported);
        }

        let usart_config = &state._config as *const vsf_usart_cfg_t as *mut vsf_usart_cfg_t;
        state.config.store(&state._config as *const vsf_usart_cfg_t as *mut vsf_usart_cfg_t, Ordering::Relaxed);

        (*usart_config).mode = {
            config.data_bits as u32 | config.stop_bits as u32 | config.parity as u32 | config.duplex as u32 | config_fix | {
                let mut mode: u32 = 0;
                #[cfg(VSF_USART_SWAP)]
                if config.swap_rx_tx {
                    mode |= into_vsf_usart_mode_t!(VSF_USART_SWAP) as u32;
                }
                #[cfg(VSF_USART_SYNC_CLOCK_ENABLE)]
                if config.enable_sync_clock {
                    mode |= into_vsf_usart_mode_t!(VSF_USART_SYNC_CLOCK_ENABLE) as u32;
                }
                mode
            }
        };
        (*usart_config).baudrate = config.baudrate;
        (*usart_config).rx_timeout = config.rx_timeout;
        #[cfg(VSF_USART_IRQ_MASK_RX_IDLE)]
        {
            (*usart_config).rx_idle_cnt = config.rx_idle_cnt;
        }
        (*usart_config).isr.handler_fn = irqhandler;
        (*usart_config).isr.target_ptr = target_ptr;

        vsf_usart_init(vsf_usart, usart_config);
    }
    Ok(())
}

fn _vsf_usart_config_and_enable(info: &Info, state: &State, config: &Config, config_fix: u32, target_ptr: *mut ::core::ffi::c_void, irqhandler: vsf_usart_isr_handler_t) -> Result<(), ConfigError> {
    _vsf_usart_config(info, state, config, config_fix, target_ptr, irqhandler)?;
    unsafe {
        let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
        vsf_usart_enable(vsf_usart);

        info.interrupt.unpend();
        info.interrupt.enable();
    }
    Ok(())
}

fn _vsf_usart_check_error(irq_mask: vsf_usart_irq_mask_t) -> Result<(), Error> {
    #[cfg(VSF_USART_IRQ_MASK_FRAME_ERR)]
    if irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_FRAME_ERR) != 0 {
        return Err(Error::Framing);
    }
    #[cfg(VSF_USART_IRQ_MASK_PARITY_ERR)]
    if irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_PARITY_ERR) != 0 {
        return Err(Error::Parity);
    }
    #[cfg(VSF_USART_IRQ_MASK_BREAK_ERR)]
    if irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_BREAK_ERR) != 0 {
        return Err(Error::Break);
    }
    #[cfg(VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR)]
    if irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR) != 0 {
        return Err(Error::RxOverrun);
    }
    #[cfg(VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR)]
    if irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR) != 0 {
        return Err(Error::TxOverrun);
    }

    Ok(())
}

impl<'d> Usart<Async> {
    /// Create a new USART without hardware flow control
    pub fn new<T: Instance>(
        _usart: Peri<'d, T>,
        rxd: Peri<'d, impl TxPin<T>>,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            None, None, None,
            config
        )
    }

    /// Create a new USART with hardware flow control (RTS/CTS)
    pub fn new_with_rtscts<T: Instance>(
        _usart: Peri<'d, T>,
        rxd: Peri<'d, impl RxPin<T>>,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        cts: Peri<'d, impl CtsPin<T>>,
        rts: Peri<'d, impl RtsPin<T>>,
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            new_pin!(rts, config.rts_config.af_type()),
            new_pin!(cts, AfType::input(config.cts_pull)),
            None,
            config
        )
    }

    /// Write all bytes in the buffer.
    pub async fn write(&mut self, buffer: &[u8]) -> Result<(), Error> {
        self.tx.write(buffer).await
    }

    /// Wait until transmission complete
    pub async fn flush(&mut self) -> Result<(), Error> {
        self.tx.flush().await
    }

    /// Read bytes until the buffer is filled.
    pub async fn read(&mut self, buffer: &mut [u8]) -> Result<(), Error> {
        self.rx.read(buffer).await
    }

    /// Perform an an asynchronous read with idle line detection enabled
    pub async fn read_until_idle(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
        self.rx.read_until_idle(buffer).await
    }
}

impl<'d> Usart<Blocking> {
    /// Create a new USART without hardware flow control
    pub fn new_blocking<T: Instance>(
        _usart: Peri<'d, T>,
        rxd: Peri<'d, impl TxPin<T>>,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            None, None, None,
            config
        )
    }

    /// Create a new USART with hardware flow control (RTS/CTS)
    pub fn new_blocking_with_rtscts<T: Instance>(
        _usart: Peri<'d, T>,
        rxd: Peri<'d, impl RxPin<T>>,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        cts: Peri<'d, impl CtsPin<T>>,
        rts: Peri<'d, impl RtsPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            new_pin!(rts, config.rts_config.af_type()),
            new_pin!(cts, AfType::input(config.cts_pull)),
            None,
            config
        )
    }
}

impl<'d, M: Mode> Usart<M> {
    fn new_inner<T: Instance>(
        _rxd: Option<Peri<'d, AnyPin>>,
        _txd: Option<Peri<'d, AnyPin>>,
        _ck: Option<Peri<'d, AnyPin>>,
        _cts: Option<Peri<'d, AnyPin>>,
        _rts: Option<Peri<'d, AnyPin>>,
        _de: Option<Peri<'d, AnyPin>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        #[cfg(all(not(VSF_USART_TX_INVERT), not(VSF_USART_RX_INVERT)))]
        let mode = 0;
        #[cfg(any(VSF_USART_TX_INVERT, VSF_USART_RX_INVERT))]
        let mode = {
            let mut mode: u32 = 0;
            #[cfg(VSF_USART_TX_INVERT)]
            if config.invert_tx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_TX_INVERT) as u32;
            }
            #[cfg(VSF_USART_RX_INVERT)]
            if config.invert_rx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_RX_INVERT) as u32;
            }
            mode
        };
        let info = T::info();
        let s = T::state();
        _vsf_usart_config_and_enable(info, s, &config, mode, ptr::from_ref(info) as *mut ::core::ffi::c_void, Some(vsf_usart_on_interrupt))?;

        Ok(Self {
            tx: UsartTx {
                info: info,
                state: s,
                p: PhantomData,
            },
            rx: UsartRx {
                info: info,
                state: s,
                p: PhantomData,
            },
        })
    }

    pub fn into_buffered<T: Instance>(self, 
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, BufferedInterruptHandler<T>> + 'd,
        tx_buffer: &'d mut [u8],
        rx_buffer: &'d mut [u8],
    ) -> BufferedUsart {
        let info = T::info();
        let s = T::state();
        let bs = T::buffered_state();

        if bs.usart_stream.load(Ordering::Relaxed) == 0 as *mut UsartStream {
            bs.usart_stream.store(&bs._usart_stream_instance as *const UsartStream as *mut UsartStream, Ordering::Relaxed);
            let usart_stream = &bs._usart_stream_instance as *const UsartStream as *mut UsartStream;
            unsafe {
                (*usart_stream).usart_stream.usart = info.vsf_usart.load(Ordering::Relaxed);
                (*usart_stream).usart_stream.target = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;

                (*usart_stream).usart_stream.stream_rx = &bs._usart_stream_instance.stream_rx as *const vsf_fifo_stream_t as *const vsf_stream_t as *mut vsf_stream_t;
                (*usart_stream).stream_rx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.rx.param = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;
                (*usart_stream).stream_rx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.rx.evthandler = Some(vsf_usart_stream_rx_evthandler);
                (*usart_stream).stream_rx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.buffer = rx_buffer.as_mut_ptr();
                (*usart_stream).stream_rx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.size = rx_buffer.len() as u32;

                (*usart_stream).usart_stream.stream_tx = &bs._usart_stream_instance.stream_tx as *const vsf_fifo_stream_t as *const vsf_stream_t as *mut vsf_stream_t;
                (*usart_stream).stream_tx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.tx.param = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;
                (*usart_stream).stream_tx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.tx.evthandler = Some(vsf_usart_stream_tx_evthandler);
                (*usart_stream).stream_tx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.buffer = tx_buffer.as_mut_ptr();
                (*usart_stream).stream_tx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.size = tx_buffer.len() as u32;

                let usart_config = s.config.load(Ordering::Relaxed);
                vsf_usart_stream_init(&((*usart_stream).usart_stream) as *const vsf_usart_stream_t as *mut vsf_usart_stream_t, usart_config);
            }
        }

        BufferedUsart {
            rx: BufferedUsartRx {
                info: info,
                state: s,
                buffered_state: bs,
            },
            tx: BufferedUsartTx {
                info: info,
                state: s,
                buffered_state: bs,
            },
        }
    }

    /// Set configuration
    pub fn _set_config(&mut self, config: &Config) -> Result<(), ConfigError> {
        #[cfg(all(not(VSF_USART_TX_INVERT), not(VSF_USART_RX_INVERT)))]
        let mode = 0;
        #[cfg(any(VSF_USART_TX_INVERT, VSF_USART_RX_INVERT))]
        let mode = {
            let mut mode: u32 = 0;
            #[cfg(VSF_USART_TX_INVERT)]
            if config.invert_tx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_TX_INVERT) as u32;
            }
            #[cfg(VSF_USART_RX_INVERT)]
            if config.invert_rx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_RX_INVERT) as u32;
            }
            mode
        };
        let info = self.tx.info;
        let s = self.tx.state;
        _vsf_usart_config(info, s, config, mode, ptr::from_ref(s) as *mut ::core::ffi::c_void, Some(vsf_usart_on_interrupt))
    }

    /// Read bytes until the buffer is filled.
    pub fn blocking_read(&mut self, buffer: &mut [u8]) -> Result<(), Error> {
        self.rx.blocking_read(buffer)
    }

    /// Write all bytes in the buffer.
    pub fn blocking_write(&mut self, buffer: &[u8]) -> Result<(), Error> {
        self.tx.blocking_write(buffer)
    }

    /// Block until transmission complete
    pub fn blocking_flush(&mut self) -> Result<(), Error> {
        self.tx.blocking_flush()
    }

    /// Send break character
    pub fn send_break(&mut self) {
        self.tx.send_break();
    }

    /// Split the Usart into the transmitter and receiver parts.
    ///
    /// This is useful to concurrently transmit and receive from independent tasks.
    pub fn split(self) -> (UsartTx<M>, UsartRx<M>) {
        (self.tx, self.rx)
    }

    /// Split the USART in reader and writer parts, by reference.
    ///
    /// The returned halves borrow from `self`, so you can drop them and go back to using
    /// the "un-split" `self`. This allows temporarily splitting the USART.
    pub fn split_by_ref(&mut self) -> (&mut UsartTx<M>, &mut UsartRx<M>) {
        (&mut self.tx, &mut self.rx)
    }
}

impl<'d> UsartTx<Async> {
    /// Create a new tx-only USART without hardware flow control
    pub fn new<T: Instance>(
        _usart: Peri<'d, T>,
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            None,
            config
        )
    }

    /// Create a new tx-only USART with hardware flow control (RTS/CTS)
    pub fn new_with_cts<T: Instance>(
        _usart: Peri<'d, T>,
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        cts: Peri<'d, impl CtsPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            new_pin!(cts, AfType::input(config.cts_pull)),
            config
        )
    }

    /// Write all bytes in the buffer.
    pub async fn write(&mut self, buffer: &[u8]) -> Result<(), Error> {
        if buffer.is_empty() {
            return Ok(());
        }

        let s = self.state;
        compiler_fence(Ordering::SeqCst);

        unsafe {
            let info = self.info;
            let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
            let ptr = buffer.as_ptr();
            let len = buffer.len();

            vsf_usart_irq_enable(vsf_usart, into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL));
            vsf_usart_request_tx(vsf_usart, ptr as *mut ::core::ffi::c_void, len as uint_fast32_t);
        }

        poll_fn(|cx| {
            s.tx_waker.register(cx.waker());
            if s.tx_irq_mask.swap(0, Ordering::Relaxed) & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL) != 0 {
                return Poll::Ready(());
            }
            Poll::Pending
        }).await;

        Ok(())
    }

    /// Wait until transmission complete
    pub async fn flush(&mut self) -> Result<(), Error> {
        // TODO:
        Ok(())
    }
}

impl<'d> UsartTx<Blocking> {
    /// Create a new tx-only USART without hardware flow control
    pub fn new_blocking<T: Instance>(
        _usart: Peri<'d, T>,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            None,
            config
        )
    }

    /// Create a new tx-only USART with hardware flow control (RTS/CTS)
    pub fn new_blocking_with_cts<T: Instance>(
        _usart: Peri<'d, T>,
        txd: Peri<'d, impl TxPin<T>>,
        ck: Option<Peri<'d, impl CkPin<T>>>,
        cts: Peri<'d, impl CtsPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(txd, config.tx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(txd, config.tx_config.af_type()),
            if let Some(ck_pin) = ck {
                new_pin!(ck_pin, AfType::output(Speed::highest(), OutputDrive::default()))
            } else {
                None
            },
            new_pin!(cts, AfType::input(config.cts_pull)),
            config
        )
    }
}

impl<'d, M: Mode> UsartTx<M> {
    fn new_inner<T: Instance>(
        _txd: Option<Peri<'d, AnyPin>>,
        _ck: Option<Peri<'d, AnyPin>>,
        _cts: Option<Peri<'d, AnyPin>>,
        config: Config
    ) -> Result<Self, ConfigError> {
        #[cfg(not(VSF_USART_TX_INVERT))]
        let mode = into_vsf_usart_mode_t!(VSF_USART_TX_ENABLE) as u32;
        #[cfg(VSF_USART_TX_INVERT)]
        let mode = {
            let mut mode: u32 = into_vsf_usart_mode_t!(VSF_USART_TX_ENABLE) as u32;
            #[cfg(VSF_USART_TX_INVERT)]
            if config.invert_tx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_TX_INVERT) as u32;
            }
            mode
        };
        let info = T::info();
        let s = T::state();
        _vsf_usart_config_and_enable(info, s, &config, mode, ptr::from_ref(info) as *mut ::core::ffi::c_void, Some(vsf_usart_on_interrupt))?;

        Ok(Self {
            info: info,
            state: s,
            p: PhantomData,
        })
    }

    pub fn into_buffered<T: Instance>(self, 
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, BufferedInterruptHandler<T>> + 'd,
        tx_buffer: &'d mut [u8],
    ) -> BufferedUsartTx {
        let info = self.info;
        let s = T::state();
        let bs = T::buffered_state();

        if bs.usart_stream.load(Ordering::Relaxed) == 0 as *mut UsartStream {
            bs.usart_stream.store(&bs._usart_stream_instance as *const UsartStream as *mut UsartStream, Ordering::Relaxed);
            let usart_stream = &bs._usart_stream_instance as *const UsartStream as *mut UsartStream;
            unsafe {
                (*usart_stream).usart_stream.usart = info.vsf_usart.load(Ordering::Relaxed);
                (*usart_stream).usart_stream.target = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;

                (*usart_stream).usart_stream.stream_tx = &bs._usart_stream_instance.stream_tx as *const vsf_fifo_stream_t as *const vsf_stream_t as *mut vsf_stream_t;
                (*usart_stream).stream_tx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.tx.param = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;
                (*usart_stream).stream_tx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.tx.evthandler = Some(vsf_usart_stream_tx_evthandler);
                (*usart_stream).stream_tx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.buffer = tx_buffer.as_mut_ptr();
                (*usart_stream).stream_tx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.size = tx_buffer.len() as u32;

                (*usart_stream).usart_stream.stream_rx = 0 as *mut vsf_stream_t;

                let usart_config = s.config.load(Ordering::Relaxed);
                vsf_usart_stream_init(&((*usart_stream).usart_stream) as *const vsf_usart_stream_t as *mut vsf_usart_stream_t, usart_config);
            }
        }

        BufferedUsartTx {
            info: info,
            state: s,
            buffered_state: bs,
        }
    }

    /// Set configuration
    pub fn _set_config(&mut self, config: &Config) -> Result<(), ConfigError> {
        #[cfg(not(VSF_USART_TX_INVERT))]
        let mode = into_vsf_usart_mode_t!(VSF_USART_TX_ENABLE) as u32;
        #[cfg(VSF_USART_TX_INVERT)]
        let mode = {
            let mut mode: u32 = into_vsf_usart_mode_t!(VSF_USART_TX_ENABLE) as u32;
            #[cfg(VSF_USART_TX_INVERT)]
            if config.invert_tx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_TX_INVERT) as u32;
            }
            mode
        };
        let info = self.info;
        let s = self.state;
        _vsf_usart_config(info, s, config, mode, ptr::from_ref(s) as *mut ::core::ffi::c_void, Some(vsf_usart_on_interrupt))
    }

    /// Write all bytes in the buffer.
    pub fn blocking_write(&mut self, buffer: &[u8]) -> Result<(), Error> {
        if buffer.is_empty() {
            return Ok(());
        }

        let s = self.state;
        compiler_fence(Ordering::SeqCst);

        unsafe {
            let info = self.info;
            let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
            let ptr = buffer.as_ptr();
            let len = buffer.len();

            vsf_usart_irq_enable(vsf_usart, into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL));
            vsf_usart_request_tx(vsf_usart, ptr as *mut ::core::ffi::c_void, len as uint_fast32_t);
        }

        while s.tx_irq_mask.swap(0, Ordering::Relaxed) & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL) == 0 {}
        Ok(())
    }

    /// Block until transmission complete
    pub fn blocking_flush(&mut self) -> Result<(), Error> {
        // TODO:
        Ok(())
    }

    /// Send break character
    pub fn send_break(&mut self) {
        // TODO: implement send_break with VSF_USART_CTRL_SEND_BREAK or with VSF_USART_CTRL_SET_BREAK and VSF_USART_CTRL_CLEAR_BREAK
        #[cfg(VSF_USART_CTRL_SEND_BREAK)]
        unsafe {
            let info = self.info;
            let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
            // 1. wait tx fifo empty
            self.blocking_flush().unwrap();
            // 2. TODO: wait previous send_break done
            // 3. send current break
            vsf_usart_ctrl(vsf_usart, into_vsf_usart_ctrl_t!(VSF_USART_CTRL_SEND_BREAK), 0 as *mut ::core::ffi::c_void);
        }
        #[cfg(all(not(VSF_USART_CTRL_SEND_BREAK), all(VSF_USART_CTRL_SET_BREAK, VSF_USART_CTRL_CLEAR_BREAK)))]
        unsafe {
            let info = self.info;
            let s = self.state;
            let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
            let usart_config = s.config.load(Ordering::Relaxed);

            // 1. calculate duration for break condition for at least 2 frames
            let bits = 2 * (1 + 10 + 1 + 1);
            let wait_usec = 1_000_000 * bits as u64 / usart_config.baudrate;

            // 2. wait tx fifo empty
            self.blocking_flush().unwrap();

            // 3. set break
            vsf_usart_ctrl(vsf_usart, into_vsf_usart_ctrl_t!(VSF_USART_CTRL_SET_BREAK), 0 as *mut ::core::ffi::c_void);

            Timer::after_Micros(wait_usec).await;

            // 5. clear break
            vsf_usart_ctrl(vsf_usart, into_vsf_usart_ctrl_t!(VSF_USART_CTRL_CLEAR_BREAK), 0 as *mut ::core::ffi::c_void);
        }
    }
}

impl<M: Mode> Drop for UsartTx<M> {
    fn drop(&mut self) {
        // TODO
    }
}

impl<'d> UsartRx<Async> {
    /// Create a new rx-only USART without hardware flow control
    pub fn new<T: Instance>(
        _usart: Peri<'d, T>,
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
        rxd: Peri<'d, impl TxPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            None,
            config
        )
    }

    /// Create a new rx-only USART with hardware flow control (RTS/CTS)
    pub fn new_with_rts<T: Instance>(
        _usart: Peri<'d, T>,
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
        rxd: Peri<'d, impl TxPin<T>>,
        rts: Peri<'d, impl RtsPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            new_pin!(rts, config.rts_config.af_type()),
            config
        )
    }

    /// Read bytes until the buffer is filled.
    pub async fn read(&mut self, buffer: &mut [u8]) -> Result<(), Error> {
        if buffer.is_empty() {
            return Ok(());
        }

        let s= self.state;
        s.rx_exit_on_idle.store(false, Ordering::Relaxed);
        compiler_fence(Ordering::SeqCst);

        unsafe {
            let info = self.info;
            let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
            let ptr = buffer.as_ptr();
            let len = buffer.len();

            vsf_usart_irq_enable(vsf_usart, into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_TX_CPL));
            vsf_usart_request_rx(vsf_usart, ptr as *mut ::core::ffi::c_void, len as uint_fast32_t);
        }

        let result = poll_fn(|cx| {
            s.rx_waker.register(cx.waker());
            let rx_irq_mask = s.rx_irq_mask.swap(0, Ordering::Relaxed);
            if let Err(error) = _vsf_usart_check_error(rx_irq_mask) {
                return Poll::Ready(Err(error));
            } else if rx_irq_mask & into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL) != 0 {
                return Poll::Ready(Ok(()));
            }
            Poll::Pending
        }).await;
        result
    }

    /// Read bytes until the buffer is filled, or the line becomes idle.
    ///
    /// Returns the amount of bytes read.
    pub async fn read_until_idle(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
        if buffer.is_empty() {
            return Ok(0);
        }

        let s = self.state;
        let info = self.info;
        let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
        s.rx_exit_on_idle.store(true, Ordering::Relaxed);
        compiler_fence(Ordering::SeqCst);

        let irq_mask = VSF_USART_IRQ_MASK_ERR | into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL) | into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_IDLE);
        unsafe {
            let ptr = buffer.as_ptr();
            let len = buffer.len();

            vsf_usart_irq_enable(vsf_usart, irq_mask);
            vsf_usart_request_rx(vsf_usart, ptr as *mut ::core::ffi::c_void, len as uint_fast32_t);
        }

        let result = poll_fn(|cx| {
            s.rx_waker.register(cx.waker());

            let rx_irq_mask = s.rx_irq_mask.swap(0, Ordering::Relaxed);
            if let Err(error) = _vsf_usart_check_error(rx_irq_mask) {
                return Poll::Ready(Err(error));
            } else if rx_irq_mask & (into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL) | into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_IDLE)) != 0 {
                return Poll::Ready(Ok(()));
            }

            Poll::Pending
        }).await;

        unsafe {
            let n = vsf_usart_get_rx_count(vsf_usart) as usize;
            result.map(|_| n)
        }
    }
}

impl<'d> UsartRx<Blocking> {
    /// Create a new rx-only USART without hardware flow control
    pub fn new_blocking<T: Instance>(
        _usart: Peri<'d, T>,
        rxd: Peri<'d, impl TxPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            None,
            config
        )
    }

    /// Create a new rx-only USART with hardware flow control (RTS/CTS)
    pub fn new_blocking_with_rts<T: Instance>(
        _usart: Peri<'d, T>,
        rxd: Peri<'d, impl TxPin<T>>,
        rts: Peri<'d, impl RtsPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner::<T>(
            #[cfg(VSF_USART_SWAP)]
            new_pin!(rxd, config.rx_af()),
            #[cfg(not(VSF_USART_SWAP))]
            new_pin!(rxd, AfType::input(Pull::None)),
            new_pin!(rts, config.rts_config.af_type()),
            config
        )
    }
}

impl<'d, M: Mode> UsartRx<M> {
    fn new_inner<T: Instance>(
        _rxd: Option<Peri<'d, AnyPin>>,
        _rts: Option<Peri<'d, AnyPin>>,
        config: Config
    ) -> Result<Self, ConfigError> {
        #[cfg(not(VSF_USART_RX_INVERT))]
        let mode = into_vsf_usart_mode_t!(VSF_USART_RX_ENABLE) as u32;
        #[cfg(VSF_USART_RX_INVERT)]
        let mode = {
            let mut mode: u32 = into_vsf_usart_mode_t!(VSF_USART_RX_ENABLE) as u32;
            #[cfg(VSF_USART_RX_INVERT)]
            if config.invert_rx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_RX_INVERT) as u32;
            }
            mode
        };
        let info = T::info();
        let s = T::state();
        _vsf_usart_config_and_enable(info, s, &config, mode, ptr::from_ref(info) as *mut ::core::ffi::c_void, Some(vsf_usart_on_interrupt))?;

        Ok(Self {
            info: info,
            state: s,
            p: PhantomData,
        })
    }

    pub fn into_buffered<T: Instance>(self, 
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, BufferedInterruptHandler<T>> + 'd,
        rx_buffer: &'d mut [u8],
    ) -> BufferedUsartRx {
        let info = self.info;
        let s = self.state;
        let bs = T::buffered_state();

        if bs.usart_stream.load(Ordering::Relaxed) == 0 as *mut UsartStream {
            bs.usart_stream.store(&bs._usart_stream_instance as *const UsartStream as *mut UsartStream, Ordering::Relaxed);
            let usart_stream = &bs._usart_stream_instance as *const UsartStream as *mut UsartStream;
            unsafe {
                (*usart_stream).usart_stream.usart = info.vsf_usart.load(Ordering::Relaxed);
                (*usart_stream).usart_stream.target = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;

                (*usart_stream).usart_stream.stream_rx = &bs._usart_stream_instance.stream_rx as *const vsf_fifo_stream_t as *const vsf_stream_t as *mut vsf_stream_t;
                (*usart_stream).stream_rx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.rx.param = ptr::addr_of!(*bs) as *mut ::core::ffi::c_void;
                (*usart_stream).stream_rx.__bindgen_anon_1.use_as__vsf_stream_t.__bindgen_anon_1.__bindgen_anon_1.rx.evthandler = Some(vsf_usart_stream_rx_evthandler);
                (*usart_stream).stream_rx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.buffer = rx_buffer.as_mut_ptr();
                (*usart_stream).stream_rx.__bindgen_anon_2.use_as__vsf_byte_fifo_t.size = rx_buffer.len() as u32;

                (*usart_stream).usart_stream.stream_tx = 0 as *mut vsf_stream_t;

                let usart_config = s.config.load(Ordering::Relaxed);
                vsf_usart_stream_init(&((*usart_stream).usart_stream) as *const vsf_usart_stream_t as *mut vsf_usart_stream_t, usart_config);
            }
        }

        BufferedUsartRx {
            info: info,
            state: s,
            buffered_state: bs,
        }
    }

    /// Set configuration
    pub fn _set_config(&mut self, config: &Config) -> Result<(), ConfigError> {
        #[cfg(not(VSF_USART_RX_INVERT))]
        let mode = into_vsf_usart_mode_t!(VSF_USART_RX_ENABLE) as u32;
        #[cfg(VSF_USART_RX_INVERT)]
        let mode = {
            let mut mode: u32 = into_vsf_usart_mode_t!(VSF_USART_RX_ENABLE) as u32;
            #[cfg(VSF_USART_RX_INVERT)]
            if config.invert_rx {
                mode |= into_vsf_usart_mode_t!(VSF_USART_RX_INVERT) as u32;
            }
            mode
        };
        let info = self.info;
        let s = self.state;
        _vsf_usart_config(info, s, config, mode, ptr::from_ref(s) as *mut ::core::ffi::c_void, Some(vsf_usart_on_interrupt))
    }

    /// Read bytes until the buffer is filled.
    pub fn blocking_read(&mut self, buffer: &mut [u8]) -> Result<(), Error> {
        if buffer.is_empty() {
            return Ok(());
        }

        let s = self.state;
        s.rx_exit_on_idle.store(false, Ordering::Relaxed);
        compiler_fence(Ordering::SeqCst);

        unsafe {
            let info = self.info;
            let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
            let ptr = buffer.as_ptr();
            let len = buffer.len();

            vsf_usart_irq_enable(vsf_usart, into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL) | VSF_USART_IRQ_MASK_ERR);
            vsf_usart_request_rx(vsf_usart, ptr as *mut ::core::ffi::c_void, len as uint_fast32_t);
        }

        let mut rx_irq_mask: u32 = 0;
        loop {
            rx_irq_mask = s.rx_irq_mask.swap(0, Ordering::Relaxed);
            if rx_irq_mask & (into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL) | VSF_USART_IRQ_MASK_ERR) != 0 {
                break;
            }
        }

        if let Err(error) = _vsf_usart_check_error(rx_irq_mask) {
            return Err(error);
        }
        return Ok(());
    }

    /// Read bytes until the buffer is filled, or the line becomes idle.
    ///
    /// Returns the amount of bytes read.
    pub fn blocking_read_until_idle(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
        if buffer.is_empty() {
            return Ok(0);
        }

        let s = self.state;
        let info = self.info;
        let vsf_usart = info.vsf_usart.load(Ordering::Relaxed);
        s.rx_exit_on_idle.store(true, Ordering::Relaxed);
        compiler_fence(Ordering::SeqCst);

        let irq_mask = VSF_USART_IRQ_MASK_ERR | into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_CPL) | into_vsf_usart_irq_mask_t!(VSF_USART_IRQ_MASK_RX_IDLE);
        unsafe {
            let ptr = buffer.as_ptr();
            let len = buffer.len();

            vsf_usart_irq_enable(vsf_usart, irq_mask);
            vsf_usart_request_rx(vsf_usart, ptr as *mut ::core::ffi::c_void, len as uint_fast32_t);
        }

        let mut rx_irq_mask: u32 = 0;
        loop {
            rx_irq_mask = s.rx_irq_mask.swap(0, Ordering::Relaxed);
            if rx_irq_mask & irq_mask != 0 {
                break;
            }
        }

        if let Err(error) = _vsf_usart_check_error(rx_irq_mask) {
            return Err(error);
        }
        Ok(unsafe { vsf_usart_get_rx_count(vsf_usart) as usize })
    }
}

impl<M: Mode> Drop for UsartRx<M> {
    fn drop(&mut self) {
        // TODO
    }
}

impl<'d> BufferedUsart {
    pub fn into_normal<T: Instance>(self, 
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
    ) -> Usart<Async> {
        Usart {
            tx: UsartTx {
                info: self.tx.info,
                state: self.tx.state,
                p: PhantomData,
            },
            rx: UsartRx {
                info: self.rx.info,
                state: self.rx.state,
                p: PhantomData,
            },
        }
    }

    /// Write to UART TX buffer blocking execution until done.
    pub fn blocking_write(&mut self, buffer: &[u8]) -> Result<usize, Error> {
        self.tx.blocking_write(buffer)
    }

    /// Flush UART TX blocking execution until done.
    pub fn blocking_flush(&mut self) -> Result<(), Error> {
        self.tx.blocking_flush()
    }

    /// Read from UART RX buffer blocking execution until done.
    pub fn blocking_read(&mut self, buffer: &'d mut [u8]) -> Result<usize, Error> {
        self.rx.blocking_read(buffer)
    }

    /// Split into separate RX and TX handles.
    pub fn split(self) -> (BufferedUsartTx, BufferedUsartRx) {
        (self.tx, self.rx)
    }

    /// Split the Uart into a transmitter and receiver by mutable reference,
    /// which is particularly useful when having two tasks correlating to
    /// transmitting and receiving.
    pub fn split_ref(&mut self) -> (&mut BufferedUsartTx, &mut BufferedUsartRx) {
        (&mut self.tx, &mut self.rx)
    }
}

impl<'d> BufferedUsartRx {
    pub fn into_normal<T: Instance>(self, 
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
    ) -> UsartRx<Async> {
        UsartRx {
            info: self.info,
            state: self.state,
            p: PhantomData,
        }
    }

    async fn read(&self, buffer: &'d mut [u8]) -> impl Future<Output = Result<usize, Error>> + 'd {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_rx = unsafe { &((*usart_stream).stream_rx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        poll_fn(move |cx| {
            let n = unsafe { vsf_stream_read(stream_rx, buffer.as_mut_ptr(), buffer.len() as u32) };
            if n == 0 {
                let rx_irq_mask = (*s).rx_irq_mask.load(Ordering::Relaxed);
                return match _vsf_usart_check_error(rx_irq_mask) {
                    Err(e) => Poll::Ready(Err(e)),
                    Ok(()) => { s.rx_waker.register(cx.waker()); Poll::Pending },
                }
            } else {
                Poll::Ready(Ok(n as usize))
            }
        })
    }

    /// Read from UART RX buffer blocking execution until done.
    pub fn blocking_read(&mut self, buffer: &'d mut [u8]) -> Result<usize, Error> {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_rx = unsafe { &((*usart_stream).stream_rx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        loop { unsafe {
            let n = vsf_stream_read(stream_rx, buffer.as_mut_ptr(), buffer.len() as u32);
            if n == 0 {
                let rx_irq_mask = s.rx_irq_mask.load(Ordering::Relaxed);
                match _vsf_usart_check_error(rx_irq_mask) {
                    Err(e) => return Err(e),
                    Ok(()) => continue,
                }
            } else {
                return Ok(n as usize);
            }
        }}
    }

    async fn fill_buf(&self) -> impl Future<Output = Result<&'d [u8], Error>> {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_rx = unsafe { &((*usart_stream).stream_rx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        poll_fn(move |cx| {
            let mut ptr: *mut u8 = ptr::null_mut();
            let n = unsafe { vsf_stream_get_rbuf(stream_rx, &mut ptr) };
            if n == 0 {
                let rx_irq_mask = s.rx_irq_mask.load(Ordering::Relaxed);
                return match _vsf_usart_check_error(rx_irq_mask) {
                    Err(e) => Poll::Ready(Err(e)),
                    Ok(()) => { s.rx_waker.register(cx.waker()); Poll::Pending },
                }
            } else {
                let buf = unsafe { slice::from_raw_parts(ptr, n as usize) };
                return Poll::Ready(Ok(buf));
            }
        })
    }

    fn consume(&self, amt: usize) {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_rx = unsafe { &((*usart_stream).stream_rx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        unsafe { vsf_stream_read(stream_rx, ptr::null_mut() as *mut u8, amt as u32); }
    }

    fn read_ready(&mut self) -> Result<bool, Error> {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_rx = unsafe { &((*usart_stream).stream_rx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        Ok(unsafe {vsf_stream_get_data_size(stream_rx) > 0 })
    }
}

impl<'d> BufferedUsartTx {
    pub fn into_normal<T: Instance>(self, 
        _irq: impl interrupt::typelevel::Binding<T::Interrupt, InterruptHandler<T>> + 'd,
    ) -> UsartTx<Async> {
        UsartTx {
            info: self.info,
            state: self.state,
            p: PhantomData,
        }
    }

    async fn write(&self, buffer: &'d [u8]) -> impl Future<Output = Result<usize, Error>> + 'd {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_tx = unsafe { &((*usart_stream).stream_tx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        poll_fn(move |cx| {
            let n = unsafe { vsf_stream_write(stream_tx, buffer.as_ptr() as *mut u8, buffer.len() as u32) };
            if n == 0 {
                s.tx_waker.register(cx.waker());
                Poll::Pending
            } else {
                Poll::Ready(Ok(n as usize))
            }
        })
    }

    async fn flush(&self) -> impl Future<Output = Result<(), Error>> {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_tx = unsafe { &((*usart_stream).stream_tx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        poll_fn(move |cx| {
            let n = unsafe { vsf_stream_get_data_size(stream_tx) };
            if n == 0 {
                Poll::Ready(Ok(()))
            } else {
                s.tx_waker.register(cx.waker());
                Poll::Pending
            }
        })
    }

    fn blocking_write(&self, buffer: &'d [u8]) -> Result<usize, Error> {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_tx = unsafe { &((*usart_stream).stream_tx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        loop { unsafe {
            let n = vsf_stream_write(stream_tx, buffer.as_ptr() as *mut u8, buffer.len() as u32);
            if n == 0 {
                continue;
            } else {
                return Ok(n as usize);
            }
        }}
    }

    fn blocking_flush(&self) -> Result<(), Error> {
        let s = self.buffered_state;
        let usart_stream = s.usart_stream.load(Ordering::Relaxed);
        let stream_tx = unsafe { &((*usart_stream).stream_tx) as *const vsf_fifo_stream_t as *mut vsf_fifo_stream_t as *mut vsf_stream_t };

        loop { unsafe {
            if 0 == vsf_stream_get_data_size(stream_tx) {
                return Ok(());
            }
        }}
    }
}

struct Info {
    vsf_usart: AtomicPtr<vsf_usart_t>,
    vsf_usart_irqhandler: unsafe extern "C" fn(),
    interrupt: Interrupt,
}

struct State {
    rx_waker: AtomicWaker,
    tx_waker: AtomicWaker,
    rx_irq_mask: AtomicU32,
    tx_irq_mask: AtomicU32,
    rx_exit_on_idle: AtomicBool,
    config: AtomicPtr<vsf_usart_cfg_t>,
    _config: vsf_usart_cfg_t,
}

unsafe impl Sync for State {}

impl vsf_usart_cfg_t {
    const fn new() -> Self {
        Self {
            mode: 0,
            baudrate: 0,
            rx_timeout: 0,
            #[cfg(VSF_USART_IRQ_MASK_RX_IDLE)]
            rx_idle_cnt: 0,
            isr: vsf_usart_isr_t {
                handler_fn: Some(vsf_usart_on_interrupt),
                target_ptr: 0 as *mut ::core::ffi::c_void,
                prio: into_vsf_arch_prio_t!(VSF_ARCH_PRIO_INVALID),
            },
        }
    }
}

impl State {
    const fn new() -> Self {
        Self {
            rx_waker: AtomicWaker::new(),
            tx_waker: AtomicWaker::new(),
            rx_irq_mask: AtomicU32::new(0),
            tx_irq_mask: AtomicU32::new(0),
            rx_exit_on_idle: AtomicBool::new(false),
            config: AtomicPtr::new(ptr::null_mut()),
            _config: vsf_usart_cfg_t::new(),
        }
    }
}

struct UsartStream {
    usart_stream: vsf_usart_stream_t,
    stream_rx: vsf_fifo_stream_t,
    stream_tx: vsf_fifo_stream_t,
}

impl UsartStream {
    const fn new() -> Self {
        Self {
            usart_stream: vsf_usart_stream_t {
                usart: 0 as *mut vsf_usart_t,
                stream_rx: 0 as *mut vsf_stream_t,
                stream_tx: 0 as *mut vsf_stream_t,
                target: 0 as *mut ::core::ffi::c_void,
                on_error: Some(vsf_usart_stream_on_error),
                tx: vsf_usart_stream_t__bindgen_ty_1 {
                    size: 0,
                },
            },
            stream_rx: vsf_fifo_stream_t {
                __bindgen_anon_1: vsf_fifo_stream_t__bindgen_ty_1 {
                    use_as__vsf_stream_t: vsf_stream_t {
                        op: unsafe { &vsf_fifo_stream_op as *const vsf_stream_op_t },
                        is_ticktock_read: false,
                        is_ticktock_write: false,
                        __bindgen_anon_1: vsf_stream_t__bindgen_ty_1 {
                            __bindgen_anon_1: vsf_stream_t__bindgen_ty_1__bindgen_ty_1 {
                                tx: vsf_stream_terminal_t {
                                    param: 0 as *mut ::core::ffi::c_void,
                                    evthandler: None,
                                    threshold: 0,
                                    ready: false,
                                    data_notified: false,
                                },
                                rx: vsf_stream_terminal_t {
                                    param: 0 as *mut ::core::ffi::c_void,
                                    evthandler: None,
                                    threshold: 0,
                                    ready: false,
                                    data_notified: false,
                                },
                            },
                        },
                    },
                },
                __bindgen_anon_2: vsf_fifo_stream_t__bindgen_ty_2 {
                    use_as__vsf_byte_fifo_t: vsf_byte_fifo_t {
                        buffer: 0 as *mut u8,
                        size: 0,
                        head: 0,
                        tail: 0,
                    },
                },
            },
            stream_tx: vsf_fifo_stream_t {
                __bindgen_anon_1: vsf_fifo_stream_t__bindgen_ty_1 {
                    use_as__vsf_stream_t: vsf_stream_t {
                        op: unsafe{ &vsf_fifo_stream_op as *const vsf_stream_op_t },
                        is_ticktock_read: false,
                        is_ticktock_write: false,
                        __bindgen_anon_1: vsf_stream_t__bindgen_ty_1 {
                            __bindgen_anon_1: vsf_stream_t__bindgen_ty_1__bindgen_ty_1 {
                                tx: vsf_stream_terminal_t {
                                    param: 0 as *mut ::core::ffi::c_void,
                                    evthandler: None,
                                    threshold: 0,
                                    ready: false,
                                    data_notified: false,
                                },
                                rx: vsf_stream_terminal_t {
                                    param: 0 as *mut ::core::ffi::c_void,
                                    evthandler: None,
                                    threshold: 0,
                                    ready: false,
                                    data_notified: false,
                                },
                            },
                        },
                    },
                },
                __bindgen_anon_2: vsf_fifo_stream_t__bindgen_ty_2 {
                    use_as__vsf_byte_fifo_t: vsf_byte_fifo_t {
                        buffer: 0 as *mut u8,
                        size: 0,
                        head: 0,
                        tail: 0,
                    },
                },
            },
        }
    }
}

struct BufferedState {
    rx_waker: AtomicWaker,
    tx_waker: AtomicWaker,
    rx_irq_mask: AtomicU32,
    usart_stream: AtomicPtr<UsartStream>,
    _usart_stream_instance: UsartStream,
}

unsafe impl Sync for BufferedState {}
impl BufferedState {
    const fn new() -> Self {
        Self {
            rx_waker: AtomicWaker::new(),
            tx_waker: AtomicWaker::new(),
            rx_irq_mask: AtomicU32::new(0),
            usart_stream: AtomicPtr::new(ptr::null_mut()),
            _usart_stream_instance: UsartStream::new(),
        }
    }
}

trait SealedInstance {
    fn info() -> &'static Info;
    fn state() -> &'static State;
    fn buffered_state() -> &'static BufferedState;
}

/// USART peripheral instance.
#[allow(private_bounds)]
pub trait Instance: SealedInstance + PeripheralType + 'static + Send {
    /// Interrupt for this peripheral.
    type Interrupt: interrupt::typelevel::Interrupt;
}

pin_trait!(RxPin, Instance);
pin_trait!(TxPin, Instance);
pin_trait!(CtsPin, Instance);
pin_trait!(RtsPin, Instance);
pin_trait!(CkPin, Instance);
pin_trait!(DePin, Instance);

macro_rules! impl_usart {
    ($type:ident, $irq:ident, $vsf_usart:ident, $vsf_irqhandler:ident) => {
        impl crate::usart::SealedInstance for crate::peripherals::$type {
            fn info() -> &'static crate::usart::Info {
                static INFO: crate::usart::Info = crate::usart::Info {
                    vsf_usart: AtomicPtr::new(ptr::addr_of_mut!($vsf_usart) as *mut vsf_hw_usart_t as *mut vsf_usart_t),
                    vsf_usart_irqhandler: $vsf_irqhandler,
                    interrupt: crate::interrupt::typelevel::$irq::IRQ,
                };
                &INFO
            }
            fn state() -> &'static crate::usart::State {
                static STATE: crate::usart::State = crate::usart::State::new();
                &STATE
            }
            fn buffered_state() -> &'static crate::usart::BufferedState {
                static STATE: crate::usart::BufferedState = crate::usart::BufferedState::new();
                &STATE
            }
        }
        impl crate::usart::Instance for peripherals::$type {
            type Interrupt = crate::interrupt::typelevel::$irq;
        }
    };
}

use crate::peripherals;
vsf_hal_macros::bind_vsf_usarts!{}

// ====================

mod eh02 {
    use super::*;

    impl embedded_hal_02::blocking::serial::Write<u8> for Usart<Blocking> {
        type Error = Error;

        fn bwrite_all(&mut self, buffer: &[u8]) -> Result<(), Self::Error> {
            self.blocking_write(buffer)
        }

        fn bflush(&mut self) -> Result<(), Self::Error> {
            Ok(())
        }
    }

    impl embedded_hal_02::blocking::serial::Write<u8> for UsartTx<Blocking> {
        type Error = Error;

        fn bwrite_all(&mut self, buffer: &[u8]) -> Result<(), Self::Error> {
            self.blocking_write(buffer)
        }

        fn bflush(&mut self) -> Result<(), Self::Error> {
            Ok(())
        }
    }
}

mod _embedded_io {
    use super::*;

    impl embedded_io_async::Error for Error {
        fn kind(&self) -> embedded_io_async::ErrorKind {
            match *self {
                #[cfg(VSF_USART_IRQ_MASK_FRAME_ERR)]
                Error::Framing => embedded_io_async::ErrorKind::InvalidData,
                #[cfg(VSF_USART_IRQ_MASK_PARITY_ERR)]
                Error::Parity => embedded_io_async::ErrorKind::InvalidData,
                #[cfg(VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR)]
                Error::RxOverrun => embedded_io_async::ErrorKind::OutOfMemory,
                #[cfg(VSF_USART_IRQ_MASK_BREAK_ERR)]
                Error::Break => embedded_io_async::ErrorKind::ConnectionAborted,
            }
        }
    }

    impl embedded_io_async::ErrorType for Usart<Async> {
        type Error = Error;
    }

    impl embedded_io_async::ErrorType for UsartTx<Async> {
        type Error = Error;
    }

    impl embedded_io_async::Write for Usart<Async> {
        async fn write(&mut self, buf: &[u8]) -> Result<usize, Self::Error> {
            self.write(buf).await?;
            Ok(buf.len())
        }
    }

    impl embedded_io_async::Write for UsartTx<Async> {
        async fn write(&mut self, buf: &[u8]) -> Result<usize, Self::Error> {
            self.write(buf).await?;
            Ok(buf.len())
        }
    }
}