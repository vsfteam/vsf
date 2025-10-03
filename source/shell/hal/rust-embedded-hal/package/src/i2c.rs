
use core::marker::PhantomData;
use core::sync::atomic::{Ordering, AtomicPtr};
use core::ptr;
use paste::paste;

use crate::interrupt::typelevel::Interrupt as _;
use crate::interrupt::{self, Interrupt, InterruptExt};

use crate::vsf_hal::{*};

pub struct Info {
    vsf_i2c: AtomicPtr<vsf_i2c_t>,
    #[cfg(I2C_IRQ_EV)]
    vsf_i2c_ev_irqhandler: unsafe extern "C" fn(),
    #[cfg(I2C_IRQ_EV)]
    interrupt_ev: Interrupt,
    #[cfg(I2C_IRQ_ER)]
    vsf_i2c_er_irqhandler: unsafe extern "C" fn(),
    #[cfg(I2C_IRQ_ER)]
    interrupt_er: Interrupt,
    #[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
    vsf_i2c_irqhandler: unsafe extern "C" fn(),
    #[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
    interrupt: Interrupt,
}

struct State {}

impl State {
    const fn new() -> Self {
        Self {
        }
    }
}

#[cfg(all(I2C_IRQ_EV, I2C_IRQ_ER))]
peri_trait!(
    irqs: [EventInterrupt, ErrorInterrupt],
);

#[cfg(not(any(I2C_IRQ_EV, I2C_IRQ_ER)))]
peri_trait!(
    irqs: [Interrupt],
);

/// Event interrupt handler.
#[cfg(I2C_IRQ_EV)]
pub struct EventInterruptHandler<T: Instance> {
    _phantom: PhantomData<T>,
}

#[cfg(I2C_IRQ_EV)]
impl<T: Instance> interrupt::typelevel::Handler<T::EventInterrupt> for EventInterruptHandler<T> {
    unsafe fn on_interrupt() {
        unsafe {
            (T::info().vsf_i2c_ev_irqhandler)();
        }
    }
}

/// Error interrupt handler.
pub struct ErrorInterruptHandler<T: Instance> {
    _phantom: PhantomData<T>,
}

#[cfg(I2C_IRQ_ER)]
impl<T: Instance> interrupt::typelevel::Handler<T::ErrorInterrupt> for ErrorInterruptHandler<T> {
    unsafe fn on_interrupt() {
        unsafe {
            (T::info().vsf_i2c_er_irqhandler)();
        }
    }
}

/// Normal interrupt handler
#[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
pub struct InterruptHandler<T: Instance> {
    _phantom: PhantomData<T>,
}

#[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
impl<T: Instance> interrupt::typelevel::Handler<T::Interrupt> for InterruptHandler<T> {
    unsafe fn on_interrupt() {
        unsafe {
            (T::info().vsf_i2c_irqhandler)();
        }
    }
}

pin_trait!(SclPin, Instance);
pin_trait!(SdaPin, Instance);
pin_trait!(SmbaPin, Instance);

macro_rules! impl_i2c_info {
    ($name_lower:ident, $name_upper:ident, $peripheral_index:literal) => { paste!{
        Info {
            vsf_i2c: AtomicPtr::new(
                ptr::addr_of_mut!([<vsf_hw_ $name_lower $peripheral_index>])
                    as *mut [<vsf_hw_ $name_lower _t>]
                    as *mut [<vsf_ $name_lower _t>]
            ),
            #[cfg(I2C_IRQ_EV)]
            vsf_i2c_ev_irqhandler: [<I2C $peripheral_index _EV_IRQHandler>],
            #[cfg(I2C_IRQ_EV)]
            interrupt_ev: crate::interrupt::typelevel::[<$name_upper $peripheral_index _EV>]::IRQ,
            #[cfg(I2C_IRQ_ER)]
            vsf_i2c_er_irqhandler: [<I2C $peripheral_index _ER_IRQHandler>],
            #[cfg(I2C_IRQ_ER)]
            interrupt_er: crate::interrupt::typelevel::[<$name_upper $peripheral_index _ER>]::IRQ,
            #[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
            vsf_i2c_irqhandler: [<I2C $peripheral_index _IRQHandler>],
            #[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
            interrupt: crate::interrupt::typelevel::[<$name_upper $peripheral_index>]::IRQ,
        }
    }}
}

macro_rules! impl_i2c_irq_type {
    ($name_lower:ident, $name_upper:ident, $peripheral_index:literal) => { paste!{
        #[cfg(I2C_IRQ_EV)]
        type EventInterrupt = crate::interrupt::typelevel::[<$name_upper $peripheral_index _EV>];
        #[cfg(I2C_IRQ_ER)]
        type ErrorInterrupt = crate::interrupt::typelevel::[<$name_upper $peripheral_index _ER>];
        #[cfg(not(any(I2C_IRQ_ER, I2C_IRQ_EV)))]
        type Interrupt = crate::interrupt::typelevel::[<$name_upper $peripheral_index>];
    }}
}

vsf_hal_macros::bind_vsf_peripheral!{i2c impl_i2c_irq_type impl_i2c_info}
