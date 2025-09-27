#![macro_use]

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
        #[doc = concat!(stringify!($signal), " pin trait")]
        pub trait $signal<T: $instance>: crate::gpio::Pin {
            #[doc = concat!("Get the AF number needed to use this pin as ", stringify!($signal))]
            fn af_num(&self) -> u8;
        }
    };
}

macro_rules! pin_trait_impl {
    (crate::$mod:ident::$trait:ident$(<$mode:ident>)?, $instance:ident, $pin:ident, $af:expr) => {
        impl crate::$mod::$trait<crate::peripherals::$instance> for crate::peripherals::$pin {
            fn af_num(&self) -> u8 {
                $af
            }
        }
    };
}
