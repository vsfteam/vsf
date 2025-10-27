use std::env;

fn main() {
    let arch = env::var("CARGO_CFG_TARGET_ARCH").unwrap();
    let os = env::var("CARGO_CFG_TARGET_OS").unwrap();
    println!("cargo:warning=target_arch: {arch}");
    println!("cargo:warning=target_os: {os}");

    println!("cargo::rustc-check-cfg=cfg(target_hostos)");
    println!("cargo::rustc-check-cfg=cfg(target_mcu)");

    if os == "" || os == "none" {
        println!("cargo:rustc-link-arg-bins=--nmagic");
        println!("cargo:rustc-link-arg-bins=-Tlink.x");
        println!("cargo:rustc-link-arg-bins=-Tdefmt.x");
        println!("cargo:rustc-cfg=target_mcu");
    } else {
        println!("cargo:rustc-cfg=target_hostos");
    }

    println!("cargo::rustc-check-cfg=cfg(vsf_gpio_enabled)");
    println!("cargo::rustc-check-cfg=cfg(vsf_usart_enabled)");
    println!("cargo::rustc-check-cfg=cfg(vsf_spi_enabled)");
    println!("cargo::rustc-check-cfg=cfg(vsf_i2c_enabled)");
}
