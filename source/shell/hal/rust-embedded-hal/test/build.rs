use std::env;
use std::path::Path;
use std::fs;

fn main() {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let model = env::var("VSF_MODEL").expect("please define VSF_MODEL in config.toml [env] section");
    let model_memory_path = Path::new(&manifest_dir).join(format!("memory/{model}/memory.x"));
    fs::copy(model_memory_path, "./memory.x").expect(&format!("fail to find memory.x for {model}"));

    println!("cargo:rustc-link-arg-bins=--nmagic");
    println!("cargo:rustc-link-arg-bins=-Tlink.x");
    println!("cargo:rustc-link-arg-bins=-Tdefmt.x");
}
