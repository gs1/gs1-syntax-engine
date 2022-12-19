/*
 * GS1 Syntax Engine
 *
 * This is a contributed example that shows how a Rust language binding might
 * be developed.
 *
 *
 * @author Copyright (c) 2021-2022 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

use std::ptr;
use std::os::raw::{c_char,c_int};
use std::ffi::{CStr,CString};

extern "C" {
    pub fn gs1_encoder_getVersion() -> *const c_char;
    pub fn gs1_encoder_init(mem: *const u32) -> *const u32;
    pub fn gs1_encoder_free(ctx: *const u32);
    pub fn gs1_encoder_getIncludeDataTitlesInHRI(ctx: *const u32) -> bool;
    pub fn gs1_encoder_setIncludeDataTitlesInHRI(ctx: *const u32, value: bool);
    pub fn gs1_encoder_getDataStr(ctx: *const u32) -> *const c_char;
    pub fn gs1_encoder_setDataStr(ctx: *const u32, value: *const c_char);
    pub fn gs1_encoder_getAIdataStr(ctx: *const u32) -> *const c_char;
    pub fn gs1_encoder_getHRI(ctx: *const u32, hri: *const *const *const c_char) -> c_int;
}

struct GS1Encoder {
    ctx: *mut u32,
}

impl GS1Encoder {

    pub fn new() -> Self {
        let mut gs1encoder = GS1Encoder { ctx: ptr::null_mut() };
        gs1encoder.ctx = unsafe { gs1_encoder_init(ptr::null()) as *mut u32 };
        return gs1encoder;
    }

    pub fn free(&self) {
        unsafe { gs1_encoder_free(self.ctx) };
    }

    pub fn get_version(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr(gs1_encoder_getVersion()) };
        return c_str.to_str().unwrap().to_owned();
    }

    pub fn set_include_data_titles_in_hri(&self, value: bool) {
        unsafe { gs1_encoder_setIncludeDataTitlesInHRI(self.ctx, value) };
        // TODO error handling
    }

    pub fn get_include_data_titles_in_hri(&self) -> bool {
        return unsafe { gs1_encoder_getIncludeDataTitlesInHRI(self.ctx) };
    }

    pub fn get_data_str(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr( gs1_encoder_getDataStr(self.ctx) ) };
        return c_str.to_str().unwrap().to_owned();
    }

    pub fn set_data_str(&self, value: &str) {
        let c_str = CString::new(value).unwrap();
        unsafe { gs1_encoder_setDataStr(self.ctx, c_str.as_ptr() as *const c_char) };
        // TODO error handling
    }

    pub fn get_ai_data_str(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr( gs1_encoder_getAIdataStr(self.ctx) ) };
        return c_str.to_str().unwrap().to_owned();
        // TODO handle NULL case for non-AI data
    }

    pub fn get_hri(&self) -> Vec<String> {
        let ptr: *const *const c_char = ptr::null();
        let size = unsafe { gs1_encoder_getHRI(self.ctx, &ptr) };
        let mut hri = Vec::with_capacity(size as usize);
        for i in 0..size {
            let c_buf = unsafe { ptr::read(ptr.offset(i as isize)) };
            let c_str: &CStr = unsafe { CStr::from_ptr(c_buf) };
            let str_buf: String = c_str.to_str().unwrap().to_owned();
            hri.push(str_buf);
        }
        return hri;
    }

}

impl Drop for GS1Encoder {
    fn drop(&mut self) {
        self.free();
    }
}


fn main() {

    let data_str = "^010952123454321310ABC123^99TEST";
    let data_titles = true;

    let gs1encoder = GS1Encoder::new();

    println!("\nVersion: {}\n", gs1encoder.get_version());

    gs1encoder.set_data_str(data_str);

    println!("IN: {}", gs1encoder.get_data_str());
    println!("AI: {}", gs1encoder.get_ai_data_str());

    gs1encoder.set_include_data_titles_in_hri(data_titles);
    let hri = gs1encoder.get_hri();

    println!("\nHRI{}:", if gs1encoder.get_include_data_titles_in_hri() { " (including data titles)" } else { "" });
    for h in &hri {
        println!("    {}", h);
    }

}
