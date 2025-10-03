
use core::marker::PhantomData;
use core::sync::atomic::{Ordering, AtomicPtr};
use core::ptr;
use paste::paste;

use crate::interrupt::typelevel::Interrupt as _;
use crate::interrupt::{self, Interrupt, InterruptExt};

use crate::vsf_hal::{*};

struct State {}

impl State {
    const fn new() -> Self {
        Self {
        }
    }
}

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
