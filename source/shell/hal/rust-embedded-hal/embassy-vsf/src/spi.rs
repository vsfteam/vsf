
use core::future::poll_fn;
use core::marker::PhantomData;
use core::sync::atomic::{Ordering, AtomicPtr, AtomicU32};
use core::task::Poll;
use core::ptr;
use paste::paste;

use embassy_hal_internal::{Peri, PeripheralType};
pub use embedded_hal_02::spi::{Mode, Phase, Polarity, MODE_0, MODE_1, MODE_2, MODE_3};
use embassy_sync::waitqueue::AtomicWaker;

use crate::interrupt::typelevel::Interrupt as _;
use crate::interrupt::{self, Interrupt, InterruptExt};

use crate::gpio::{AfType, AnyPin, Pull, Speed, OutputDrive};
use crate::mode::{Async, Blocking, Mode as PeriMode};
use super::Hertz;

use crate::vsf_hal::{*};

#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_spi_mode_t {($mode:ident) => { vsf_spi_mode_t::$mode }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_spi_irq_mask_t {($irq:ident) => { vsf_spi_irq_mask_t::$irq }}

#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_spi_mode_t {($mode:ident) => { paste!{[<vsf_spi_mode_t_ $mode>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_spi_irq_mask_t {($irq:ident) => { paste!{[<vsf_spi_irq_mask_t_ $irq>]} }}

/// SPI error
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Error {
    /// RxOverrun.
    RxOverrun,
}

impl core::fmt::Display for Error {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        let message = match self {
            Self::RxOverrun => "Rx Buffer Overrun",
        };

        write!(f, "{}", message)
    }
}

impl core::error::Error for Error {}

/// SPI bit order
#[derive(Copy, Clone)]
pub enum BitOrder {
    /// Least significant bit first.
    LsbFirst = into_vsf_spi_mode_t!(VSF_SPI_LSB_FIRST) as isize,
    /// Most significant bit first.
    MsbFirst = into_vsf_spi_mode_t!(VSF_SPI_MSB_FIRST) as isize,
}

/// SPI configuration.
#[non_exhaustive]
#[derive(Copy, Clone)]
pub struct Config {
    /// SPI mode.
    pub mode: Mode,
    /// Bit order.
    pub bit_order: BitOrder,
    /// Clock frequency.
    pub frequency: Hertz,
    /// Enable internal pullup on MISO.
    ///
    /// There are some ICs that require a pull-up on the MISO pin for some applications.
    /// If you  are unsure, you probably don't need this.
    pub miso_pull: Pull,
    /// signal rise/fall speed (slew rate) - defaults to `Medium`.
    /// Increase for high SPI speeds. Change to `Low` to reduce ringing.
    pub gpio_speed: Speed,
}

#[non_exhaustive]
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Config Error
pub enum ConfigError {
    /// frequency too low
    FrequencyTooLow,
    /// Frequency too high
    FrequencyTooHigh,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            mode: MODE_0,
            bit_order: BitOrder::MsbFirst,
            frequency: Hertz(1_000_000),
            miso_pull: Pull::None,
            gpio_speed: Speed::highest(),
        }
    }
}

impl Config {
    fn raw_phase(&self) -> u32 {
        match self.mode.phase {
            Phase::CaptureOnSecondTransition => into_vsf_spi_mode_t!(VSF_SPI_CPHA_HIGH) as u32,
            Phase::CaptureOnFirstTransition => into_vsf_spi_mode_t!(VSF_SPI_CPHA_LOW) as u32,
        }
    }

    fn raw_polarity(&self) -> u32 {
        match self.mode.polarity {
            Polarity::IdleHigh => into_vsf_spi_mode_t!(VSF_SPI_CPOL_HIGH) as u32,
            Polarity::IdleLow => into_vsf_spi_mode_t!(VSF_SPI_CPOL_LOW) as u32,
        }
    }

    fn sck_af(&self) -> AfType {
        AfType::output(self.gpio_speed, OutputDrive::highest())
    }
}

/// Configure vsf_spi.
/// 
/// config_fix is set by caller with Tx/Rx related configurations
fn _vsf_spi_config(info: &Info, state: &State, config: &Config, config_fix: u32, target_ptr: *mut ::core::ffi::c_void, irqhandler: vsf_spi_isr_handler_t) -> Result<(), ConfigError> {
    unsafe {
        let vsf_spi = info.vsf_spi.load(Ordering::Relaxed);
        let cap = vsf_spi_capability(vsf_spi);
        if (cap.min_clock_hz != 0) && (config.frequency < Hertz::hz(cap.min_clock_hz)) {
            return Err(ConfigError::FrequencyTooLow);
        }
        if (cap.max_clock_hz != 0) && (config.frequency > Hertz::hz(cap.max_clock_hz)) {
            return Err(ConfigError::FrequencyTooHigh);
        }

        let spi_config = &state._config as *const vsf_spi_cfg_t as *mut vsf_spi_cfg_t;
        state.config.store(&state._config as *const vsf_spi_cfg_t as *mut vsf_spi_cfg_t, Ordering::Relaxed);

        (*spi_config).mode = config.bit_order as u32 | config_fix | config.raw_phase() | config.raw_polarity();
        (*spi_config).clock_hz = config.frequency.0;
        (*spi_config).auto_cs_index = 0;
        (*spi_config).isr.handler_fn = irqhandler;
        (*spi_config).isr.target_ptr = target_ptr;

        vsf_spi_init(vsf_spi, spi_config);
    }
    Ok(())
}

fn _vsf_spi_config_and_enable(info: &Info, state: &State, config: &Config, config_fix: u32, target_ptr: *mut ::core::ffi::c_void, irqhandler: vsf_spi_isr_handler_t) -> Result<(), ConfigError> {
    _vsf_spi_config(info, state, config, config_fix, target_ptr, irqhandler)?;
    unsafe {
        let vsf_spi = info.vsf_spi.load(Ordering::Relaxed);
        while vsf_spi_enable(vsf_spi) != into_fsm_rt_t!(fsm_rt_cpl) {}

        info.interrupt.unpend();
        info.interrupt.enable();
    }
    Ok(())
}

fn _vsf_spi_drop(info: &Info) {
    unsafe {
        let vsf_spi = info.vsf_spi.load(Ordering::Relaxed);
        while vsf_spi_disable(vsf_spi) != into_fsm_rt_t!(fsm_rt_cpl) {}
        vsf_spi_fini(vsf_spi);
    }
}

unsafe extern "C" fn vsf_spi_on_interrupt(
    target_ptr: *mut ::core::ffi::c_void,
    vsf_spi: *mut vsf_spi_t,
    irq_mask: into_enum_type!(vsf_spi_irq_mask_t),
) {
    let s = target_ptr as *const State;
    let s = unsafe { &*s };

    let mut transfer_irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) | into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL);
    if transfer_irq_mask & irq_mask != 0 {
        unsafe {
            vsf_spi_irq_disable(vsf_spi, transfer_irq_mask);
        }
        s.irq_mask.fetch_or(transfer_irq_mask, Ordering::Relaxed);
        s.waker.wake();
    }
}

/// SPI driver.
pub struct Spi<M: PeriMode> {
    pub(crate) info: &'static Info,
    pub(crate) state: &'static State,
    _phantom: PhantomData<M>,
}

impl<'d, M: PeriMode> Spi<M> {
    fn new_inner<T: Instance>(
        _peri: Peri<'d, T>,
        _sck: Option<Peri<'d, AnyPin>>,
        _mosi: Option<Peri<'d, AnyPin>>,
        _miso: Option<Peri<'d, AnyPin>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        let info = T::info();
        let s = T::state();
        _vsf_spi_config_and_enable(info, s, &config, 0, ptr::from_ref(s) as *mut ::core::ffi::c_void, Some(vsf_spi_on_interrupt))?;

        Ok(Self {
            info: info,
            state: s,
            _phantom: PhantomData,
        })
    }

    /// Blocking write
    pub fn blocking_write(&mut self, data: &[u8]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let tx_offset: uint_fast32_t = 0 as uint_fast32_t;

            while tx_offset < data.len() as uint_fast32_t {
                vsf_spi_fifo_transfer(vsf_spi,
                    data.as_ptr() as *mut ::core::ffi::c_void, &tx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    0 as *mut ::core::ffi::c_void, 0 as *mut uint_fast32_t,
                    data.len() as uint_fast32_t
                );
            }
        }
        Ok(())
    }

    /// Blocking read
    pub fn blocking_read(&mut self, data: &[u8]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let rx_offset: uint_fast32_t = 0 as uint_fast32_t;

            while rx_offset < data.len() as uint_fast32_t {
                vsf_spi_fifo_transfer(vsf_spi,
                    0 as *mut ::core::ffi::c_void, 0 as *mut uint_fast32_t,
                    data.as_ptr() as *mut ::core::ffi::c_void, &rx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    data.len() as uint_fast32_t
                );
            }
        }
        Ok(())
    }

    /// Simultaneously sends and receives data. Blocks until the transmission is completed.
    /// If necessary, the write buffer will be copied into RAM (see struct description for detail).
    pub fn blocking_transfer(&mut self, read: &mut [u8], write: &[u8]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let xfer_len = core::cmp::max(read.len(), write.len());
            let xfer_common_len = core::cmp::min(read.len(), write.len());
            let mut tx_ptr = write.as_ptr() as *mut ::core::ffi::c_void;
            let mut rx_ptr = read.as_ptr() as *mut ::core::ffi::c_void;
            let tx_offset: uint_fast32_t = 0 as uint_fast32_t;
            let rx_offset: uint_fast32_t = 0 as uint_fast32_t;
            let mut tx_offset_ptr = &tx_offset as *const uint_fast32_t as *mut uint_fast32_t;
            let mut rx_offset_ptr = &rx_offset as *const uint_fast32_t as *mut uint_fast32_t;
            let mut xfered_len = 0;

            while xfered_len < xfer_len {
                let cur_len = if xfered_len < xfer_common_len {
                    xfer_common_len
                } else {
                    if xfered_len >= read.len() {
                        rx_offset_ptr = 0 as *mut uint_fast32_t;
                        rx_ptr = 0 as *mut ::core::ffi::c_void;
                    } else if xfered_len >= write.len() {
                        tx_offset_ptr = 0 as *mut uint_fast32_t;
                        tx_ptr = 0 as *mut ::core::ffi::c_void;
                    }
                    xfer_len - xfered_len
                };

                vsf_spi_fifo_transfer(vsf_spi,
                    tx_ptr, tx_offset_ptr,
                    rx_ptr, rx_offset_ptr,
                    cur_len as uint_fast32_t
                );
                if tx_offset_ptr != 0 as *mut uint_fast32_t {
                    xfered_len = *tx_offset_ptr as usize;
                } else {
                    xfered_len = *rx_offset_ptr  as usize;
                }
            }
        }
        Ok(())
    }

    /// Simultaneously sends and receives data.
    /// Places the received data into the same buffer and blocks until the transmission is completed.
    pub fn blocking_transfer_in_place(&mut self, data: &mut [u8]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let data_ptr = data.as_ptr() as *mut ::core::ffi::c_void;
            let tx_offset: uint_fast32_t = 0 as uint_fast32_t;
            let rx_offset: uint_fast32_t = 0 as uint_fast32_t;

            while rx_offset < data.len() as uint_fast32_t {
                vsf_spi_fifo_transfer(vsf_spi,
                    data_ptr, &tx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    data_ptr, &rx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    data.len() as uint_fast32_t
                );
            }
        }
        Ok(())
    }
}

impl<M: PeriMode> Drop for Spi<M> {
    fn drop(&mut self) {
        _vsf_spi_drop(self.info)
    }
}

impl<'d> Spi<Blocking> {
    /// Create a new blocking SPI driver.
    pub fn new_blocking<T: Instance>(
        peri: Peri<'d, T>,
        sck: Peri<'d, impl SckPin<T>>,
        mosi: Peri<'d, impl MosiPin<T>>,
        miso: Peri<'d, impl MisoPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner(
            peri,
            new_pin!(sck, config.sck_af()),
            new_pin!(mosi, AfType::output(config.gpio_speed, OutputDrive::highest())),
            new_pin!(miso, AfType::input(config.miso_pull)),
            config,
        )
    }

    /// Create a new blocking SPI driver, in RX-only mode (only MISO pin, no MOSI).
    pub fn new_blocking_rxonly<T: Instance>(
        peri: Peri<'d, T>,
        sck: Peri<'d, impl SckPin<T>>,
        miso: Peri<'d, impl MisoPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner(
            peri,
            new_pin!(sck, config.sck_af()),
            None,
            new_pin!(miso, AfType::input(config.miso_pull)),
            config,
        )
    }

    /// Create a new blocking SPI driver, in TX-only mode (only MOSI pin, no MISO).
    pub fn new_blocking_txonly<T: Instance>(
        peri: Peri<'d, T>,
        sck: Peri<'d, impl SckPin<T>>,
        mosi: Peri<'d, impl MosiPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner(
            peri,
            new_pin!(sck, config.sck_af()),
            new_pin!(mosi, AfType::output(config.gpio_speed, OutputDrive::highest())),
            None,
            config,
        )
    }
}

impl<'d> Spi<Async> {
    /// Create a new SPI driver.
    pub fn new<T: Instance>(
        peri: Peri<'d, T>,
        sck: Peri<'d, impl SckPin<T>>,
        mosi: Peri<'d, impl MosiPin<T>>,
        miso: Peri<'d, impl MisoPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner(
            peri,
            new_pin!(sck, config.sck_af()),
            new_pin!(mosi, AfType::output(config.gpio_speed, OutputDrive::highest())),
            new_pin!(miso, AfType::input(config.miso_pull)),
            config,
        )
    }

    /// Create a new SPI driver, in RX-only mode (only MISO pin, no MOSI).
    pub fn new_rxonly<T: Instance>(
        peri: Peri<'d, T>,
        sck: Peri<'d, impl SckPin<T>>,
        miso: Peri<'d, impl MisoPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner(
            peri,
            new_pin!(sck, config.sck_af()),
            None,
            new_pin!(miso, AfType::input(config.miso_pull)),
            config,
        )
    }

    /// Create a new SPI driver, in TX-only mode (only MOSI pin, no MISO).
    pub fn new_txonly<T: Instance>(
        peri: Peri<'d, T>,
        sck: Peri<'d, impl SckPin<T>>,
        mosi: Peri<'d, impl MosiPin<T>>,
        config: Config,
    ) -> Result<Self, ConfigError> {
        Self::new_inner(
            peri,
            new_pin!(sck, config.sck_af()),
            new_pin!(mosi, AfType::output(config.gpio_speed, OutputDrive::highest())),
            None,
            config,
        )
    }

    /// SPI write, using DMA.
    pub async fn write(&mut self, data: &[u8]) -> Result<(), Error> {
        if data.is_empty() {
            return Ok(());
        }

        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let ptr = data.as_ptr();
            let len = data.len();

            vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL));
            vsf_spi_request_transfer(vsf_spi,
                ptr as *mut ::core::ffi::c_void,
                0 as *mut ::core::ffi::c_void,
                len as uint_fast32_t);
        }

        let s = self.state;
        poll_fn(|cx| {
            s.waker.register(cx.waker());
            if s.irq_mask.swap(0, Ordering::Relaxed) & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) != 0 {
                return Poll::Ready(());
            }
            Poll::Pending
        }).await;

        Ok(())
    }

    pub async fn read(&mut self, data: &mut [u8]) -> Result<(), Error> {
        if data.is_empty() {
            return Ok(());
        }

        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let ptr = data.as_ptr();
            let len = data.len();

            vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL));
            vsf_spi_request_transfer(vsf_spi,
                ptr as *mut ::core::ffi::c_void,
                0 as *mut ::core::ffi::c_void,
                len as uint_fast32_t);
        }

        let s = self.state;
        poll_fn(|cx| {
            s.waker.register(cx.waker());
            if s.irq_mask.swap(0, Ordering::Relaxed) & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL) != 0 {
                return Poll::Ready(());
            }
            Poll::Pending
        }).await;

        Ok(())
    }

    /// Bidirectional transfer, using DMA.
    ///
    /// This transfers both buffers at the same time, so it is NOT equivalent to `write` followed by `read`.
    ///
    /// The transfer runs for `max(read.len(), write.len())` bytes. If `read` is shorter extra bytes are ignored.
    /// If `write` is shorter it is padded with zero bytes.
    pub async fn transfer(&mut self, read: &mut [u8], write: &[u8]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let xfer_len = core::cmp::max(read.len(), write.len());
            let xfer_common_len = core::cmp::min(read.len(), write.len());
            let mut tx_ptr = write.as_ptr() as *mut ::core::ffi::c_void;
            let mut rx_ptr = read.as_ptr() as *mut ::core::ffi::c_void;
            let mut xfered_len = 0;
            let mut irq_mask: u32 = 0;

            while xfered_len < xfer_len {
                let cur_len = if xfered_len < xfer_common_len {
                    irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL);
                    xfer_common_len
                } else {
                    if xfered_len >= read.len() {
                        irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL);
                        rx_ptr = 0 as *mut ::core::ffi::c_void;
                    } else if xfered_len >= write.len() {
                        irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL);
                        tx_ptr = 0 as *mut ::core::ffi::c_void;
                    }
                    xfer_len - xfered_len
                };

                vsf_spi_irq_enable(vsf_spi, irq_mask);
                vsf_spi_request_transfer(vsf_spi,
                    tx_ptr, rx_ptr,
                    cur_len as uint_fast32_t
                );

                let s = self.state;
                poll_fn(|cx| {
                    s.waker.register(cx.waker());
                    if s.irq_mask.swap(0, Ordering::Relaxed) & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL) != 0 {
                        return Poll::Ready(());
                    }
                    Poll::Pending
                }).await;

                xfered_len += cur_len as usize;
            }
        }

        Ok(())
    }

    /// In-place bidirectional transfer, using DMA.
    ///
    /// This writes the contents of `data` on MOSI, and puts the received data on MISO in `data`, at the same time.
    pub async fn transfer_in_place(&mut self, data: &mut [u8]) -> Result<(), Error> {
        if data.is_empty() {
            return Ok(());
        }

        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let ptr = data.as_ptr();
            let len = data.len();

            vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL));
            vsf_spi_request_transfer(vsf_spi,
                ptr as *mut ::core::ffi::c_void,
                ptr as *mut ::core::ffi::c_void,
                len as uint_fast32_t);
        }

        let s = self.state;
        poll_fn(|cx| {
            s.waker.register(cx.waker());
            if s.irq_mask.swap(0, Ordering::Relaxed) & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CPL) != 0 {
                return Poll::Ready(());
            }
            Poll::Pending
        }).await;

        Ok(())
    }
}






struct State {
    waker: AtomicWaker,
    irq_mask: AtomicU32,
    config: AtomicPtr<vsf_spi_cfg_t>,
    _config: vsf_spi_cfg_t,
}

impl vsf_spi_cfg_t {
    const fn new() -> Self {
        Self {
            mode: 0,
            clock_hz: 0,
            auto_cs_index: 0,
            isr: vsf_spi_isr_t {
                handler_fn: Some(vsf_spi_on_interrupt),
                target_ptr: 0 as *mut ::core::ffi::c_void,
                prio: into_vsf_arch_prio_t!(VSF_ARCH_PRIO_INVALID),
            },
        }
    }
}

impl State {
    const fn new() -> Self {
        Self {
            waker: AtomicWaker::new(),
            irq_mask: AtomicU32::new(0),
            config: AtomicPtr::new(ptr::null_mut()),
            _config: vsf_spi_cfg_t::new(),
        }
    }
}

unsafe impl Sync for State {}

peri_trait!(
    irqs: [Interrupt],
);

/// interrupt handler
pub struct InterruptHandler<T: Instance> {
    _phantom: PhantomData<T>,
}

impl<T: Instance> interrupt::typelevel::Handler<T::Interrupt> for InterruptHandler<T> {
    unsafe fn on_interrupt() {
        unsafe {
            (T::info().vsf_spi_irqhandler)();
        }
    }
}

pin_trait!(SckPin, Instance);
pin_trait!(MisoPin, Instance);
pin_trait!(MosiPin, Instance);
pin_trait!(CsPin, Instance);

macro_rules! impl_spi_irq_type {
    ($name_lower:ident, $name_upper:ident, $peripheral_index:literal) => { paste!{
        type Interrupt = crate::interrupt::typelevel::[<$name_upper $peripheral_index>];
    }}
}

vsf_hal_macros::bind_vsf_peripheral!{spi impl_spi_irq_type}
