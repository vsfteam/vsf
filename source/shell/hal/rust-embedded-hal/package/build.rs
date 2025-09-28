use std::path::Path;
use std::process::Command;
use std::sync::Mutex;
use std::collections::{HashMap, BTreeMap};
use std::env;
use std::fs;
use shellexpand;

const BINDGEN_ENUM_VAR: bindgen::EnumVariation = bindgen::EnumVariation::Consts;

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

const CONSTANTS_MACRO: [(&'static str, &'static str); 28] = [
    // USART constants
    ("vsf_usart_mode_t", "VSF_USART_5_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_6_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_7_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_8_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_9_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_10_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_NO_PARITY"),
    ("vsf_usart_mode_t", "VSF_USART_ODD_PARITY"),
    ("vsf_usart_mode_t", "VSF_USART_EVEN_PARITY"),
    ("vsf_usart_mode_t", "VSF_USART_0_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_1_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_1_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_2_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_SYNC_CLOCK_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_DISABLE"),
    ("vsf_usart_mode_t", "VSF_USART_SWAP"),
    ("vsf_usart_mode_t", "VSF_USART_TX_INVERT"),
    ("vsf_usart_mode_t", "VSF_USART_RX_INVERT"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_FRAME_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_PARITY_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_BREAK_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_IDLE"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SEND_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SET_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_CLEAR_BREAK"),
];

const CONSTANTS_ENUM: [(&'static str, &'static str); 82] = [
    // GPIO constants
    ("", "VSF_HW_GPIO_PIN_COUNT"),
    ("vsf_gpio_mode_t", "VSF_GPIO_INPUT"),
    ("vsf_gpio_mode_t", "VSF_GPIO_OUTPUT"),
    ("vsf_gpio_mode_t", "VSF_GPIO_ANALOG"),
    ("vsf_gpio_mode_t", "VSF_GPIO_OUTPUT_PUSH_PULL"),
    ("vsf_gpio_mode_t", "VSF_GPIO_OUTPUT_OPEN_DRAIN"),
    ("vsf_gpio_mode_t", "VSF_GPIO_AF"),
    ("vsf_gpio_mode_t", "VSF_GPIO_AF_INPUT"),
    ("vsf_gpio_mode_t", "VSF_GPIO_AF_PUSH_PULL"),
    ("vsf_gpio_mode_t", "VSF_GPIO_AF_OPEN_DRAIN"),
    ("vsf_gpio_mode_t", "VSF_GPIO_SPEED_MASK"),
    ("vsf_gpio_mode_t", "VSF_GPIO_SPEED_LOW"),
    ("vsf_gpio_mode_t", "VSF_GPIO_SPEED_MEDIUM"),
    ("vsf_gpio_mode_t", "VSF_GPIO_SPEED_HIGH"),
    ("vsf_gpio_mode_t", "VSF_GPIO_SPEED_VERY_HIGH"),
    ("vsf_gpio_mode_t", "VSF_GPIO_PULL_UP"),
    ("vsf_gpio_mode_t", "VSF_GPIO_PULL_DOWN"),
    ("vsf_gpio_mode_t", "VSF_GPIO_NO_PULL_UP_DOWN"),
    ("vsf_gpio_mode_t", "VSF_GPIO_DRIVE_STRENGTH_LOW"),
    ("vsf_gpio_mode_t", "VSF_GPIO_DRIVE_STRENGTH_MEDIUM"),
    ("vsf_gpio_mode_t", "VSF_GPIO_DRIVE_STRENGTH_HIGH"),
    ("vsf_gpio_mode_t", "VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH"),
    // USART constants
    ("vsf_usart_mode_t", "VSF_USART_5_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_6_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_7_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_8_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_9_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_10_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_NO_PARITY"),
    ("vsf_usart_mode_t", "VSF_USART_ODD_PARITY"),
    ("vsf_usart_mode_t", "VSF_USART_EVEN_PARITY"),
    ("vsf_usart_mode_t", "VSF_USART_0_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_1_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_1_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_2_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_SYNC_CLOCK_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_DISABLE"),
    ("vsf_usart_mode_t", "VSF_USART_SWAP"),
    ("vsf_usart_mode_t", "VSF_USART_TX_INVERT"),
    ("vsf_usart_mode_t", "VSF_USART_RX_INVERT"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_FRAME_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_PARITY_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_BREAK_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_IDLE"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SEND_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SET_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_CLEAR_BREAK"),
    // peripherial enable/disable
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO0"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO1"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO2"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO3"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO4"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO5"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO6"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO7"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO8"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO9"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO10"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO11"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO12"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO13"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO14"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO15"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO16"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO17"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO18"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO19"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO20"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO21"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO22"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO23"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO24"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO25"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO26"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO27"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO28"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO29"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO30"),
    ("vsf_hw_peripheral_en_t", "VSF_HW_EN_GPIO31"),
];

/// if constants conflicts, {1.1} will be removed, and {1.1}__IS__{1.0} configuration will be valid
const CONSTANTS_CONFLICT: [(&'static str, (&'static str, &'static str)); 1] = [
    ("vsf_usart_irq_mask_t", ("VSF_USART_IRQ_MASK_FRAME_ERR", "VSF_USART_IRQ_MASK_BREAK_ERR")),
];

const FUNCTIONS: [&'static str; 2] = [
    "vsf_hw_clkrst_region_set_bit",
    "vsf_hw_clkrst_region_clear_bit",
];
const TYPES: [&'static str; 1] = [
    "vsf_hw_peripheral_en_t",
];

struct PeripheralAfInfo {
    module: &'static str,
    name: &'static str,
    pins: HashMap<&'static str, &'static str>,
}

use lazy_static::lazy_static;
lazy_static! {
    static ref GLOBAL_INTERRUPT_VEC: Mutex<Vec<String>> = Mutex::new({
        let v = Vec::new();
        v
    });
    static ref GLOBAL_AF_MAP: Mutex<HashMap<String, u8>> = Mutex::new({
        let m = HashMap::new();
        m
    });
    static ref GLOBAL_AF_INFO: Mutex<HashMap<&'static str, PeripheralAfInfo>> = Mutex::new({
        let mut m = HashMap::new();
        m.insert("USART", PeripheralAfInfo {
            module: "usart",
            name: "Usart",
            pins: HashMap::from([
                ("TX", "TxPin"),
                ("RX", "RxPin"),
                ("DE", "DePin"),
                ("CTS", "CtsPin"),
                ("RTS", "RtsPin"),
                ("CK", "CkPin"),
            ]),
        });
        m
    });
    static ref GLOBAL_CONSTANTS_VEC: Mutex<Vec<(&'static str, &'static str)>> = Mutex::new({
        let v = Vec::new();
        v
    });
}

use bindgen::callbacks::{
    Token, ParseCallbacks,
};

#[derive(Debug)]
struct Callbacks {}

impl ParseCallbacks for Callbacks {
    fn modify_macro(&self, name: &str, tokens: &mut Vec<Token>) {
        let name_str = String::from(name);
        if name_str.starts_with("VSF_HW_INTERRUPT") && tokens.len() == 2 {
            if let Ok(interrupt_index) = String::from(&name_str[16..]).parse::<u8>() {
                let value_str = String::from_utf8_lossy(&tokens[1].raw).into_owned();
                let mut interrupt_vec = GLOBAL_INTERRUPT_VEC.lock().unwrap();

                if interrupt_index as usize >= interrupt_vec.len() {
                    interrupt_vec.resize(interrupt_index as usize + 1, String::from(""));
                }
                interrupt_vec[interrupt_index as usize] = value_str.clone();
            }
        } else if name_str.starts_with("VSF_HW_AF_") && tokens.len() == 2 {
            let value: u8 = String::from_utf8_lossy(&tokens[1].raw).parse().unwrap();
            let mut af_map = GLOBAL_AF_MAP.lock().unwrap();
            af_map.insert(String::from(name_str.strip_prefix("VSF_HW_AF_").unwrap()), value);
        } else {
            let mut constants_vec = GLOBAL_CONSTANTS_VEC.lock().unwrap();
            for constant in CONSTANTS_MACRO {
                if constant.1 == name {
                    constants_vec.push((constant.0, constant.1));
                }
            }
        }
    }
}

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
            Command::new("cmake").current_dir(&vsf_hal_build_path).arg("-GNinja").arg("-DVSF_TARGET=".to_string() + &model).arg(path.clone() + "source/shell/hal/rust-embedded-hal/lib").output().expect("Fail to run cmake");
            Command::new("ninja").current_dir(&vsf_hal_build_path).output().expect("Fail to run ninja");
        }

        println!("cargo:rustc-link-search=native={}", vsf_hal_build_path.to_str().unwrap());
        println!("cargo:rustc-link-lib=static=vsf_hal");
    }

    fs::write("./vsf_c.h", "
        #include <service/vsf_service.h>
        #include <hal/vsf_hal.h>
    ").unwrap();

    println!("cargo::rustc-check-cfg=cfg(bindgen_enum_type_consts)");
    println!("cargo::rustc-check-cfg=cfg(bindgen_enum_type_moduleconsts)");
    if BINDGEN_ENUM_VAR == bindgen::EnumVariation::Consts {
        println!("cargo:rustc-cfg=bindgen_enum_type_consts");
    } else if BINDGEN_ENUM_VAR == bindgen::EnumVariation::ModuleConsts {
        println!("cargo:rustc-cfg=bindgen_enum_type_moduleconsts");
    }

    let mut builder = bindgen::Builder::default()
                    .header("./vsf_c.h")
                    .parse_callbacks(Box::new(Callbacks{}))
                    .raw_line("#![allow(non_camel_case_types)]")
                    .raw_line("#![allow(non_upper_case_globals)]")
                    .raw_line("#![allow(non_snake_case)]")
                    .raw_line("#![allow(dead_code)]")
                    .use_core()
                    .default_enum_style(BINDGEN_ENUM_VAR)
                    .clang_arg("-D".to_string() + "__" + &vendor + "__")
                    .clang_arg("-D".to_string() + "__" + &model + "__")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/lib/inc")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/lib/inc/empty_libc")
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
    let mut constants_cfg: BTreeMap<(&'static str, &'static str), u64> = BTreeMap::new();
    let constants_vec = GLOBAL_CONSTANTS_VEC.lock().unwrap();
    'outer: for constant_enum in CONSTANTS_ENUM {
        println!("cargo::rustc-check-cfg=cfg({})", constant_enum.1);
        if let Some(cur_value) = extract_const_integer::<u64>(&bindings_lines, &(String::from(constant_enum.0) + "_" + constant_enum.1)) {
            for constant_macro in CONSTANTS_MACRO {
                if constant_macro.1 == constant_enum.1 && !constants_vec.contains(&constant_macro) {
                    continue 'outer;
                }
            }
            constants_cfg.insert((constant_enum.0, constant_enum.1), cur_value);
        }
    }
    for constant_conflict in CONSTANTS_CONFLICT {
        println!("cargo::rustc-check-cfg=cfg({}__IS__{})", constant_conflict.1.1, constant_conflict.1.0);
        if let Some(constant_conflict0) = constants_cfg.get(&(constant_conflict.0, constant_conflict.1.0)) {
            if let Some(constant_conflict1) = constants_cfg.get(&(constant_conflict.0, constant_conflict.1.1)) {
                if *constant_conflict0 == *constant_conflict1 {
                    constants_cfg.remove(&(constant_conflict.0, constant_conflict.1.1));
                    println!("cargo:rustc-cfg={}__IS__{}", constant_conflict.1.1, constant_conflict.1.0);
                }
            }
        }
    }

    for constant in constants_cfg {
        println!("cargo:warning=constant: {}::{} enabled", constant.0.0, constant.0.1);
        println!("cargo:rustc-cfg={}", constant.0.1);
    }

    // parse functions
    for func in FUNCTIONS {
        println!("cargo::rustc-check-cfg=cfg({func})");
        if extract_function(&bindings_lines, func) {
            println!("cargo:warning=function: {func} enabled");
            println!("cargo:rustc-cfg={func}");
        }
    }

    // parse types
    for _type in TYPES {
        println!("cargo::rustc-check-cfg=cfg({_type})");
        if BINDGEN_ENUM_VAR == bindgen::EnumVariation::Consts {
            if extract_type(&bindings_lines, _type) {
                println!("cargo:warning=type: {_type} enabled");
                println!("cargo:rustc-cfg={_type}");
            }
        } else if BINDGEN_ENUM_VAR == bindgen::EnumVariation::ModuleConsts {
            if extract_module(&bindings_lines, _type) {
                println!("cargo:warning=type: {_type} enabled");
                println!("cargo:rustc-cfg={_type}");
            }
        }
    }

    // parse interrupts and generate device.x and pac.rs
    let mut device_x_str = String::from("");
    let mut interrupt_str = String::from("");
    let mut interrupt_func_dec_str = String::from("");
    let mut interrupt_vecotr_str = String::from("");
    let mut interrupt_name_str = String::from("");
    let mut interrupt_num: u8 = 0;
    if let Some(interrupt_num_tmp) = extract_const_integer::<u8>(&bindings_lines, "VSF_HW_INTERRUPTS_NUM") {
        interrupt_num = interrupt_num_tmp;
        let interrupt_vec = GLOBAL_INTERRUPT_VEC.lock().unwrap();
        for interrupt_index in 0..interrupt_vec.len() {
            if interrupt_vec[interrupt_index].len() > 0 {
                let interrupt_name = interrupt_vec[interrupt_index].strip_suffix("_IRQHandler").unwrap();

                println!("cargo:warning=irq{interrupt_index}: {interrupt_name}");
                interrupt_name_str.push_str(&format!("{interrupt_name},\n"));
                device_x_str.push_str(&format!("PROVIDE({interrupt_name} = DefaultHandler);\n"));
                interrupt_str.push_str(&format!("{interrupt_name} = {interrupt_index},\n"));
                interrupt_func_dec_str.push_str(&format!("fn {interrupt_name}();\n"));
                interrupt_vecotr_str.push_str(&format!("Vector {{ _handler: {interrupt_name} }},\n"));
            } else {
                interrupt_vecotr_str.push_str("Vector { _reserved: 0: {interrupt_name} },\n");
            }
        }
    }
    fs::write("./device.x", device_x_str).unwrap();
    let pac_rs_str = String::from(&format!("
        #[derive(Copy, Clone, Debug, PartialEq, Eq)]
        #[cfg_attr(feature = \"defmt\", derive(defmt::Format))]
        #[allow(non_camel_case_types)]
        pub enum Interrupt {{
            {interrupt_str}
        }}
        unsafe impl cortex_m::interrupt::InterruptNumber for Interrupt {{
            #[inline(always)]
            fn number(self) -> u16 {{
                self as u16
            }}
        }}
        mod _vectors {{
            unsafe extern \"C\" {{
                {interrupt_func_dec_str}
            }}

            pub union Vector {{
                _handler: unsafe extern \"C\" fn(),
                _reserved: u32,
            }}
            #[unsafe(link_section = \".vector_table.interrupts\")]
            #[unsafe(no_mangle)]
            #[allow(non_camel_case_types)]
            pub static __INTERRUPTS: [Vector; {interrupt_num}] = [
                {interrupt_vecotr_str}
            ];
        }}
    "));
    fs::write("./src/pac.rs", pac_rs_str).unwrap();

    // parse alternate functions and generate _generated.rs
    let mut af_str = String::from("");
    let af_map = GLOBAL_AF_MAP.lock().unwrap();
    let afs_info = GLOBAL_AF_INFO.lock().unwrap();
    for af in af_map.iter() {
        let str = af.0;
        let mut gpio_pos = str.rfind('_').unwrap();
        gpio_pos = str[..gpio_pos].rfind('_').unwrap();
        let gpio_str = &str[gpio_pos + 1 ..];
        let pin_pos = str.find('_').unwrap();
        let pin_str = &str[pin_pos + 1 .. gpio_pos];
        let peripheral_str = &str[..pin_pos];
        let peripheral_type_str = peripheral_str.trim_end_matches(|c: char| c.is_ascii_digit());
        let peripheral_index_str = &peripheral_str[peripheral_type_str.len()..];
        let af = af.1;

        if let Some(af_info) = afs_info.get(peripheral_type_str) {
            if let Some(af_pin) = af_info.pins.get(pin_str) {
                println!("cargo:warning={}::{peripheral_type_str}{peripheral_index_str}: {pin_str} on {gpio_str} ==> AF{af}", af_info.module);
                af_str.push_str(&format!("pin_trait_impl!(crate::{}::{}, {peripheral_str}, {gpio_str}, {af}u16);\n", af_info.module, *af_pin));
            }
        }
    }

    // bind vsf peripherals
    let mut generated_rs_str = String::from("");
    let mut gpio_output_code = String::from("");
    bind_vsf_gpios(&bindings_lines, &mut gpio_output_code);
    let mut usart_output_code = String::from("");
    bind_vsf_peripheral(&bindings_lines, "usart", &mut usart_output_code);

    generated_rs_str.push_str(&format!("
        embassy_hal_internal::peripherals_definition!(
            {gpio_output_code}
            {usart_output_code}
        );
        embassy_hal_internal::peripherals_struct!(
            {gpio_output_code}
            {usart_output_code}
        );
        embassy_hal_internal::interrupt_mod!(
            {interrupt_name_str}
        );
        {af_str}
    "));
    fs::write("./src/_generated.rs", generated_rs_str).unwrap();
}

fn enable_peripherial(lines: &Vec<&str>, name: &str) -> u32 {
    println!("cargo::rustc-check-cfg=cfg(vsf_{name}_enabled)");
    for peripheral_index in 0..32 {
        println!("cargo::rustc-check-cfg=cfg(vsf_{name}{peripheral_index}_enabled)");
    }

    let mask = extract_peripheral_mask(lines, name);
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

fn bind_vsf_gpios(lines: &Vec<&str>, output_code: &mut String) {
    let mask = extract_peripheral_mask(lines, "gpio");

    for port_index in 0..32 {
        if mask & (1 << port_index) != 0 {
            if let Some(mut pin_mask) = extract_const_integer::<u128>(&lines, &format!("VSF_HW_GPIO_PORT{port_index}_MASK")) {
                let mut pin_index = 0;
                while pin_mask != 0 {
                    if pin_mask & 1 != 0 {
                        output_code.push_str(&format!("P{port_index}_{pin_index},"));
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
    let mask = extract_peripheral_mask(lines, &peripheral_name_upper);

    for peripheral_index in 0..32 {
        if mask & (1 << peripheral_index) != 0 {
            output_code.push_str(&format!("{peripheral_name_upper}{peripheral_index},"));
        }
    }
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

fn extract_type(lines: &Vec<&str>, name: &str) -> bool {
    let mut matched = false;
    for line in lines {
        let parts: Vec<&str> = line.split_whitespace().collect();
        // pub type TYPE_NAME =
        if parts.len() >= 3 && parts[0] == "pub" && parts[1] == "type" {
            if parts[2] == name {
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
