extern crate proc_macro;
use proc_macro::TokenStream;

#[proc_macro]
pub fn bind_vsf_gpios(_item: TokenStream) -> TokenStream {
    let mut output_code = String::from("embassy_hal_internal::peripherals_definition!(");
    for gpio_port_idx in 0..26 {
        let port_ch: char = ('A' as u8 + gpio_port_idx) as char;
        for gpio_pin_idx in 0..64 {
            output_code.push_str(&format!("#[cfg(VSF_HW_GPIO_PORT{gpio_port_idx}_PIN{gpio_pin_idx})]P{port_ch}{gpio_pin_idx},"));
        }
    }
    output_code.push_str(");");
    output_code.parse().unwrap()
}