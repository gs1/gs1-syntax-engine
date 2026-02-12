/*
 * GS1 Barcode Syntax Engine
 *
 * This is a contributed example that shows how a Rust language binding might
 * be developed.
 *
 *
 * @author Copyright (c) 2021-2026 GS1 AISBL.
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

use std::ffi::{CStr, CString};
use std::fmt;
use std::os::raw::{c_char, c_int};
use std::ptr;

extern "C" {
    fn gs1_encoder_getVersion() -> *const c_char;
    fn gs1_encoder_init(mem: *const u32) -> *const u32;
    fn gs1_encoder_free(ctx: *const u32);
    fn gs1_encoder_getErrMsg(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_getIncludeDataTitlesInHRI(ctx: *const u32) -> bool;
    fn gs1_encoder_setIncludeDataTitlesInHRI(ctx: *const u32, value: bool) -> bool;
    fn gs1_encoder_getDataStr(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_setDataStr(ctx: *const u32, value: *const c_char) -> bool;
    fn gs1_encoder_getAIdataStr(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_getHRI(ctx: *const u32, hri: *const *const *const c_char) -> c_int;
}

pub struct GS1Encoder {
    ctx: *mut u32,
}

impl GS1Encoder {
    fn get_err_msg(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr(gs1_encoder_getErrMsg(self.ctx)) };
        c_str.to_str().unwrap().to_owned()
    }

    pub fn new() -> Result<Self, GS1EncoderError> {
        let mut gs1encoder = GS1Encoder {
            ctx: ptr::null_mut(),
        };
        gs1encoder.ctx = unsafe { gs1_encoder_init(ptr::null()) as *mut u32 };
        if gs1encoder.ctx.is_null() {
            return Err(GS1EncoderError::GS1GeneralError(
                "Failed to initialise the native library".to_string(),
            ));
        }
        Ok(gs1encoder)
    }

    pub fn free(&mut self) {
        unsafe { gs1_encoder_free(self.ctx) };
        self.ctx = ptr::null_mut();
    }

    pub fn get_version(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr(gs1_encoder_getVersion()) };
        c_str.to_str().unwrap().to_owned()
    }

    pub fn set_include_data_titles_in_hri(&self, value: bool) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setIncludeDataTitlesInHRI(self.ctx, value) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_include_data_titles_in_hri(&self) -> bool {
        unsafe { gs1_encoder_getIncludeDataTitlesInHRI(self.ctx) }
    }

    pub fn get_data_str(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr(gs1_encoder_getDataStr(self.ctx)) };
        c_str.to_str().unwrap().to_owned()
    }

    pub fn set_data_str(&self, value: &str) -> Result<(), GS1EncoderError> {
        let c_str = CString::new(value).unwrap();
        let ret = unsafe { gs1_encoder_setDataStr(self.ctx, c_str.as_ptr() as *const c_char) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_ai_data_str(&self) -> Option<String> {
        let ptr = unsafe { gs1_encoder_getAIdataStr(self.ctx) };
        if ptr.is_null() {
            return None;
        }
        let c_str: &CStr = unsafe { CStr::from_ptr(ptr) };
        Some(c_str.to_str().unwrap().to_owned())
    }

    pub fn get_hri(&self) -> Vec<String> {
        let ptr: *const *const c_char = ptr::null();
        let size = unsafe { gs1_encoder_getHRI(self.ctx, &ptr) };
        let mut hri = Vec::with_capacity(size as usize);
        for i in 0..size {
            let c_buf = unsafe { ptr::read(ptr.offset(i as isize)) };
            let c_str: &CStr = unsafe { CStr::from_ptr(c_buf) };
            hri.push(c_str.to_str().unwrap().to_owned());
        }
        hri
    }
}

impl Drop for GS1Encoder {
    fn drop(&mut self) {
        self.free();
    }
}

#[derive(Debug)]
pub enum GS1EncoderError {
    GS1GeneralError(String),
    GS1ParameterError(String),
    GS1ScanDataError(String),
    GS1DigitalLinkError(String),
}

impl fmt::Display for GS1EncoderError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            GS1EncoderError::GS1GeneralError(msg) => write!(f, "{}", msg),
            GS1EncoderError::GS1ParameterError(msg) => write!(f, "{}", msg),
            GS1EncoderError::GS1ScanDataError(msg) => write!(f, "{}", msg),
            GS1EncoderError::GS1DigitalLinkError(msg) => write!(f, "{}", msg),
        }
    }
}
