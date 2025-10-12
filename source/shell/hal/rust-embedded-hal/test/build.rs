use std::env;

fn main() {
    let arch = env::var("CARGO_CFG_TARGET_ARCH").unwrap();
    let mut hostos = false;
    println!("cargo:warning=target_arch: {arch}");
    if let Ok(os) = env::var("CARGO_CFG_TARGET_OS") {
        println!("cargo:warning=target_os: {os}");
        hostos = true;
    }

    println!("cargo::rustc-check-cfg=cfg(use_defmt_rtt)");

    if !hostos {
        println!("cargo:rustc-link-arg-bins=--nmagic");
        println!("cargo:rustc-link-arg-bins=-Tlink.x");
        println!("cargo:rustc-link-arg-bins=-Tdefmt.x");

        println!("cargo:rustc-cfg=use_defmt_rtt");
    }
}
