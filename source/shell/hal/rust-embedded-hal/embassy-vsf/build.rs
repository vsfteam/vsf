use std::path::Path;
use std::process::Command;
use std::sync::Mutex;
use std::collections::{HashMap, BTreeMap};
use std::env;
use std::fs;
use shellexpand;
use regex::Regex;

const BINDGEN_ENUM_VAR: bindgen::EnumVariation = bindgen::EnumVariation::Consts;
//const BINDGEN_ENUM_VAR: bindgen::EnumVariation = bindgen::EnumVariation::ModuleConsts;

const BINDGEN_DEFINITIONS: [&'static str; 2] = [
    "__VSF__",
    "__VSF_CPP__",
];

const PERIPHERIALS: [&'static str; 4] = ["gpio", "usart", "spi", "i2c"];

const CONSTANTS_MACRO: [(&'static str, &'static str); 66] = [
    // USART optional constants defined in MACRO
    ("vsf_usart_status_t", "VSF_USART_STATUS_BREAK_SENT"),
    ("vsf_usart_mode_t", "VSF_USART_5_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_6_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_7_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_9_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_10_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_0_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_1_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_2_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_SYNC_CLOCK_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_DISABLE"),
    ("vsf_usart_mode_t", "VSF_USART_SWAP"),
    ("vsf_usart_mode_t", "VSF_USART_TX_INVERT"),
    ("vsf_usart_mode_t", "VSF_USART_RX_INVERT"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_TIMEOUT"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_CTS"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_BREAK_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_IDLE"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_IDLE"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SEND_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SET_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_CLEAR_BREAK"),
    // SPI optional constants defined in MACRO
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_4"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_5"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_6"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_7"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_9"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_10"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_11"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_12"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_13"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_14"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_15"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_16"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_17"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_18"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_19"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_20"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_21"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_22"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_23"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_24"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_25"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_26"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_27"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_28"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_29"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_30"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_31"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_32"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_RX_ONLY"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_TX_ONLY"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_16_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_MOTOROLA_MODE"),
    ("vsf_spi_mode_t", "VSF_SPI_TI_MODE"),
    ("vsf_spi_mode_t", "VSF_SPI_CRC_ENABLED"),
    ("vsf_spi_mode_t", "VSF_SPI_CRC_DISABLED"),
    ("vsf_spi_ctrl_t", "VSF_SPI_CTRL_SET_DATASIZE"),
    ("vsf_spi_irq_mask_t", "VSF_SPI_IRQ_MASK_CRC_ERR"),
];

const CONSTANTS_ENUM: [(&'static str, &'static str); 122] = [
    // GPIO constants
    ("", "VSF_HW_GPIO_PIN_COUNT"),
    ("vsf_gpio_mode_t", "VSF_GPIO_ANALOG"),
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
    ("vsf_usart_mode_t", "VSF_USART_9_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_10_BIT_LENGTH"),
    ("vsf_usart_mode_t", "VSF_USART_0_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_1_5_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_2_STOPBIT"),
    ("vsf_usart_mode_t", "VSF_USART_SYNC_CLOCK_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_ENABLE"),
    ("vsf_usart_mode_t", "VSF_USART_HALF_DUPLEX_DISABLE"),
    ("vsf_usart_mode_t", "VSF_USART_SWAP"),
    ("vsf_usart_mode_t", "VSF_USART_TX_INVERT"),
    ("vsf_usart_mode_t", "VSF_USART_RX_INVERT"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_CPL"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_CPL"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_TIMEOUT"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_CTS"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_FRAME_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_PARITY_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_BREAK_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_OVERFLOW_ERR"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_RX_IDLE"),
    ("vsf_usart_irq_mask_t", "VSF_USART_IRQ_MASK_TX_IDLE"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SEND_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_SET_BREAK"),
    ("vsf_usart_ctrl_t", "VSF_USART_CTRL_CLEAR_BREAK"),
    // SPI constants
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_4"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_5"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_6"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_7"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_9"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_10"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_11"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_12"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_13"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_14"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_15"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_16"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_17"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_18"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_19"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_20"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_21"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_22"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_23"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_24"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_25"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_26"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_27"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_28"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_29"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_30"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_31"),
    ("vsf_spi_mode_t", "VSF_SPI_DATASIZE_32"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_FULL_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_RX_ONLY"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_TX_ONLY"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_1_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_2_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_4_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_8_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_DATALINE_16_LINE_HALF_DUPLEX"),
    ("vsf_spi_mode_t", "VSF_SPI_MOTOROLA_MODE"),
    ("vsf_spi_mode_t", "VSF_SPI_TI_MODE"),
    ("vsf_spi_mode_t", "VSF_SPI_CRC_ENABLED"),
    ("vsf_spi_mode_t", "VSF_SPI_CRC_DISABLED"),
    ("vsf_spi_ctrl_t", "VSF_SPI_CTRL_SET_DATASIZE"),
    ("vsf_spi_irq_mask_t", "VSF_SPI_IRQ_MASK_CRC_ERR"),
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
const TYPES: [&'static str; 5] = [
    "vsf_hw_peripheral_en_t",
    "vsf_usart_mode_t",
    "vsf_usart_irq_mask_t",
    "vsf_spi_mode_t",
    "vsf_spi_irq_mask_t",
];
const CTYPES: [(&'static str, &'static str); 2] = [
    ("::core::ffi::c_int", "i32"),
    ("::core::ffi::c_uint", "u32"),
];

const INTERRUPT_CFG: [(&'static str, &'static str); 2] = [
    (r"^I2C[0-9]+_EV$", "I2C_IRQ_EV"),
    (r"^I2C[0-9]+_ER$", "I2C_IRQ_ER"),
];

struct PeripheralAfInfo {
    module: &'static str,
    name: &'static str,
    pins: HashMap<&'static str, &'static str>,
}

struct ArchInfo {
    name: &'static str,
    detect: Box<dyn Fn() -> bool + Send>,
    features: Vec<&'static str>,
    bindgen_cflags: Vec<&'static str>,
    bindgen_model: Box<dyn Fn(&str) -> &str + Send>,
    cmake_arch: &'static str,
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
        if PERIPHERIALS.contains(&"usart") {
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
        }
        if PERIPHERIALS.contains(&"spi") {
            m.insert("SPI", PeripheralAfInfo {
                module: "spi",
                name: "Spi",
                pins: HashMap::from([
                    ("SCK", "SckPin"),
                    ("NSS", "CsPin"),
                    ("MISO", "MisoPin"),
                    ("MOSI", "MosiPin"),
                ]),
            });
        }
        if PERIPHERIALS.contains(&"i2c") {
            m.insert("I2C", PeripheralAfInfo {
                module: "i2c",
                name: "I2c",
                pins: HashMap::from([
                    ("SCL", "SclPin"),
                    ("SDA", "SdaPin"),
                    ("SMBA", "SmbaPin"),
                ]),
            });
        }
        m
    });
    static ref GLOBAL_CONSTANTS_MACRO_VEC: Mutex<HashMap<(&'static str, &'static str), Option<String>>> = Mutex::new({
        let v = HashMap::new();
        v
    });

    static ref GLOBAL_ARCH_INFO: Mutex<Vec<ArchInfo>> = Mutex::new(vec![
        ArchInfo {
            name: "CortexM",
            detect: Box::new(|| -> bool {
                env::var("TARGET").unwrap().starts_with("thumb")
            }),
            bindgen_model: Box::new(|cmake_model| {
                cmake_model
            }),
            features: Vec::from([]),
            bindgen_cflags: Vec::from([
                "-I${VSF_PATH}/source/utilities/compiler/arm/3rd-party/CMSIS/CMSIS/Core/Include",
            ]),
            cmake_arch: "ARM",
        },
        ArchInfo {
            name: "RiscV",
            detect: Box::new(|| -> bool {
                env::var("TARGET").unwrap().starts_with("riscv")
            }),
            bindgen_model: Box::new(|cmake_model| {
                cmake_model
            }),
            features: Vec::from([]),
            bindgen_cflags: Vec::from([]),
            cmake_arch: "Riscv",
        },
        ArchInfo {
            name: "Windows",
            detect: Box::new(|| -> bool {
                env::var("CARGO_CFG_WINDOWS").is_ok()
            }),
            bindgen_model: Box::new(|_cmake_model| {
                "EMULATION_DISTBUS"
            }),
            features: Vec::from([]),
            bindgen_cflags: Vec::from([
                "-D__WIN__",
                {
                    let target = env::var("TARGET").unwrap();
                    if target == "i686-pc-windows-msvc" {
                        "-D__CPU_X86__"
                    } else if target == "x86_64-pc-windows-msvc" {
                        "-D__CPU_X64__"
                    } else {
                        ""
                    }
                },
            ]),
            cmake_arch: "Windows",
        },
        ArchInfo {
            name: "Unix",          // for Linux, Unix, MacOS
            detect: Box::new(|| -> bool {
                env::var("CARGO_CFG_UNIX").is_ok()
            }),
            bindgen_model: Box::new(|_cmake_model| {
                "EMULATION_DISTBUS"
            }),
            features: Vec::from([]),
            bindgen_cflags: Vec::from([]),
            cmake_arch: "Linux",
        },
    ]);
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
            let mut constants_macro = GLOBAL_CONSTANTS_MACRO_VEC.lock().unwrap();
            for constant in CONSTANTS_MACRO {
                if constant.1 == name {
                    if tokens.len() == 1 {
                        constants_macro.insert((constant.0, constant.1), None);
                    } else {
                        constants_macro.insert((constant.0, constant.1),
                            Some(String::from_utf8_lossy(&tokens[1].raw).into_owned()));
                    }
                    break;
                }
            }
        }
    }
}

fn main() {
     if let Err(_error) = env::var("VSF_PATH") {
        let path_relative = "../../../../../";
        let path_absolute = env::current_dir().unwrap().join(path_relative);
        println!("VSF_PATH not defined: set to {path_absolute:?}");
        unsafe { env::set_var("VSF_PATH", path_absolute); }
    }

    let out_dir = env::var_os("OUT_DIR").unwrap();
    let out_path = Path::new(&out_dir);
    println!("cargo:rustc-link-search={}", out_path.to_str().unwrap());

    let mut path = env::var("VSF_PATH").unwrap();
    let vendor = env::var("VSF_VENDOR").expect("please define VSF_VENDOR in config.toml [env] section");
    let cmake_model = env::var("VSF_MODEL").expect("please define VSF_MODEL in config.toml [env] section");
    let mut bindgen_model: String = String::new();
    let bindgen_cflags_str = env::var("VSF_FLAGS").unwrap_or("".to_string());
    let mut bindgen_cflags: Vec<String> = bindgen_cflags_str.split(';').map(|s| s.to_string()).collect();
    bindgen_cflags.insert(0, format!("--target={}", env::var("TARGET").unwrap()));

    // arch
    let arch_infos = GLOBAL_ARCH_INFO.lock().unwrap();
    let mut cmake_arch: &'static str = "";
    let mut arch_name: &'static str = "";
    for arch in arch_infos.iter() {
        if (arch.detect)() {
            println!("cargo:warning=ARCH: {}", arch.name);
            for feature in arch.features.iter() {
                println!("{feature}");
            }
            for bindgen_cflag in arch.bindgen_cflags.iter() {
                if !bindgen_cflag.is_empty() {
                    bindgen_cflags.push(bindgen_cflag.to_string());
                }
            }
            cmake_arch = arch.cmake_arch;
            arch_name = arch.name;
            bindgen_model = String::from((arch.bindgen_model)(&cmake_model));
            break;
        }
    }

    println!("cargo:rerun-if-changed={}/vsf_hal.rs", out_path.to_str().unwrap());
    println!("cargo:warning=path: {path}");
    println!("cargo:warning=target: {vendor}.{bindgen_model}");
    println!("cargo:warning=flags:");
    for flag in &bindgen_cflags {
        if !flag.is_empty() {
            println!("cargo:warning=\t{flag}");
        }
    }

    path = shellexpand::full(&path).expect("Fail to expand for environment variables").into_owned();
    if path.starts_with(".") {
        path.insert(0, '/');
        path.insert_str(0, &env::var("CARGO_MANIFEST_DIR").unwrap());
    }
    if !path.ends_with("/") {
        path.push('/');
    }

    {
        let vsf_hal_build_path = out_path.join("vsf_hal_build");
        if !vsf_hal_build_path.exists() {
            fs::create_dir_all(&vsf_hal_build_path).unwrap();
            println!("cargo:warning=cmd: cmake -GNinja -DVSF_TARGET={cmake_model} {path}source/shell/hal/rust-embedded-hal/lib");
            Command::new("cmake").current_dir(&vsf_hal_build_path)
                        .arg("-GNinja")
                        .arg("-DVSF_TARGET=".to_string() + &cmake_model)
                        .arg("-DVSF_ARCH=".to_string() + &cmake_arch)
                        .arg(path.clone() + "source/shell/hal/rust-embedded-hal/lib")
                        .output().expect("Fail to run cmake");
            println!("cargo:warning=cmd: ninja");
            Command::new("ninja").current_dir(&vsf_hal_build_path).output().expect("Fail to run ninja");
        }

        println!("cargo:rustc-link-search=native={}", vsf_hal_build_path.to_str().unwrap());
        println!("cargo:rustc-link-lib=static=vsf_hal");
    }

    let vsf_c_path = out_path.join("vsf_c.h");
    fs::write(&vsf_c_path, "
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
                    .header(vsf_c_path.to_str().unwrap())
                    .parse_callbacks(Box::new(Callbacks{}))
                    .use_core()
                    .default_enum_style(BINDGEN_ENUM_VAR)
                    .clang_arg("-D".to_string() + "__" + &vendor + "__")
                    .clang_arg("-D".to_string() + "__" + &bindgen_model + "__")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/lib/inc")
                    .clang_arg("-I".to_string() + &path + "source/shell/hal/rust-embedded-hal/lib/inc/empty_libc")
                    .clang_arg("-I".to_string() + &path + "source");

    for definition in BINDGEN_DEFINITIONS {
        builder = builder.clang_arg("-D".to_string() + definition);
    }
    for flag in &bindgen_cflags {
        if !flag.is_empty() {
            builder = builder.clang_arg(shellexpand::full(&flag).unwrap().into_owned());
        }
    }

    let bindings = builder.generate().expect("Failed to generate rust bindings");

    let pathbuf = out_path.join("vsf_hal.rs");
    bindings.write_to_file(&pathbuf).unwrap();

    let bindings_content = fs::read_to_string(&pathbuf).unwrap();
    let bindings_lines = bindings_content.lines().collect();
    let constants_macro = GLOBAL_CONSTANTS_MACRO_VEC.lock().unwrap();

    // parse constants
    let mut constants_cfg: BTreeMap<(&'static str, &'static str), u64> = BTreeMap::new();
    // add constants in CONSTANTS_ENUM
    'outer: for constant_enum in CONSTANTS_ENUM {
        println!("cargo::rustc-check-cfg=cfg({})", constant_enum.1);
        if let Some(cur_value) = {
            if BINDGEN_ENUM_VAR == bindgen::EnumVariation::Consts {
                if constant_enum.0 != "" {
                    extract_const_integer::<u64>(&bindings_lines, &(String::from(constant_enum.0) + "_" + constant_enum.1))
                } else {
                    extract_const_integer::<u64>(&bindings_lines, &constant_enum.1)
                }
            } else if BINDGEN_ENUM_VAR == bindgen::EnumVariation::ModuleConsts {
                extract_const_integer::<u64>(&bindings_lines, &constant_enum.1)
            } else {
                panic!("should not run till here");
            }
        } {
            for constant_macro in CONSTANTS_MACRO {
                if constant_macro.1 == constant_enum.1 && !constants_macro.contains_key(&constant_macro) {
                    continue 'outer;
                }
            }
            constants_cfg.insert((constant_enum.0, constant_enum.1), cur_value);
        }
    }
    // add constants in CONSTANTS_MACRO
    for constant_macro in CONSTANTS_MACRO {
        if !CONSTANTS_ENUM.contains(&constant_macro) {
            println!("cargo::rustc-check-cfg=cfg({})", constant_macro.1);
            if constants_macro.contains_key(&constant_macro) {
                constants_cfg.insert((constant_macro.0, constant_macro.1), 0);
            }
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
        let mut type_value = String::from("");
        println!("cargo::rustc-check-cfg=cfg({_type})");
        if BINDGEN_ENUM_VAR == bindgen::EnumVariation::Consts {
            type_value = extract_type(&bindings_lines, _type);
        } else if BINDGEN_ENUM_VAR == bindgen::EnumVariation::ModuleConsts {
            type_value = extract_enum_module_type(&bindings_lines, _type);
        }

        let mut type_value_short: &'static str = "";
        for ctype in CTYPES {
            println!("cargo::rustc-check-cfg=cfg({_type}_is_{})", ctype.1);
        }
        for ctype in CTYPES {
            if type_value == ctype.0 {
                type_value_short = ctype.1;
                break;
            }
        }
        if type_value_short != "" {
            println!("cargo:warning=type: {_type} enabled as type {type_value_short}");
            println!("cargo:rustc-cfg={_type}");
            println!("cargo:rustc-cfg={_type}_is_{type_value_short}");
        }
    }

    // parse interrupts and generate device.x and pac.rs
    for interrupt in INTERRUPT_CFG {
        println!("cargo::rustc-check-cfg=cfg({})", interrupt.1);
    }

    let mut device_x_str = String::from("");
    let mut interrupt_str = String::from("");
    let mut interrupt_func_dec_str = String::from("");
    let mut interrupt_vecotr_str = String::from("");
    let mut interrupt_name_str = String::from("");
    let mut interrupt_num: u8 = 0;
    let mut interrupt_cfg_map: Vec<&'static str> = Vec::new();
    if let Some(interrupt_num_tmp) = extract_const_integer::<u8>(&bindings_lines, "VSF_HW_INTERRUPTS_NUM") {
        interrupt_num = interrupt_num_tmp;
        let interrupt_vec = GLOBAL_INTERRUPT_VEC.lock().unwrap();
        for interrupt_index in 0..interrupt_vec.len() {
            if interrupt_vec[interrupt_index].len() > 0 {
                let interrupt_name = interrupt_vec[interrupt_index].strip_suffix("_IRQHandler").unwrap();

                for interrupt in INTERRUPT_CFG {
                    if !interrupt_cfg_map.contains(&interrupt.1) && Regex::new(interrupt.0).unwrap().is_match(interrupt_name) {
                        interrupt_cfg_map.push(&interrupt.1);
                        println!("cargo::rustc-cfg={}", interrupt.1);
                    }
                }

                println!("cargo:warning=irq{interrupt_index}: {interrupt_name}");
                interrupt_name_str.push_str(&format!("{interrupt_name},\n"));
                device_x_str.push_str(&format!("PROVIDE({interrupt_name} = DefaultHandler);\n"));
                interrupt_str.push_str(&format!("{interrupt_name} = {interrupt_index},\n"));
                interrupt_func_dec_str.push_str(&format!("fn {interrupt_name}();\n"));
                interrupt_vecotr_str.push_str(&format!("Vector {{ _handler: {interrupt_name} }},\n"));
            } else {
                interrupt_vecotr_str.push_str("Vector { _reserved: 0 },\n");
            }
        }
        for _interrupt_index in interrupt_vec.len()..(interrupt_num as usize) {
            interrupt_vecotr_str.push_str("Vector { _reserved: 0 },\n");
        }
    }

    // generate device.x
    fs::write(out_path.join("device.x"), device_x_str).unwrap();
    fs::write(out_path.join("pac.rs"), String::from(&format!("
        #[derive(Copy, Clone, Debug, PartialEq, Eq)]
        #[cfg_attr(feature = \"defmt\", derive(defmt::Format))]
        #[allow(non_camel_case_types)]
        pub enum Interrupt {{
            {interrupt_str}
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
    "))).unwrap();

    // generate memory.x
    let mut flash_addr = 0u64;
    let mut flash_size = 0u32;
    let mut ram_addr = 0u64;
    let mut ram_size = 0u32;
    let flash_mask = extract_peripheral_mask(&bindings_lines, "flash");
    if flash_mask != 0 {
        for flash_index in 0..32 {
            if  flash_mask & (1 << flash_index) != 0
            &&  let Some(flash_addr_tmp) = extract_const_integer::<u64>(&bindings_lines, &format!("VSF_HW_FLASH{flash_index}_ADDR"))
            &&  let Some(flash_size_tmp) = extract_const_integer::<u32>(&bindings_lines, &format!("VSF_HW_FLASH{flash_index}_SIZE")) {
                flash_addr = flash_addr_tmp;
                flash_size = flash_size_tmp;
            }
        }
    }
    let ram_mask = extract_peripheral_mask(&bindings_lines, "ram");
    if ram_mask != 0 {
        for ram_index in 0..32 {
            if  ram_mask & (1 << ram_index) != 0
            &&  let Some(ram_addr_tmp) = extract_const_integer::<u64>(&bindings_lines, &format!("VSF_HW_RAM{ram_index}_ADDR"))
            &&  let Some(ram_size_tmp) = extract_const_integer::<u32>(&bindings_lines, &format!("VSF_HW_RAM{ram_index}_SIZE")) {
                ram_addr = ram_addr_tmp;
                ram_size = ram_size_tmp;
            }
        }
    }

    fs::write(out_path.join("memory.x"), String::from(&format!("
        MEMORY
        {{
            FLASH (rx) : ORIGIN = 0x{flash_addr:X}, LENGTH = 0x{flash_size:X}
            RAM (xrw)  : ORIGIN = 0x{ram_addr:X}, LENGTH = 0x{ram_size:X}
        }}
    "))).unwrap();

    // bind vsf peripherals
    let mut generated_rs_str = String::from("");
    let mut peripheral_list = String::from("");
    if PERIPHERIALS.contains(&"gpio") {
        bind_vsf_gpios(&bindings_lines, &mut peripheral_list);
    }
    let mut usarts_sync_support: [bool; 32] = [false; 32];
    if PERIPHERIALS.contains(&"usart") {
        bind_vsf_usarts(&bindings_lines, &mut peripheral_list, &mut usarts_sync_support);
    }
    if PERIPHERIALS.contains(&"spi") {
        bind_vsf_peripheral(&bindings_lines, "spi", &mut peripheral_list);
    }
    if PERIPHERIALS.contains(&"i2c") {
        bind_vsf_peripheral(&bindings_lines, "i2c", &mut peripheral_list);
    }

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
        let mut peripheral_type_str = peripheral_str.trim_end_matches(|c: char| c.is_ascii_digit());
        let peripheral_index_str = &peripheral_str[peripheral_type_str.len()..];
        let peripheral_index: usize = peripheral_index_str.parse::<usize>().unwrap();
        let af = af.1;

        if peripheral_index >= 32 {
            println!("cargo:error={peripheral_type_str}{peripheral_index_str}: not supported because peripheral index >= 32");
            continue;
        }

        // patch peripheral_str, eg USART without sync support => UART
        if peripheral_type_str == "USART" && !usarts_sync_support.get(peripheral_index).unwrap() {
            peripheral_type_str = "UART";
        }

        if let Some(af_info) = afs_info.get(peripheral_type_str) {
            if let Some(af_pin) = af_info.pins.get(pin_str) {
                println!("cargo:warning={}::{peripheral_type_str}{peripheral_index_str}: {pin_str} on {gpio_str} ==> AF{af}", af_info.module);
                af_str.push_str(&format!("pin_trait_impl!(crate::{}::{}, {peripheral_type_str}{peripheral_index_str}, {gpio_str}, {af}u16);\n", af_info.module, *af_pin));
            }
        }
    }

    generated_rs_str.push_str(&format!("
        embassy_hal_internal::peripherals_definition!(
            {peripheral_list}
        );
        embassy_hal_internal::peripherals_struct!(
            {peripheral_list}
        );
        interrupt_mod!({arch_name},
            {interrupt_name_str}
        );
        {af_str}
    "));
    fs::write(out_path.join("_generated.rs"), generated_rs_str).unwrap();
}

fn enable_peripherial(name: &str, mask: u32) {
    println!("cargo::rustc-check-cfg=cfg(vsf_{name}_enabled)");
    for peripheral_index in 0..32 {
        println!("cargo::rustc-check-cfg=cfg(vsf_{name}{peripheral_index}_enabled)");
    }

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
}

fn bind_vsf_gpios(lines: &Vec<&str>, output_code: &mut String) {
    let mask = extract_peripheral_mask(lines, "gpio");
    let pin_count = extract_const_integer::<u32>(&lines, "VSF_HW_GPIO_PIN_COUNT").unwrap_or(0);

    enable_peripherial("gpio", mask);

    for port_index in 0..32 {
        if mask & (1 << port_index) != 0 {
            let mut port_pin_mask = (1 << pin_count) - 1;
            if let Some(pin_mask) = extract_const_integer::<u128>(&lines, &format!("VSF_HW_GPIO_PORT{port_index}_MASK")) {
                port_pin_mask = pin_mask;
            }

            let mut pin_index = 0;
            while port_pin_mask != 0 {
                if port_pin_mask & 1 != 0 {
                    output_code.push_str(&format!("P{port_index}_{pin_index},"));
                }
                pin_index += 1;
                port_pin_mask >>= 1;
            }
        }
    }
}

fn bind_vsf_usarts(lines: &Vec<&str>, output_code: &mut String, support_sync: &mut [bool; 32]) {
    let mask: u32 = extract_peripheral_mask(lines, "usart");

    enable_peripherial("usart", mask);

    for usart_index in 0..32 {
        if mask & (1 << usart_index) != 0 {
            if let Some(sync_support) = extract_const_integer::<u8>(lines, &format!("VSF_HW_USART{usart_index}_SYNC")) {
                if sync_support != 0 {
                    support_sync[usart_index] = true;
                    output_code.push_str(&format!("USART{usart_index},"));
                } else {
                    support_sync[usart_index] = false;
                    output_code.push_str(&format!("UART{usart_index},"));
                }
            } else {
                support_sync[usart_index] = true;
                output_code.push_str(&format!("USART{usart_index},"));
            }
        }
    }
}

fn bind_vsf_peripheral(lines: &Vec<&str>, name: &str, output_code: &mut String) {
    let peripheral_name_upper = String::from(name).to_uppercase();
    let mask = extract_peripheral_mask(lines, &peripheral_name_upper);

    enable_peripherial(name, mask);

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

fn extract_type(lines: &Vec<&str>, name: &str) -> String {
    for line in lines {
        let parts: Vec<&str> = line.split_whitespace().collect();
        // pub type TYPE_NAME =
        if parts.len() == 5 && parts[0] == "pub" && parts[1] == "type" && parts[3] == "=" {
            if parts[2] == name {
                return String::from(parts[4].strip_suffix(";").unwrap());
            }
        }
    }
    "".to_string()
}

fn extract_enum_module_type(lines: &Vec<&str>, name: &str) -> String {
    let mut matched = false;
    for line in lines {
        let parts: Vec<&str> = line.split_whitespace().collect();
        let parts_len = parts.len();
        // pub mod MODULE_NAME {
        //   pub type Type = ......;
        // }
        if !matched {
            if parts_len == 4 && parts[0] == "pub" && parts[1] == "mod" && parts[3] == "{" {
                if parts[2] == name {
                    matched = true;
                }
            }
        } else {
            if parts_len == 1 && parts[0] == "}" {
                matched = false;
            } else if parts_len == 5 && parts[0] == "pub" && parts[1] == "type" && parts[2] == "Type" && parts[3] == "=" {
                return String::from(parts[4].strip_suffix(";").unwrap());
            }
        }
    }
    "".to_string()
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
