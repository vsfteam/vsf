extern crate proc_macro;
use proc_macro::TokenStream;

use std::path::Path;
use std::env;
use std::fs;

#[proc_macro]
pub fn bind_vsf_peripherials(_item: TokenStream) -> TokenStream {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let pathbuf = Path::new(&manifest_dir).join("./src/vsf_hal.rs");
    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();

    let mut gpio_output_code = String::from("");
    bind_vsf_gpios(&bindings_lines, &mut gpio_output_code);
    let mut usart_output_code = String::from("");
    bind_vsf_peripheral(&bindings_lines, "usart", &mut usart_output_code);

    let mut output_code = String::from("embassy_hal_internal::peripherals_definition!(");
    output_code.push_str(&gpio_output_code);
    output_code.push_str(&usart_output_code);
    output_code.push_str(");");

    output_code.push_str("embassy_hal_internal::peripherals_struct!(");
    output_code.push_str(&gpio_output_code);
    output_code.push_str(&usart_output_code);
    output_code.push_str(");");

    output_code.parse().unwrap()
}

#[proc_macro]
pub fn bind_vsf_gpio_pins(_item: TokenStream) -> TokenStream {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let pathbuf = Path::new(&manifest_dir).join("./src/vsf_hal.rs");
    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();

    let mut output_code = String::from("");
    let mask = extrace_peripheral_mask(&bindings_lines, "gpio");
    for index in 0..32 {
        if mask & (1 << index) != 0 {
            let port_ch: char = ('A' as u8 + index) as char;
            if let Some(mut pin_mask) = extract_const_integer::<u32>(&bindings_lines, &format!("VSF_HW_GPIO_PORT{index}_MASK")) {
                let mut pin_index = 0;
                while pin_mask != 0 {
                if pin_mask & 1 != 0 {
                    output_code.push_str(&format!("
                        impl Pin for peripherals::P{port_ch}{pin_index} {{
                        }}
                        impl SealedPin for peripherals::P{port_ch}{pin_index} {{
                            #[inline] fn pin_port(&self) -> PinPortType {{
                                ({index} << 8) + {pin_index}
                            }}
                        }}
                        impl From<peripherals::P{port_ch}{pin_index}> for AnyPin {{
                            fn from(val: peripherals::P{port_ch}{pin_index}) -> Self {{
                                Self {{
                                    pin_port: val.pin_port(),
                                }}
                            }}
                        }}
                    "));
                }
                pin_index += 1;
                pin_mask >>= 1;
                }
            }
        }
    }

    output_code.parse().unwrap()
}

fn bind_vsf_gpios(lines: &Vec<&str>, output_code: &mut String) {
    let mask = extrace_peripheral_mask(lines, "gpio");

    for index in 0..32 {
        if mask & (1 << index) != 0 {
            let port_ch: char = ('A' as u8 + index) as char;
            if let Some(mut pin_mask) = extract_const_integer::<u32>(&lines, &format!("VSF_HW_GPIO_PORT{index}_MASK")) {
                let mut pin_index = 0;
                while pin_mask != 0 {
                if pin_mask & 1 != 0 {
                    output_code.push_str(&format!("P{port_ch}{pin_index},"));
                }
                pin_index += 1;
                pin_mask >>= 1;
                }
            }
        }
    }
}

fn bind_vsf_peripheral(lines: &Vec<&str>, name: &str, output_code: &mut String) {
    let peripheral_name_upper = String::from(name).to_uppercase();
    let mask = extrace_peripheral_mask(lines, &peripheral_name_upper);

    for index in 0..32 {
        if mask & (1 << index) != 0 {
            output_code.push_str(&format!("{peripheral_name_upper}{index},"));
        }
    }
}

fn extrace_peripheral_mask(lines: &Vec<&str>, name: &str) -> u32 {
    let prefix_str = "VSF_HW_".to_string() + &String::from(name).to_uppercase();
    let mask_str = String::from(&prefix_str) + "_MASK";
    let count_str = String::from(&prefix_str) + "_COUNT";

    let mask_option = extract_const_integer::<u32>(lines, &mask_str);
    if let Some(mask) = mask_option {
        mask
    } else {
        let count_option = extract_const_integer::<u32>(lines, &count_str);
        if let Some(count) = count_option {
            if count >= 32 {
                return 0u32;
            }
            (1u32 << count) - 1
        } else {
            0u32
        }
    }
}

fn extract_const_integer<T>(lines: &Vec<&str>, name: &str) -> Option<T>
where
    T: std::str::FromStr,
    <T as std::str::FromStr>::Err : std::fmt::Debug,
{
    if let Some(cur_value) = extract_constant_value(lines, name) {
        let value = String::from(cur_value).parse::<T>().unwrap();
        Some(value)
    } else {
        None
    }
}

fn extract_constant_value<'a>(lines: &'a Vec<&str>, name: &str) -> Option<&'a str> {
    for line in lines {
        let parts: Vec<&str> = line.split_whitespace().collect();
        // pub const CONST_NAME: TYPE = CONST_VALUE;
        if parts.len() >= 6 && parts[0] == "pub" && parts[1] == "const" {
            let cur_name = parts[2].trim_end_matches(':');
            if cur_name == name {
                let cur_value = parts[5].trim_end_matches(';');
                return Some(cur_value);
            }
        }
    }
    None
}
