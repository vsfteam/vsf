use std::path::Path;
use std::process::Command;
use std::env;
use std::fs;
use shellexpand;

const TOML_VSF_NODE: &str = "vsf"; 
const TOML_VSF_PATH_NODE: &str = "path";
const TOML_TARGET_NODE: &str = "target";
const TOML_TARGET_VENDOR_NODE: &str = "vendor";
const TOML_TARGET_MODEL_NODE: &str = "model";
const TOML_TARGET_FLAGS_NODE: &str = "flags";
const TOML_TARGET_MEMORY_NODE: &str = "memory";

const BINDGEN_DEFINITIONS: [&'static str; 2] = [
    "__VSF__",
    "__VSF_CPP__",
];

const PERIPHERIALS: [&'static str; 2] = ["gpio", "usart"];
const CONSTANTS: [&'static str; 67] = [
    // GPIO constants
    "VSF_HW_GPIO_PIN_COUNT",
    "VSF_GPIO_INPUT",
    "VSF_GPIO_OUTPUT",
    "VSF_GPIO_ANALOG",
    "VSF_GPIO_OUTPUT_PUSH_PULL",
    "VSF_GPIO_OUTPUT_OPEN_DRAIN",
    "VSF_GPIO_AF",
    "VSF_GPIO_AF_INPUT",
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
    // USART constants
    "VSF_USART_7_BIT_LENGTH",
    "VSF_USART_8_BIT_LENGTH",
    "VSF_USART_9_BIT_LENGTH",
    "VSF_USART_NO_PARITY",
    "VSF_USART_ODD_PARITY",
    "VSF_USART_EVEN_PARITY",
    "VSF_USART_0_5_STOPBIT",
    "VSF_USART_1_STOPBIT",
    "VSF_USART_1_5_STOPBIT",
    "VSF_USART_2_STOPBIT",
    "VSF_USART_SWAP",
    "VSF_USART_TX_INVERT",
    "VSF_USART_RX_INVERT",
    // peripherial enable/disable
    "VSF_HW_EN_GPIO0",
    "VSF_HW_EN_GPIO1",
    "VSF_HW_EN_GPIO2",
    "VSF_HW_EN_GPIO3",
    "VSF_HW_EN_GPIO4",
    "VSF_HW_EN_GPIO5",
    "VSF_HW_EN_GPIO6",
    "VSF_HW_EN_GPIO7",
    "VSF_HW_EN_GPIO8",
    "VSF_HW_EN_GPIO9",
    "VSF_HW_EN_GPIO10",
    "VSF_HW_EN_GPIO11",
    "VSF_HW_EN_GPIO12",
    "VSF_HW_EN_GPIO13",
    "VSF_HW_EN_GPIO14",
    "VSF_HW_EN_GPIO15",
    "VSF_HW_EN_GPIO16",
    "VSF_HW_EN_GPIO17",
    "VSF_HW_EN_GPIO18",
    "VSF_HW_EN_GPIO19",
    "VSF_HW_EN_GPIO20",
    "VSF_HW_EN_GPIO21",
    "VSF_HW_EN_GPIO22",
    "VSF_HW_EN_GPIO23",
    "VSF_HW_EN_GPIO24",
    "VSF_HW_EN_GPIO25",
    "VSF_HW_EN_GPIO26",
    "VSF_HW_EN_GPIO27",
    "VSF_HW_EN_GPIO28",
    "VSF_HW_EN_GPIO29",
    "VSF_HW_EN_GPIO30",
    "VSF_HW_EN_GPIO31",
];
const FUNCTIONS: [&'static str; 2] = [
    "vsf_hw_clkrst_region_set_bit",
    "vsf_hw_clkrst_region_clear_bit",
];
const MODULES: [&'static str; 1] = [
    "vsf_hw_peripheral_en_t",
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

            if let Some(memory_node) = target_node.get(TOML_TARGET_MEMORY_NODE) {
                if let Some(memory_str) = memory_node.as_str() {
                    println!("cargo:rerun-if-changed={memory_str}");
                    fs::copy(memory_str, "./memory.x").expect("Fail to copy memory.x, is memory setting in Cargo.toml correct?");
                }
            }
        }
    }

    println!("cargo:rustc-link-arg-bins=--nmagic");
    println!("cargo:rustc-link-arg-bins=-Tlink.x");
    println!("cargo:rustc-link-arg-bins=-Tdefmt.x");

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

    {
        let vsf_hal_build_path = Path::new("./vsf_hal_build");
        if !vsf_hal_build_path.exists() {
            fs::create_dir_all(&vsf_hal_build_path).unwrap();
            Command::new("cmake").current_dir(&vsf_hal_build_path).arg("-GNinja").arg("-DVSF_TARGET=".to_string() + &model).arg(path.clone() + "source/shell/hal/rust-embedded-hal/").output().expect("Fail to run cmake");
            Command::new("ninja").current_dir(&vsf_hal_build_path).output().expect("Fail to run ninja");
        }

        println!("cargo:rustc-link-search=native={}", vsf_hal_build_path.to_str().unwrap());
        println!("cargo:rustc-link-lib=static=vsf_hal");
    }

    fs::write("./vsf_c.h", "
        #include <service/vsf_service.h>
        #include <hal/vsf_hal.h>
    ").unwrap();

    let mut builder = bindgen::Builder::default()
                    .header("./vsf_c.h")
                    .raw_line("#![allow(non_camel_case_types)]")
                    .raw_line("#![allow(non_upper_case_globals)]")
                    .raw_line("#![allow(non_snake_case)]")
                    .raw_line("#![allow(dead_code)]")
                    .use_core()
                    .default_enum_style(bindgen::EnumVariation::ModuleConsts)
                    .clang_arg("-D".to_string() + "__" + &vendor + "__")
                    .clang_arg("-D".to_string() + "__" + &model + "__")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/inc")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/inc/empty_libc")
                    .clang_arg("-I".to_string() + &path + "source");

    for definition in BINDGEN_DEFINITIONS {
        builder = builder.clang_arg("-D".to_string() + definition);
    }
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
        enable_peripherial(&bindings_lines, peripherial);
    }

    // parse constants
    for constant in CONSTANTS {
        println!("cargo::rustc-check-cfg=cfg({constant})");
        if let Some(_cur_value) = extract_constant_value(&bindings_lines, constant) {
            println!("cargo:warning=constant: {constant} enabled");
            println!("cargo:rustc-cfg={constant}");
        }
    }

    // parse functions
    for func in FUNCTIONS {
        println!("cargo::rustc-check-cfg=cfg({func})");
        if extract_function(&bindings_lines, func) {
            println!("cargo:warning=function: {func} enabled");
            println!("cargo:rustc-cfg={func}");
        }
    }

    // parse modules
    for module in MODULES {
        println!("cargo::rustc-check-cfg=cfg({module})");
        if extract_module(&bindings_lines, module) {
            println!("cargo:warning=module: {module} enabled");
            println!("cargo:rustc-cfg={module}");
        }
    }
}

fn enable_peripherial(lines: &Vec<&str>, name: &str) -> u32 {
    println!("cargo::rustc-check-cfg=cfg(vsf_{name}_enabled)");
    for peripheral_index in 0..32 {
        println!("cargo::rustc-check-cfg=cfg(vsf_{name}{peripheral_index}_enabled)");
    }

    let mask = extrace_peripheral_mask(lines, name);
    if mask != 0 {
        println!("cargo:warning={name}_mask: 0x{mask:X}");
        println!("cargo:rustc-cfg=vsf_{name}_enabled");

        for peripheral_index in 0..32 {
            if mask & (1 << peripheral_index) != 0 {
                println!("cargo:warning={name}: enable {name}{peripheral_index}");
                println!("cargo:rustc-cfg=vsf_{name}{peripheral_index}_enabled");
            }
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

fn extract_function(lines: &Vec<&str>, name: &str) -> bool {
    let mut matched = false;
    let prefix = String::from(name) + "(";
    for line in lines {
        let parts: Vec<&str> = line.split_whitespace().collect();
        // pub fn FUNC_NAME(...);
        if parts.len() >= 3 && parts[0] == "pub" && parts[1] == "fn" {
            if parts[2].starts_with(&prefix) {
                matched = true;
                break;
            }
        }
    }
    return matched;
}

fn extract_module(lines: &Vec<&str>, name: &str) -> bool {
    let mut matched = false;
    for line in lines {
        let parts: Vec<&str> = line.split_whitespace().collect();
        // pub mod MODULE_NAME {
        if parts.len() >= 3 && parts[0] == "pub" && parts[1] == "mod" {
            if parts[2] == name {
                matched = true;
                break;
            }
        }
    }
    return matched;
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
