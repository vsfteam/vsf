#![macro_use]

macro_rules! new_pin {
    ($name:ident, $af_type:expr) => {{
        let pin = $name;
        #[cfg(afio)]
        pin.afio_remap();
        pin.set_as_af(
            #[cfg(not(afio))]
            pin.af_num(),
            $af_type,
        );
        Some(pin.into())
    }};
}
