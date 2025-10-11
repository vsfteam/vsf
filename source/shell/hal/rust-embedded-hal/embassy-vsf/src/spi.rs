
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
use crate::word;
use super::Hertz;

use crate::vsf_hal::{*};

#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_spi_mode_t {($mode:ident) => { vsf_spi_mode_t::$mode }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_spi_irq_mask_t {($irq:ident) => { vsf_spi_irq_mask_t::$irq }}
#[cfg(bindgen_enum_type_moduleconsts)]
macro_rules! into_vsf_spi_ctrl_t {($ctrl:ident) => { vsf_spi_ctrl_t::$ctrl }}

#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_spi_mode_t {($mode:ident) => { paste!{[<vsf_spi_mode_t_ $mode>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_spi_irq_mask_t {($irq:ident) => { paste!{[<vsf_spi_irq_mask_t_ $irq>]} }}
#[cfg(bindgen_enum_type_consts)]
macro_rules! into_vsf_spi_ctrl_t {($ctrl:ident) => { paste!{[<vsf_spi_ctrl_t_ $ctrl>]} }}

/// SPI error
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
#[cfg_attr(feature = "defmt", derive(defmt::Format))]
pub enum Error {
    /// CRC error (only if hardware CRC checking is enabled).
    #[cfg(VSF_SPI_IRQ_MASK_CRC_ERR)]
    Crc = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CRC_ERR) as isize,
    /// RxOverrun.
    RxOverrun = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR) as isize,
}

impl core::fmt::Display for Error {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        let message = match self {
            Self::RxOverrun => "Rx Buffer Overrun",
            #[cfg(VSF_SPI_IRQ_MASK_CRC_ERR)]
            Self::Crc => "Hardware CRC Check Failed",
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
    pub is_master: bool,
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
            is_master: true,
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

        (*spi_config).mode = config.bit_order as u32 | config_fix | config.raw_phase() | config.raw_polarity() | {
            if config.is_master {
                into_vsf_spi_mode_t!(VSF_SPI_MASTER) as u32
            } else {
                into_vsf_spi_mode_t!(VSF_SPI_SLAVE) as u32
            }
        };
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

fn _vsf_spi_check_error(irq_mask: into_enum_type!(vsf_spi_irq_mask_t)) -> Result<(), Error> {
    #[cfg(VSF_SPI_IRQ_MASK_CRC_ERR)]
    if irq_mask & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_CRC_ERR) as into_enum_type!(vsf_spi_irq_mask_t) != 0 {
        return Err(Error::Crc);
    }
    if irq_mask & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_OVERFLOW_ERR) as into_enum_type!(vsf_spi_irq_mask_t) != 0 {
        return Err(Error::RxOverrun);
    }

    Ok(())
}

unsafe extern "C" fn vsf_spi_on_interrupt(
    target_ptr: *mut ::core::ffi::c_void,
    vsf_spi: *mut vsf_spi_t,
    irq_mask: into_enum_type!(vsf_spi_irq_mask_t),
) {
    let s = target_ptr as *const State;
    let s = unsafe { &*s };

    let mut transfer_irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) as u32 | into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as u32;
    if transfer_irq_mask & irq_mask as u32 != 0 {
        unsafe {
            vsf_spi_irq_disable(vsf_spi, transfer_irq_mask as into_enum_type!(vsf_spi_irq_mask_t));
        }
        s.irq_mask.fetch_or(transfer_irq_mask, Ordering::Relaxed);
        s.waker.wake();
    }
}

/// SPI driver.
pub struct Spi<M: PeriMode> {
    pub(crate) info: &'static Info,
    pub(crate) state: &'static State,
    current_word_size: word_impl::Config,
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
            current_word_size: <u8 as SealedWord>::CONFIG,
            _phantom: PhantomData,
        })
    }

    pub(crate) fn set_word_size(&mut self, word_size: word_impl::Config) {
        if self.current_word_size == word_size {
            return;
        }

        #[cfg(VSF_SPI_CTRL_SET_DATASIZE)]
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let mut config = word_size as u32;
            vsf_spi_ctrl(vsf_spi,
                into_vsf_spi_ctrl_t!(VSF_SPI_CTRL_SET_DATASIZE), 
                &config as *const ::core::ffi::c_void as *mut ::core::ffi::c_void
            );
            self.current_word_size = word_size;
        }
        #[cfg(not(VSF_SPI_CTRL_SET_DATASIZE))]
        unreachable!("dynamic word_size not supported");
    }

    /// Blocking write
    pub fn blocking_write<W: Word>(&mut self, data: &[W]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let tx_offset: uint_fast32_t = 0 as uint_fast32_t;

            self.set_word_size(W::CONFIG);
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
    pub fn blocking_read<W: Word>(&mut self, data: &[W]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let rx_offset: uint_fast32_t = 0 as uint_fast32_t;

            self.set_word_size(W::CONFIG);
            while rx_offset < data.len() as uint_fast32_t {
                vsf_spi_fifo_transfer(vsf_spi,
                    0 as *mut ::core::ffi::c_void, 0 as *mut uint_fast32_t,
                    data.as_ptr() as *mut ::core::ffi::c_void, &rx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    data.len() as uint_fast32_t
                );
                if let Err(error) = _vsf_spi_check_error(vsf_spi_irq_clear(vsf_spi, VSF_SPI_IRQ_MASK_ERR as into_enum_type!(vsf_spi_irq_mask_t))) {
                    return Err(error);
                }
            }
        }
        Ok(())
    }

    /// Simultaneously sends and receives data. Blocks until the transmission is completed.
    /// If necessary, the write buffer will be copied into RAM (see struct description for detail).
    pub fn blocking_transfer<W: Word>(&mut self, read: &mut [W], write: &[W]) -> Result<(), Error> {
        unsafe {
            let mut read_len = read.len();
            let mut write_len = write.len();
            let mut read_ptr = read.as_ptr() as *mut ::core::ffi::c_void;
            let mut write_ptr = write.as_ptr() as *mut ::core::ffi::c_void;

            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let xfer_len = core::cmp::max(read_len, write_len);
            let xfer_common_len = core::cmp::min(read_len, write_len);
            let mut irq_mask: u32 = 0;
            let tx_offset: uint_fast32_t = 0 as uint_fast32_t;
            let rx_offset: uint_fast32_t = 0 as uint_fast32_t;
            let mut tx_offset_ptr = &tx_offset as *const uint_fast32_t as *mut uint_fast32_t;
            let mut rx_offset_ptr = &rx_offset as *const uint_fast32_t as *mut uint_fast32_t;
            let mut xfered_len = 0;
            let mut err_mask = 0 as u32;

            self.set_word_size(W::CONFIG);
            while xfered_len < xfer_len {
                let cur_len = if xfered_len < xfer_common_len {
                    err_mask = VSF_SPI_IRQ_MASK_ERR as u32;
                    xfer_common_len
                } else {
                    if xfered_len >= read_len {
                        err_mask = 0;
                        rx_offset_ptr = 0 as *mut uint_fast32_t;
                        read_ptr = 0 as *mut ::core::ffi::c_void;
                    } else if xfered_len >= write_len {
                        err_mask = VSF_SPI_IRQ_MASK_ERR as u32;
                        tx_offset_ptr = 0 as *mut uint_fast32_t;
                        write_ptr = 0 as *mut ::core::ffi::c_void;
                    }
                    xfer_len - xfered_len
                };

                vsf_spi_fifo_transfer(vsf_spi,
                    write_ptr, tx_offset_ptr,
                    read_ptr, rx_offset_ptr,
                    cur_len as uint_fast32_t
                );
                if let Err(error) = _vsf_spi_check_error(vsf_spi_irq_clear(vsf_spi, err_mask as into_enum_type!(vsf_spi_irq_mask_t))) {
                    return Err(error);
                }
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
    pub fn blocking_transfer_in_place<W: Word>(&mut self, data: &mut [W]) -> Result<(), Error> {
        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let data_ptr = data.as_ptr() as *mut ::core::ffi::c_void;
            let tx_offset: uint_fast32_t = 0 as uint_fast32_t;
            let rx_offset: uint_fast32_t = 0 as uint_fast32_t;

            self.set_word_size(W::CONFIG);
            while rx_offset < data.len() as uint_fast32_t {
                vsf_spi_fifo_transfer(vsf_spi,
                    data_ptr, &tx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    data_ptr, &rx_offset as *const uint_fast32_t as *mut uint_fast32_t,
                    data.len() as uint_fast32_t
                );
                if let Err(error) = _vsf_spi_check_error(vsf_spi_irq_clear(vsf_spi, VSF_SPI_IRQ_MASK_ERR as into_enum_type!(vsf_spi_irq_mask_t))) {
                    return Err(error);
                }
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

    /// SPI write.
    pub async fn write<W: Word>(&mut self, data: &[W]) -> Result<(), Error> {
        if data.is_empty() {
            return Ok(());
        }

        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let ptr = data.as_ptr();
            let len = data.len();

            self.set_word_size(W::CONFIG);
            vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
            vsf_spi_request_transfer(vsf_spi,
                ptr as *mut ::core::ffi::c_void,
                0 as *mut ::core::ffi::c_void,
                len as uint_fast32_t
            );

            let s = self.state;
            poll_fn(|cx| -> Poll<Result<(), Error>> {
                if s.irq_mask.swap(0, Ordering::Relaxed) & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) as u32 != 0 {
                    vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
                    Poll::Ready(Ok(()))
                } else {
                    s.waker.register(cx.waker());
                    Poll::Pending
                }
            }).await
        }
    }

    pub async fn read<W: Word>(&mut self, data: &mut [W]) -> Result<(), Error> {
        if data.is_empty() {
            return Ok(());
        }

        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let ptr = data.as_ptr();
            let len = data.len();

            self.set_word_size(W::CONFIG);
            vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
            vsf_spi_request_transfer(vsf_spi,
                ptr as *mut ::core::ffi::c_void,
                0 as *mut ::core::ffi::c_void,
                len as uint_fast32_t
            );

            let s = self.state;
            poll_fn(|cx| -> Poll<Result<(), Error>> {
                let irq_mask = s.irq_mask.swap(0, Ordering::Relaxed);
                if irq_mask & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as u32 != 0 {
                    vsf_spi_irq_disable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
                    Poll::Ready(Ok(()))
                } else {
                    match _vsf_spi_check_error(irq_mask as into_enum_type!(vsf_spi_irq_mask_t)) {
                        Err(e) => {
                            vsf_spi_irq_disable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
                            Poll::Ready(Err(e))
                        },
                        Ok(()) => {
                            s.waker.register(cx.waker());
                            Poll::Pending
                        },
                    }
                }
            }).await
        }
    }

    /// Bidirectional transfer, using DMA.
    ///
    /// This transfers both buffers at the same time, so it is NOT equivalent to `write` followed by `read`.
    ///
    /// The transfer runs for `max(read.len(), write.len())` bytes. If `read` is shorter extra bytes are ignored.
    /// If `write` is shorter it is padded with zero bytes.
    pub async fn transfer<W: Word>(&mut self, read: &mut [W], write: &[W]) -> Result<(), Error> {
        unsafe {
            let mut read_len = read.len();
            let mut write_len = write.len();
            let mut read_ptr = read.as_ptr() as *mut ::core::ffi::c_void;
            let mut write_ptr = write.as_ptr() as *mut ::core::ffi::c_void;

            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let xfer_len = core::cmp::max(read_len, write_len);
            let xfer_common_len = core::cmp::min(read_len, write_len);
            let mut xfered_len = 0;
            let mut irq_mask: u32 = 0;

            self.set_word_size(W::CONFIG);
            while xfered_len < xfer_len {
                let cur_len = if xfered_len < xfer_common_len {
                    irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as u32 | VSF_SPI_IRQ_MASK_ERR as u32;
                    xfer_common_len
                } else {
                    if xfered_len >= read_len {
                        irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_TX_CPL) as u32;
                        read_ptr = 0 as *mut ::core::ffi::c_void;
                    } else if xfered_len >= write_len {
                        irq_mask = into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as u32 | VSF_SPI_IRQ_MASK_ERR as u32;
                        write_ptr = 0 as *mut ::core::ffi::c_void;
                    }
                    xfer_len - xfered_len
                };

                vsf_spi_irq_enable(vsf_spi, irq_mask as into_enum_type!(vsf_spi_irq_mask_t));
                vsf_spi_request_transfer(vsf_spi,
                    write_ptr, read_ptr,
                    cur_len as uint_fast32_t
                );

                let s = self.state;
                poll_fn(|cx| {
                    let irq_mask = s.irq_mask.swap(0, Ordering::Relaxed);
                    if irq_mask & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as u32 != 0 {
                        Poll::Ready(Ok(()))
                    } else {
                        match _vsf_spi_check_error(irq_mask as into_enum_type!(vsf_spi_irq_mask_t)) {
                            Err(e) => { vsf_spi_irq_disable(vsf_spi, irq_mask as into_enum_type!(vsf_spi_irq_mask_t)); return Poll::Ready(Err(e)) },
                            Ok(()) => { s.waker.register(cx.waker()); Poll::Pending },
                        }
                    }
                }).await?;

                vsf_spi_irq_disable(vsf_spi, irq_mask as into_enum_type!(vsf_spi_irq_mask_t));
                xfered_len += cur_len as usize;
            }
        }

        Ok(())
    }

    /// In-place bidirectional transfer, using DMA.
    ///
    /// This writes the contents of `data` on MOSI, and puts the received data on MISO in `data`, at the same time.
    pub async fn transfer_in_place<W: Word>(&mut self, data: &mut [W]) -> Result<(), Error> {
        if data.is_empty() {
            return Ok(());
        }

        unsafe {
            let vsf_spi = self.info.vsf_spi.load(Ordering::Relaxed);
            let ptr = data.as_ptr();
            let len = data.len();

            self.set_word_size(W::CONFIG);
            vsf_spi_irq_enable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
            vsf_spi_request_transfer(vsf_spi,
                ptr as *mut ::core::ffi::c_void,
                ptr as *mut ::core::ffi::c_void,
                len as uint_fast32_t
            );

            let s = self.state;
            poll_fn(|cx| -> Poll<Result<(), Error>> {
                let irq_mask = s.irq_mask.swap(0, Ordering::Relaxed);
                if irq_mask & into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as u32 != 0 {
                    vsf_spi_irq_disable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
                    Poll::Ready(Ok(()))
                } else {
                    match _vsf_spi_check_error(irq_mask as into_enum_type!(vsf_spi_irq_mask_t)) {
                        Err(e) => {
                            vsf_spi_irq_disable(vsf_spi, into_vsf_spi_irq_mask_t!(VSF_SPI_IRQ_MASK_RX_CPL) as into_enum_type!(vsf_spi_irq_mask_t));
                            Poll::Ready(Err(e))
                        },
                        Ok(()) => {
                            s.waker.register(cx.waker());
                            Poll::Pending
                        },
                    }
                }
            }).await
        }
    }
}

pub(crate) trait SealedWord {
    const CONFIG: word_impl::Config;
}

pub trait Word: word::Word + SealedWord + Default {}

macro_rules! impl_word {
    ($T:ty, $config:expr) => {
        impl SealedWord for $T {
            const CONFIG: Config = $config;
        }
        impl Word for $T {}
    };
}

mod word_impl {
    use super::*;

    #[derive(Copy, Clone, Debug, Eq, PartialEq)]
    enum WordConfig {
        #[cfg(VSF_SPI_DATASIZE_4)]
        BITS4 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_4) as isize,
        #[cfg(VSF_SPI_DATASIZE_5)]
        BITS5 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_5) as isize,
        #[cfg(VSF_SPI_DATASIZE_6)]
        BITS6 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_6) as isize,
        #[cfg(VSF_SPI_DATASIZE_7)]
        BITS7 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_7) as isize,
        BITS8 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_8) as isize,
        #[cfg(VSF_SPI_DATASIZE_9)]
        BITS9 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_9) as isize,
        #[cfg(VSF_SPI_DATASIZE_10)]
        BITS10 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_10) as isize,
        #[cfg(VSF_SPI_DATASIZE_11)]
        BITS11 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_11) as isize,
        #[cfg(VSF_SPI_DATASIZE_12)]
        BITS12 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_12) as isize,
        #[cfg(VSF_SPI_DATASIZE_13)]
        BITS13 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_13) as isize,
        #[cfg(VSF_SPI_DATASIZE_14)]
        BITS14 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_14) as isize,
        #[cfg(VSF_SPI_DATASIZE_15)]
        BITS15 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_15) as isize,
        #[cfg(VSF_SPI_DATASIZE_16)]
        BITS16 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_16) as isize,
        #[cfg(VSF_SPI_DATASIZE_17)]
        BITS17 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_17) as isize,
        #[cfg(VSF_SPI_DATASIZE_18)]
        BITS18 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_18) as isize,
        #[cfg(VSF_SPI_DATASIZE_19)]
        BITS19 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_19) as isize,
        #[cfg(VSF_SPI_DATASIZE_20)]
        BITS20 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_20) as isize,
        #[cfg(VSF_SPI_DATASIZE_21)]
        BITS21 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_21) as isize,
        #[cfg(VSF_SPI_DATASIZE_22)]
        BITS22 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_22) as isize,
        #[cfg(VSF_SPI_DATASIZE_23)]
        BITS23 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_23) as isize,
        #[cfg(VSF_SPI_DATASIZE_24)]
        BITS24 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_24) as isize,
        #[cfg(VSF_SPI_DATASIZE_25)]
        BITS25 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_25) as isize,
        #[cfg(VSF_SPI_DATASIZE_26)]
        BITS26 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_26) as isize,
        #[cfg(VSF_SPI_DATASIZE_27)]
        BITS27 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_27) as isize,
        #[cfg(VSF_SPI_DATASIZE_28)]
        BITS28 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_28) as isize,
        #[cfg(VSF_SPI_DATASIZE_29)]
        BITS29 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_29) as isize,
        #[cfg(VSF_SPI_DATASIZE_30)]
        BITS30 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_30) as isize,
        #[cfg(VSF_SPI_DATASIZE_31)]
        BITS31 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_31) as isize,
        #[cfg(VSF_SPI_DATASIZE_32)]
        BITS32 = into_vsf_spi_mode_t!(VSF_SPI_DATASIZE_32) as isize,
    }
    pub type Config = WordConfig;

    #[cfg(VSF_SPI_DATASIZE_4)]
    impl_word!(word::U4, Config::BITS4);
    #[cfg(VSF_SPI_DATASIZE_5)]
    impl_word!(word::U5, Config::BITS5);
    #[cfg(VSF_SPI_DATASIZE_6)]
    impl_word!(word::U6, Config::BITS6);
    #[cfg(VSF_SPI_DATASIZE_7)]
    impl_word!(word::U7, Config::BITS7);
    impl_word!(u8, Config::BITS8);
    #[cfg(VSF_SPI_DATASIZE_9)]
    impl_word!(word::U9, Config::BITS9);
    #[cfg(VSF_SPI_DATASIZE_10)]
    impl_word!(word::U10, Config::BITS10);
    #[cfg(VSF_SPI_DATASIZE_11)]
    impl_word!(word::U11, Config::BITS11);
    #[cfg(VSF_SPI_DATASIZE_12)]
    impl_word!(word::U12, Config::BITS12);
    #[cfg(VSF_SPI_DATASIZE_13)]
    impl_word!(word::U13, Config::BITS13);
    #[cfg(VSF_SPI_DATASIZE_14)]
    impl_word!(word::U14, Config::BITS14);
    #[cfg(VSF_SPI_DATASIZE_15)]
    impl_word!(word::U15, Config::BITS15);
    #[cfg(VSF_SPI_DATASIZE_16)]
    impl_word!(u16, Config::BITS16);
    #[cfg(VSF_SPI_DATASIZE_17)]
    impl_word!(word::U17, Config::BITS17);
    #[cfg(VSF_SPI_DATASIZE_18)]
    impl_word!(word::U18, Config::BITS18);
    #[cfg(VSF_SPI_DATASIZE_19)]
    impl_word!(word::U19, Config::BITS19);
    #[cfg(VSF_SPI_DATASIZE_20)]
    impl_word!(word::U20, Config::BITS20);
    #[cfg(VSF_SPI_DATASIZE_21)]
    impl_word!(word::U21, Config::BITS21);
    #[cfg(VSF_SPI_DATASIZE_22)]
    impl_word!(word::U22, Config::BITS22);
    #[cfg(VSF_SPI_DATASIZE_23)]
    impl_word!(word::U23, Config::BITS23);
    #[cfg(VSF_SPI_DATASIZE_24)]
    impl_word!(word::U24, Config::BITS24);
    #[cfg(VSF_SPI_DATASIZE_25)]
    impl_word!(word::U25, Config::BITS25);
    #[cfg(VSF_SPI_DATASIZE_26)]
    impl_word!(word::U26, Config::BITS26);
    #[cfg(VSF_SPI_DATASIZE_27)]
    impl_word!(word::U27, Config::BITS27);
    #[cfg(VSF_SPI_DATASIZE_28)]
    impl_word!(word::U28, Config::BITS28);
    #[cfg(VSF_SPI_DATASIZE_29)]
    impl_word!(word::U29, Config::BITS29);
    #[cfg(VSF_SPI_DATASIZE_30)]
    impl_word!(word::U30, Config::BITS30);
    #[cfg(VSF_SPI_DATASIZE_31)]
    impl_word!(word::U31, Config::BITS31);
    #[cfg(VSF_SPI_DATASIZE_32)]
    impl_word!(u32, Config::BITS32);
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



macro_rules! impl_blocking {
    ($w:ident) => {
        impl<M: PeriMode> embedded_hal_02::blocking::spi::Write<$w> for Spi<M> {
            type Error = Error;

            fn write(&mut self, words: &[$w]) -> Result<(), Self::Error> {
                self.blocking_write(words)
            }
        }

        impl<M: PeriMode> embedded_hal_02::blocking::spi::Transfer<$w> for Spi<M> {
            type Error = Error;

            fn transfer<'w>(&mut self, words: &'w mut [$w]) -> Result<&'w [$w], Self::Error> {
                self.blocking_transfer_in_place(words)?;
                Ok(words)
            }
        }
    };
}

impl_blocking!(u8);
#[cfg(VSF_SPI_DATASIZE_16)]
impl_blocking!(u16);
#[cfg(VSF_SPI_DATASIZE_32)]
impl_blocking!(u32);

impl<M: PeriMode> embedded_hal_1::spi::ErrorType for Spi<M> {
    type Error = Error;
}

impl<W: Word, M: PeriMode> embedded_hal_1::spi::SpiBus<W> for Spi<M> {
    fn flush(&mut self) -> Result<(), Self::Error> {
        Ok(())
    }

    fn read(&mut self, words: &mut [W]) -> Result<(), Self::Error> {
        self.blocking_read(words)
    }

    fn write(&mut self, words: &[W]) -> Result<(), Self::Error> {
        self.blocking_write(words)
    }

    fn transfer(&mut self, read: &mut [W], write: &[W]) -> Result<(), Self::Error> {
        self.blocking_transfer(read, write)
    }

    fn transfer_in_place(&mut self, words: &mut [W]) -> Result<(), Self::Error> {
        self.blocking_transfer_in_place(words)
    }
}

impl embedded_hal_1::spi::Error for Error {
    fn kind(&self) -> embedded_hal_1::spi::ErrorKind {
        match *self {
            #[cfg(VSF_SPI_IRQ_MASK_CRC_ERR)]
            Self::Crc => embedded_hal_1::spi::ErrorKind::Other,
            Self::RxOverrun => embedded_hal_1::spi::ErrorKind::Overrun,
        }
    }
}

impl<W: Word> embedded_hal_async::spi::SpiBus<W> for Spi<Async> {
    async fn flush(&mut self) -> Result<(), Self::Error> {
        Ok(())
    }

    async fn write(&mut self, words: &[W]) -> Result<(), Self::Error> {
        self.write(words).await
    }

    async fn read(&mut self, words: &mut [W]) -> Result<(), Self::Error> {
        self.read(words).await
    }

    async fn transfer(&mut self, read: &mut [W], write: &[W]) -> Result<(), Self::Error> {
        self.transfer(read, write).await
    }

    async fn transfer_in_place(&mut self, words: &mut [W]) -> Result<(), Self::Error> {
        self.transfer_in_place(words).await
    }
}