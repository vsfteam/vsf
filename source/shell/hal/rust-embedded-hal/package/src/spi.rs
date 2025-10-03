
use core::sync::atomic::{Ordering, AtomicPtr};
use core::ptr;

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

);

pin_trait!(SckPin, Instance);
pin_trait!(MisoPin, Instance);
pin_trait!(MosiPin, Instance);
pin_trait!(CsPin, Instance);

use crate::peripherals;
vsf_hal_macros::bind_vsf_peripheral!{spi}
