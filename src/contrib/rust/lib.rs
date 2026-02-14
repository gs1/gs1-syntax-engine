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

#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Symbology {
    None = -1,
    DataBarOmni = 0,
    DataBarTruncated,
    DataBarStacked,
    DataBarStackedOmni,
    DataBarLimited,
    DataBarExpanded,
    UpcA,
    UpcE,
    Ean13,
    Ean8,
    Gs1128Cca,
    Gs1128Ccc,
    Qr,
    Dm,
    DotCode,
}

#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Validation {
    MutexAis = 0,
    RequisiteAis,
    RepeatedAis,
    DigSigSerialKey,
    UnknownAiNotDlAttr,
}

extern "C" {
    fn gs1_encoder_getVersion() -> *const c_char;
    fn gs1_encoder_init(mem: *const u32) -> *const u32;
    fn gs1_encoder_free(ctx: *const u32);
    fn gs1_encoder_getErrMsg(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_getErrMarkup(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_getSym(ctx: *const u32) -> c_int;
    fn gs1_encoder_setSym(ctx: *const u32, sym: c_int) -> bool;
    fn gs1_encoder_getAddCheckDigit(ctx: *const u32) -> bool;
    fn gs1_encoder_setAddCheckDigit(ctx: *const u32, value: bool) -> bool;
    fn gs1_encoder_getPermitUnknownAIs(ctx: *const u32) -> bool;
    fn gs1_encoder_setPermitUnknownAIs(ctx: *const u32, value: bool) -> bool;
    fn gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx: *const u32) -> bool;
    fn gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx: *const u32, value: bool) -> bool;
    fn gs1_encoder_getValidationEnabled(ctx: *const u32, validation: c_int) -> bool;
    fn gs1_encoder_setValidationEnabled(ctx: *const u32, validation: c_int, enabled: bool) -> bool;
    fn gs1_encoder_getIncludeDataTitlesInHRI(ctx: *const u32) -> bool;
    fn gs1_encoder_setIncludeDataTitlesInHRI(ctx: *const u32, value: bool) -> bool;
    fn gs1_encoder_getDataStr(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_setDataStr(ctx: *const u32, value: *const c_char) -> bool;
    fn gs1_encoder_getAIdataStr(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_setAIdataStr(ctx: *const u32, value: *const c_char) -> bool;
    fn gs1_encoder_getScanData(ctx: *const u32) -> *const c_char;
    fn gs1_encoder_setScanData(ctx: *const u32, value: *const c_char) -> bool;
    fn gs1_encoder_getDLuri(ctx: *const u32, stem: *const c_char) -> *const c_char;
    fn gs1_encoder_getDLignoredQueryParams(
        ctx: *const u32,
        qp: *const *const *const c_char,
    ) -> c_int;
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
        if !self.ctx.is_null() {
            unsafe { gs1_encoder_free(self.ctx) };
            self.ctx = ptr::null_mut();
        }
    }

    pub fn get_version(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr(gs1_encoder_getVersion()) };
        c_str.to_str().unwrap().to_owned()
    }

    pub fn get_err_markup(&self) -> String {
        let c_str: &CStr = unsafe { CStr::from_ptr(gs1_encoder_getErrMarkup(self.ctx)) };
        c_str.to_str().unwrap().to_owned()
    }

    pub fn get_sym(&self) -> Symbology {
        let raw = unsafe { gs1_encoder_getSym(self.ctx) };
        unsafe { std::mem::transmute(raw) }
    }

    pub fn set_sym(&self, sym: Symbology) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setSym(self.ctx, sym as c_int) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_add_check_digit(&self) -> bool {
        unsafe { gs1_encoder_getAddCheckDigit(self.ctx) }
    }

    pub fn set_add_check_digit(&self, value: bool) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setAddCheckDigit(self.ctx, value) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_permit_unknown_ais(&self) -> bool {
        unsafe { gs1_encoder_getPermitUnknownAIs(self.ctx) }
    }

    pub fn set_permit_unknown_ais(&self, value: bool) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setPermitUnknownAIs(self.ctx, value) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_permit_zero_suppressed_gtin_in_dl_uris(&self) -> bool {
        unsafe { gs1_encoder_getPermitZeroSuppressedGTINinDLuris(self.ctx) }
    }

    pub fn set_permit_zero_suppressed_gtin_in_dl_uris(
        &self,
        value: bool,
    ) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setPermitZeroSuppressedGTINinDLuris(self.ctx, value) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_validation_enabled(&self, validation: Validation) -> bool {
        unsafe { gs1_encoder_getValidationEnabled(self.ctx, validation as c_int) }
    }

    pub fn set_validation_enabled(
        &self,
        validation: Validation,
        enabled: bool,
    ) -> Result<(), GS1EncoderError> {
        let ret =
            unsafe { gs1_encoder_setValidationEnabled(self.ctx, validation as c_int, enabled) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    #[deprecated(note = "Use get_validation_enabled(Validation::RequisiteAis) instead")]
    pub fn get_validate_ai_associations(&self) -> bool {
        self.get_validation_enabled(Validation::RequisiteAis)
    }

    #[deprecated(note = "Use set_validation_enabled(Validation::RequisiteAis, value) instead")]
    pub fn set_validate_ai_associations(&self, value: bool) -> Result<(), GS1EncoderError> {
        self.set_validation_enabled(Validation::RequisiteAis, value)
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

    pub fn set_ai_data_str(&self, value: &str) -> Result<(), GS1EncoderError> {
        let c_str = CString::new(value).unwrap();
        let ret = unsafe { gs1_encoder_setAIdataStr(self.ctx, c_str.as_ptr() as *const c_char) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_scan_data(&self) -> Option<String> {
        let ptr = unsafe { gs1_encoder_getScanData(self.ctx) };
        if ptr.is_null() {
            return None;
        }
        let c_str: &CStr = unsafe { CStr::from_ptr(ptr) };
        Some(c_str.to_str().unwrap().to_owned())
    }

    pub fn set_scan_data(&self, value: &str) -> Result<(), GS1EncoderError> {
        let c_str = CString::new(value).unwrap();
        let ret = unsafe { gs1_encoder_setScanData(self.ctx, c_str.as_ptr() as *const c_char) };
        if !ret {
            return Err(GS1EncoderError::GS1ScanDataError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_dl_uri(&self, stem: Option<&str>) -> Option<String> {
        let c_stem = stem.map(|s| CString::new(s).unwrap());
        let stem_ptr = c_stem
            .as_ref()
            .map_or(ptr::null(), |s| s.as_ptr() as *const c_char);
        let ptr = unsafe { gs1_encoder_getDLuri(self.ctx, stem_ptr) };
        if ptr.is_null() {
            return None;
        }
        let c_str: &CStr = unsafe { CStr::from_ptr(ptr) };
        Some(c_str.to_str().unwrap().to_owned())
    }

    pub fn get_dl_ignored_query_params(&self) -> Vec<String> {
        let ptr: *const *const c_char = ptr::null();
        let size = unsafe { gs1_encoder_getDLignoredQueryParams(self.ctx, &ptr) };
        let mut params = Vec::with_capacity(size as usize);
        for i in 0..size {
            let c_buf = unsafe { ptr::read(ptr.offset(i as isize)) };
            let c_str: &CStr = unsafe { CStr::from_ptr(c_buf) };
            params.push(c_str.to_str().unwrap().to_owned());
        }
        params
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
