use std::env;

fn main() {
    let arch = env::var("CARGO_CFG_TARGET_ARCH").unwrap();
    let os = env::var("CARGO_CFG_TARGET_OS").unwrap();
    println!("cargo:warning=target_arch: {arch}");
    println!("cargo:warning=target_os: {os}");

    println!("cargo::rustc-check-cfg=cfg(use_defmt_rtt)");

    if os == "" || os == "none" {
        println!("cargo:rustc-link-arg-bins=--nmagic");
        println!("cargo:rustc-link-arg-bins=-Tlink.x");
        println!("cargo:rustc-link-arg-bins=-Tdefmt.x");

        println!("cargo:rustc-cfg=use_defmt_rtt");
    }
}
