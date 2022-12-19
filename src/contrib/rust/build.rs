fn main() {
    println!("cargo:rustc-link-lib=gs1encoders");
    println!("cargo:rustc-link-search=../../c-lib/build");
    println!("cargo:rustc-env=LD_LIBRARY_PATH=../../c-lib/build");
}
