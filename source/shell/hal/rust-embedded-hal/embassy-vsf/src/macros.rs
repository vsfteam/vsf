#![macro_use]

macro_rules! peri_trait {
    (
        $(irqs: [$($irq:ident),*],)?
    ) => {
        #[allow(private_interfaces)]
        pub(crate) trait SealedInstance {
            #[allow(unused)]
            fn info() -> &'static Info;
            #[allow(unused)]
            fn state() -> &'static State;
        }

        /// Peripheral instance trait.
        #[allow(private_bounds)]
        pub trait Instance: SealedInstance + crate::PeripheralType + 'static {
            $($(
                /// Interrupt for this peripheral.
                type $irq: crate::interrupt::typelevel::Interrupt;
            )*)?
        }
    };
}

macro_rules! peri_trait_impl {
    ($instance:ident, $info:expr $(, $($irq_type:item)* )?) => {
        #[allow(private_interfaces)]
        impl SealedInstance for crate::peripherals::$instance {
            fn info() -> &'static Info {
                static INFO: Info = $info;
                &INFO
            }
            fn state() -> &'static State {
                static STATE: State = State::new();
                &STATE
            }
        }
        impl Instance for crate::peripherals::$instance {
            $($($irq_type)*)?
        }
    };
}

macro_rules! new_pin {
    ($name:ident, $af_type:expr) => {{
        let pin = $name;
        pin.set_as_af(
            pin.af_num(),
            $af_type,
        );
        Some(pin.into())
    }};
}

macro_rules! pin_trait {
    ($signal:ident, $instance:path) => {
        pub trait $signal<T: $instance>: crate::gpio::Pin {
            fn af_num(&self) -> crate::gpio::AfNumType;
        }
    };
}

macro_rules! pin_trait_impl {
    (crate::$mod:ident::$trait:ident$(<$mode:ident>)?, $instance:ident, $pin:ident, $af:expr) => {
        impl crate::$mod::$trait<crate::peripherals::$instance> for crate::peripherals::$pin {
            fn af_num(&self) -> crate::gpio::AfNumType {
                $af
            }
        }
    };
}

macro_rules! __interrupt_mod {
    ($($irqs:ident),* $(,)?) => {
        pub mod interrupt {
            pub use crate::pac::Interrupt::*;
            pub use crate::pac::Interrupt;

            pub unsafe trait InterruptNumber: Copy {
                fn number(self) -> u16;
            }
            pub unsafe trait InterruptExt: InterruptNumber + Copy {
                #[inline]
                unsafe fn enable(self) {}
                #[inline]
                unsafe fn disable(self) {}
                #[inline]
                unsafe fn pend(self) {}
                #[inline]
                unsafe fn unpend(self) {}
            }

            pub mod typelevel {
                use super::InterruptExt;
                trait SealedInterrupt {}
                pub trait Interrupt: SealedInterrupt {
                    const IRQ: super::Interrupt;
                    #[inline]
                    unsafe fn enable() {}
                    #[inline]
                    unsafe fn disable() {}
                    #[inline]
                    unsafe fn pend() {}
                    #[inline]
                    unsafe fn unpend() {}
                }

                $(
                    #[allow(non_camel_case_types)]
                    #[doc=stringify!($irqs)]
                    #[doc=" typelevel interrupt."]
                    pub enum $irqs {}
                    impl SealedInterrupt for $irqs{}
                    impl Interrupt for $irqs {
                        const IRQ: super::Interrupt = super::Interrupt::$irqs;
                    }
                )*

                pub trait Handler<I: Interrupt> {
                    unsafe fn on_interrupt();
                }
                pub unsafe trait Binding<I: Interrupt, H: Handler<I>> {}
            }
        }
    }
}

macro_rules! interrupt_mod {
    (Windows, $($irqs:ident),* $(,)?) => {
        __interrupt_mod!($($irqs,)*);
    };
    (Unix, $($irqs:ident),* $(,)?) => {
        __interrupt_mod!($($irqs,)*);
    };
    (CortexM, $($irqs:ident),* $(,)?) => {
        unsafe impl cortex_m::interrupt::InterruptNumber for crate::pac::Interrupt {
            #[inline(always)]
            fn number(self) -> u16 {
                self as u16
            }
        }
        embassy_hal_internal::interrupt_mod!($($irqs,)*);
    };
    ($_arch:ident, $($irqs:ident),* $(,)?) => {
        embassy_hal_internal::interrupt_mod!($($irqs,)*);
    };
}