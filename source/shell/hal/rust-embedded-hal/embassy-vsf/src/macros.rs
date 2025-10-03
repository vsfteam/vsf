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
