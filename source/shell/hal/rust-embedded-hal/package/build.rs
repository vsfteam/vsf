use std::path::Path;
use std::env;
use std::fs;
use shellexpand;

const TOML_VSF_NODE: &str = "vsf"; 
const TOML_VSF_PATH_NODE: &str = "path";
const TOML_TARGET_NODE: &str = "target";
const TOML_TARGET_VENDOR_NODE: &str = "vendor";
const TOML_TARGET_MODEL_NODE: &str = "model";
const TOML_TARGET_FLAGS_NODE: &str = "flags";

const PERIPHERIALS: [&'static str; 2] = ["gpio", "usart"];
const OPTIONS: [&'static str; 49] = [
    // GPIO configurations
    "VSF_HW_GPIO_PIN_COUNT",
    "VSF_GPIO_INPUT",
    "VSF_GPIO_OUTPUT",
    "VSF_GPIO_ANALOG",
    "VSF_GPIO_OUTPUT_PUSH_PULL",
    "VSF_GPIO_OUTPUT_OPEN_DRAIN",
    "VSF_GPIO_AF",
    "VSF_GPIO_AF_PUSH_PULL",
    "VSF_GPIO_AF_OPEN_DRAIN",
    "VSF_GPIO_SPEED_MASK",
    "VSF_GPIO_SPEED_LOW",
    "VSF_GPIO_SPEED_MEDIUM",
    "VSF_GPIO_SPEED_HIGH",
    "VSF_GPIO_SPEED_VERY_HIGH",
    "VSF_GPIO_PULL_UP",
    "VSF_GPIO_PULL_DOWN",
    "VSF_GPIO_NO_PULL_UP_DOWN",
    "VSF_GPIO_DRIVE_STRENGTH_LOW",
    "VSF_GPIO_DRIVE_STRENGTH_MEDIUM",
    "VSF_GPIO_DRIVE_STRENGTH_HIGH",
    "VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH",
    // peripherial enable/disable
    "vsf_hw_clkrst_region_set_bit",
    "vsf_hw_clkrst_region_clear_bit",
    "VSF_HW_EN_GPIOA",
    "VSF_HW_EN_GPIOB",
    "VSF_HW_EN_GPIOC",
    "VSF_HW_EN_GPIOD",
    "VSF_HW_EN_GPIOE",
    "VSF_HW_EN_GPIOF",
    "VSF_HW_EN_GPIOG",
    "VSF_HW_EN_GPIOH",
    "VSF_HW_EN_GPIOI",
    "VSF_HW_EN_GPIOJ",
    "VSF_HW_EN_GPIOK",
    "VSF_HW_EN_GPIOL",
    "VSF_HW_EN_GPIOM",
    "VSF_HW_EN_GPION",
    "VSF_HW_EN_GPIOO",
    "VSF_HW_EN_GPIOP",
    "VSF_HW_EN_GPIOQ",
    "VSF_HW_EN_GPIOR",
    "VSF_HW_EN_GPIOS",
    "VSF_HW_EN_GPIOT",
    "VSF_HW_EN_GPIOU",
    "VSF_HW_EN_GPIOV",
    "VSF_HW_EN_GPIOW",
    "VSF_HW_EN_GPIOX",
    "VSF_HW_EN_GPIOY",
    "VSF_HW_EN_GPIOZ",
];

fn main() {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let manifest_path = Path::new(&manifest_dir).join("Cargo.toml");

    let cargo_toml_content = fs::read_to_string(manifest_path).unwrap();
    let cargo_toml: toml::Value = toml::from_str(&cargo_toml_content).unwrap();

    let mut path = String::from("");
    let mut vendor = String::from("");
    let mut model = String::from("");
    let mut flags:Vec<String> = vec![];

    if let Some(vsf_node) = cargo_toml.get(TOML_VSF_NODE) {
        if let Some(path_node) = vsf_node.get(TOML_VSF_PATH_NODE) {
            if let Some(path_str) = path_node.as_str() {
                path = String::from(path_str);
            }
        }

        if let Some(target_node) = vsf_node.get(TOML_TARGET_NODE) {
            if let Some(vendor_node) = target_node.get(TOML_TARGET_VENDOR_NODE) {
                if let Some(vendor_str) = vendor_node.as_str() {
                    vendor = String::from(vendor_str);
                }
            }

            if let Some(model_node) = target_node.get(TOML_TARGET_MODEL_NODE) {
                if let Some(model_str) = model_node.as_str() {
                    model = String::from(model_str);
                }
            }

            if let Some(flags_node) = target_node.get(TOML_TARGET_FLAGS_NODE) {
                if let Some(flags_arr) = flags_node.as_array() {
                    for flag in flags_arr {
                        let flag_option_str = flag.as_str();
                        if let Some(flag_str) = flag_option_str {
                            flags.push(String::from(flag_str));
                        }
                    }
                }
            }
        }
    }

    println!("cargo:rerun-if-changed={path}/src/vsf_hal.rs");
    println!("cargo:warning=path: {path}");
    println!("cargo:warning=target: {vendor}.{model}");
    println!("cargo:warning=flags:");
    for flag in &flags {
        println!("cargo:warning=\t{flag}");
    }

    path = shellexpand::full(&path).expect("Fail to expand for environment variables").into_owned();
    if path.starts_with(".") {
        path.insert(0, '/');
        path.insert_str(0, &manifest_dir);
    }
    if !path.ends_with("/") {
        path.push('/');
    }

    let mut builder = bindgen::Builder::default()
                    .header("".to_string() + &path + "source/hal/driver/driver.h")
                    .raw_line("#![allow(non_camel_case_types)]")
                    .raw_line("#![allow(non_upper_case_globals)]")
                    .raw_line("#![allow(non_snake_case)]")
                    .raw_line("#![allow(dead_code)]")
                    .use_core()
                    .default_enum_style(bindgen::EnumVariation::ModuleConsts)
                    .clang_arg("-D".to_string() + "__" + &vendor + "__")
                    .clang_arg("-D".to_string() + "__" + &model + "__")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/inc")
                    .clang_arg("-I".to_string() + &path + "source");

    for flag in &flags {
        builder = builder.clang_arg(shellexpand::full(&flag).unwrap().into_owned());
    }

    let bindings = builder.generate().expect("Failed to generate rust bindings");

    let pathbuf = Path::new(&manifest_dir).join("./src/vsf_hal.rs");
    bindings.write_to_file(&pathbuf).unwrap();

    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();

    // parse peripherials
    for peripherial in PERIPHERIALS {
        let _mask = enable_peripherial(&bindings_lines, peripherial);
/*
        if peripherial == "gpio" && _mask != 0 {
            for index in 0..32 {
                if _mask & (1 << index) != 0 {
                    if let Some(mut gpio_pin_mask) = extract_const_integer::<u32>(&bindings_lines, &format!("VSF_HW_GPIO_PORT{index}_MASK")) {
                        println!("cargo:warning=VSF_HW_GPIO_PORT{index}_MASK: 0x{gpio_pin_mask:X}");
                        println!("cargo::rustc-check-cfg=cfg(VSF_HW_GPIO_PORT{index}_MASK)");
                        println!("cargo:rustc-cfg=VSF_HW_GPIO_PORT{index}_MASK");

                        let mut pin_index = 0;
                        while gpio_pin_mask != 0 {
                            if gpio_pin_mask & 1 != 0 {
                                println!("cargo:warning=VSF_HW_GPIO_PORT{index}_PIN{pin_index} enabled");
                                println!("cargo::rustc-check-cfg=cfg(VSF_HW_GPIO_PORT{index}_PIN{pin_index})");
                                println!("cargo:rustc-cfg=VSF_HW_GPIO_PORT{index}_PIN{pin_index}");
                            }
                            pin_index += 1;
                            gpio_pin_mask >>= 1;
                        }
                    }
                }
            }
        }
*/
    }

    // parse options
    for option in OPTIONS {
        println!("cargo::rustc-check-cfg=cfg({option})");
        if let Some(_cur_value) = extract_constant_value(&bindings_lines, option) {
            println!("cargo:warning=option: {option} enabled");
            println!("cargo:rustc-cfg={option}");
        }
    }
}

fn enable_peripherial(lines: &Vec<&str>, name: &str) -> u32 {
    let mask = extrace_peripheral_mask(lines, name);

    println!("cargo:warning={name}_mask: 0x{mask:X}");
    println!("cargo::rustc-check-cfg=cfg(vsf_{name}_enabled)");
    println!("cargo:rustc-cfg=vsf_{name}_enabled");

    for index in 0..32 {
        if mask & (1 << index) != 0 {
            println!("cargo:warning={name}: enable {name}{index}");
            println!("cargo::rustc-check-cfg=cfg(vsf_{name}{index}_enabled)");
            println!("cargo:rustc-cfg=vsf_{name}{index}_enabled");
        }
    }
    mask
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
