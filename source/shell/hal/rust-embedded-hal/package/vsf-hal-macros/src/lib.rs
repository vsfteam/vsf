extern crate proc_macro;
use proc_macro::{TokenStream, TokenTree};
use proc_macro2;

use std::path::Path;
use std::env;
use std::fs;

#[proc_macro]
pub fn bind_vsf_gpio_pins(_item: TokenStream) -> TokenStream {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let pathbuf = Path::new(&manifest_dir).join("./src/vsf_hal.rs");
    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();

    let mut output_code = String::from("");
    let mask = extract_peripheral_mask(&bindings_lines, "gpio");
    for port_index in 0..32 {
        if mask & (1 << port_index) != 0 {
            // VSF_HW_GPIO_PORT{port_index}_MASK only the PIN_MASK in one dedicated port, pin number can exceed 32 or maybe 64, so use the largest unsigned integer supported.
            if let Some(mut pin_mask) = extract_const_integer::<u128>(&bindings_lines, &format!("VSF_HW_GPIO_PORT{port_index}_MASK")) {
                let mut pin_index = 0;
                while pin_mask != 0 {
                    if pin_mask & 1 != 0 {
                        output_code.push_str(&format!("
                            impl crate::gpio::Pin for peripherals::P{port_index}_{pin_index} {{
                            }}
                            impl crate::gpio::SealedPin for peripherals::P{port_index}_{pin_index} {{
                                #[inline] fn pin_port(&self) -> PinPortType {{
                                    into_vsf_io_port_pin_no_t!(VSF_P{port_index}_{pin_index}) as PinPortType
                                }}
                            }}
                            impl From<peripherals::P{port_index}_{pin_index}> for crate::gpio::AnyPin {{
                                fn from(_val: peripherals::P{port_index}_{pin_index}) -> Self {{
                                    Self {{
                                        pin_port: into_vsf_io_port_pin_no_t!(VSF_P{port_index}_{pin_index}) as PinPortType,
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

#[proc_macro]
pub fn bind_vsf_usarts(_item: TokenStream) -> TokenStream {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let pathbuf = Path::new(&manifest_dir).join("./src/vsf_hal.rs");
    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();

    let mut output_code = String::from("");
    let mask = extract_peripheral_mask(&bindings_lines, "usart");
    for usart_index in 0..32 {
        if mask & (1 << usart_index) != 0 {
            if let Some(sync_support) = extract_const_integer::<u8>(&bindings_lines, &format!("VSF_HW_USART{usart_index}_SYNC")) {
                if sync_support != 0 {
                    output_code.push_str(&format!("impl_usart!(USART{usart_index}, USART{usart_index}, vsf_hw_usart{usart_index}, USART{usart_index}_IRQHandler);\n"));
                } else {
                    output_code.push_str(&format!("impl_usart!(UART{usart_index}, UART{usart_index}, vsf_hw_usart{usart_index}, UART{usart_index}_IRQHandler);\n"));
                }
            } else {
                output_code.push_str(&format!("impl_usart!(USART{usart_index}, USART{usart_index}, vsf_hw_usart{usart_index}, USART{usart_index}_IRQHandler);\n"));
            }
        }
    }

    output_code.parse().unwrap()
}

#[proc_macro]
pub fn bind_vsf_peripheral(item: TokenStream) -> TokenStream {
    let mut tokens = item.into_iter();
    let peripheral_name = match tokens.next() {
        Some(TokenTree::Ident(ident)) => {
            ident.to_string()
        },
        Some(TokenTree::Literal(literal)) => {
            let peripheral_str = literal.to_string();
            peripheral_str.trim_matches('\"').to_string()
        },
        _ => {
            return syn::Error::new(
                proc_macro2::Span::call_site(),
                "invalid parameter"
            ).to_compile_error().into();
        },
    };
    let irq_type_macro = match tokens.next() {
        Some(TokenTree::Ident(ident)) => {
            ident.to_string()
        },
        _ => "".to_string(),
    };
    let info_macro = match tokens.next() {
        Some(TokenTree::Ident(ident)) => {
            ident.to_string()
        },
        _ => "".to_string(),
    };

    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let pathbuf = Path::new(&manifest_dir).join("./src/vsf_hal.rs");
    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();

    let name_upper = peripheral_name.to_ascii_uppercase();
    let name_lower= peripheral_name.to_ascii_lowercase();
    let mask = extract_peripheral_mask(&bindings_lines, &name_upper);
    let mut output_code = String::from("");

    if info_macro != "" {
        for peripheral_index in 0..32 {
            if mask & (1 << peripheral_index) != 0 {
                if irq_type_macro != "" {
                    output_code.push_str(&format!("
                        peri_trait_impl!({name_upper}{peripheral_index}, {info_macro}! {{ {name_lower}, {name_upper}, {peripheral_index} }}, {irq_type_macro}! {{{name_lower}, {name_upper}, {peripheral_index} }});
                    "));
                } else {
                    output_code.push_str(&format!("
                        peri_trait_impl!({name_upper}{peripheral_index}, {info_macro}! {{ {name_lower}, {name_upper}, {peripheral_index} }});
                    "));
                }
            }
        }
    } else {
        output_code.push_str(&format!("
            pub struct Info {{
                vsf_{name_lower}: AtomicPtr<vsf_{name_lower}_t>,
                vsf_{name_lower}_irqhandler: unsafe extern \"C\" fn(),
                interrupt: Interrupt,
            }}"
        ));

        for peripheral_index in 0..32 {
            if mask & (1 << peripheral_index) != 0 {
                if irq_type_macro != "" {
                    output_code.push_str(&format!("
                        peri_trait_impl!({name_upper}{peripheral_index}, Info {{
                            vsf_{name_lower}: AtomicPtr::new(ptr::addr_of_mut!(vsf_hw_{name_lower}{peripheral_index}) as *mut vsf_hw_{name_lower}_t as *mut vsf_{name_lower}_t),
                            vsf_{name_lower}_irqhandler: {name_upper}{peripheral_index}_IRQHandler,
                            interrupt: crate::interrupt::typelevel::{name_upper}{peripheral_index}::IRQ,
                        }}, {irq_type_macro}! {{{name_lower}, {name_upper}, {peripheral_index} }});
                    "));
                } else {
                    output_code.push_str(&format!("
                        peri_trait_impl!({name_upper}{peripheral_index}, Info {{
                            vsf_{name_lower}: AtomicPtr::new(ptr::addr_of_mut!(vsf_hw_{name_lower}{peripheral_index}) as *mut vsf_hw_{name_lower}_t as *mut vsf_{name_lower}_t),
                            vsf_{name_lower}_irqhandler: {name_upper}{peripheral_index}_IRQHandler,
                            interrupt: crate::interrupt::typelevel::{name_upper}{peripheral_index}::IRQ,
                        }});
                    "));
                }
            }
        }
    }

    output_code.parse().unwrap()
}

fn extract_peripheral_mask(lines: &Vec<&str>, name: &str) -> u32 {
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
