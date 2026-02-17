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

    pub fn set_sym(&mut self, sym: Symbology) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setSym(self.ctx, sym as c_int) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_add_check_digit(&self) -> bool {
        unsafe { gs1_encoder_getAddCheckDigit(self.ctx) }
    }

    pub fn set_add_check_digit(&mut self, value: bool) -> Result<(), GS1EncoderError> {
        let ret = unsafe { gs1_encoder_setAddCheckDigit(self.ctx, value) };
        if !ret {
            return Err(GS1EncoderError::GS1ParameterError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_permit_unknown_ais(&self) -> bool {
        unsafe { gs1_encoder_getPermitUnknownAIs(self.ctx) }
    }

    pub fn set_permit_unknown_ais(&mut self, value: bool) -> Result<(), GS1EncoderError> {
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
        &mut self,
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
        &mut self,
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
    pub fn set_validate_ai_associations(&mut self, value: bool) -> Result<(), GS1EncoderError> {
        self.set_validation_enabled(Validation::RequisiteAis, value)
    }

    pub fn set_include_data_titles_in_hri(&mut self, value: bool) -> Result<(), GS1EncoderError> {
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

    pub fn set_data_str(&mut self, value: &str) -> Result<(), GS1EncoderError> {
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

    pub fn set_ai_data_str(&mut self, value: &str) -> Result<(), GS1EncoderError> {
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

    pub fn set_scan_data(&mut self, value: &str) -> Result<(), GS1EncoderError> {
        let c_str = CString::new(value).unwrap();
        let ret = unsafe { gs1_encoder_setScanData(self.ctx, c_str.as_ptr() as *const c_char) };
        if !ret {
            return Err(GS1EncoderError::GS1ScanDataError(self.get_err_msg()));
        }
        Ok(())
    }

    pub fn get_dl_uri(&self, stem: Option<&str>) -> Result<String, GS1EncoderError> {
        let c_stem = stem.map(|s| CString::new(s).unwrap());
        let stem_ptr = c_stem
            .as_ref()
            .map_or(ptr::null(), |s| s.as_ptr() as *const c_char);
        let ptr = unsafe { gs1_encoder_getDLuri(self.ctx, stem_ptr) };
        if ptr.is_null() {
            return Err(GS1EncoderError::GS1DigitalLinkError(self.get_err_msg()));
        }
        let c_str: &CStr = unsafe { CStr::from_ptr(ptr) };
        Ok(c_str.to_str().unwrap().to_owned())
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

impl std::error::Error for GS1EncoderError {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_set_dl_uri() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        gs1encoder
            .set_data_str("https://id.example.org/test/01/12312312312319?99=TESTING123")
            .unwrap();

        assert_eq!(
            gs1encoder.get_data_str(),
            "https://id.example.org/test/01/12312312312319?99=TESTING123"
        );
        assert_eq!(
            gs1encoder.get_dl_uri(None).unwrap(),
            "https://id.gs1.org/01/12312312312319?99=TESTING123"
        );
        assert_eq!(
            gs1encoder.get_ai_data_str().unwrap(),
            "(01)12312312312319(99)TESTING123"
        );

        gs1encoder.set_include_data_titles_in_hri(true).unwrap();
        assert_eq!(
            gs1encoder.get_hri(),
            vec!["GTIN (01) 12312312312319", "INTERNAL (99) TESTING123"]
        );

        gs1encoder.set_sym(Symbology::Dm).unwrap();
        assert_eq!(gs1encoder.get_sym(), Symbology::Dm);
        assert_eq!(
            gs1encoder.get_scan_data().unwrap(),
            "]d1https://id.example.org/test/01/12312312312319?99=TESTING123"
        );
    }

    #[test]
    fn test_set_ai_data_str() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        gs1encoder
            .set_ai_data_str("(01)12312312312319(99)TESTING123")
            .unwrap();

        assert_eq!(gs1encoder.get_data_str(), "^011231231231231999TESTING123");
        assert_eq!(
            gs1encoder.get_dl_uri(None).unwrap(),
            "https://id.gs1.org/01/12312312312319?99=TESTING123"
        );
        assert_eq!(
            gs1encoder.get_ai_data_str().unwrap(),
            "(01)12312312312319(99)TESTING123"
        );
        assert_eq!(
            gs1encoder.get_hri(),
            vec!["(01) 12312312312319", "(99) TESTING123"]
        );

        gs1encoder.set_sym(Symbology::Qr).unwrap();
        assert_eq!(gs1encoder.get_sym(), Symbology::Qr);
        assert_eq!(
            gs1encoder.get_scan_data().unwrap(),
            "]Q3011231231231231999TESTING123"
        );
    }

    #[test]
    fn test_requisites() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        assert!(gs1encoder.get_validation_enabled(Validation::RequisiteAis));

        let err = gs1encoder.set_data_str("^0212312312312319").unwrap_err();
        assert!(matches!(err, GS1EncoderError::GS1ParameterError(_)));
        assert!(
            err.to_string().contains("not satisfied"),
            "Expected 'not satisfied' in error: {err}"
        );

        gs1encoder
            .set_validation_enabled(Validation::RequisiteAis, false)
            .unwrap();
        assert!(!gs1encoder.get_validation_enabled(Validation::RequisiteAis));
        gs1encoder.set_data_str("^0212312312312319").unwrap();

        assert_eq!(gs1encoder.get_data_str(), "^0212312312312319");
        let err = gs1encoder.get_dl_uri(None).unwrap_err();
        assert!(matches!(err, GS1EncoderError::GS1DigitalLinkError(_)));
        assert!(
            err.to_string().contains("without a primary key"),
            "Expected 'without a primary key' in error: {err}"
        );
        assert_eq!(gs1encoder.get_ai_data_str().unwrap(), "(02)12312312312319");
        assert_eq!(gs1encoder.get_hri(), vec!["(02) 12312312312319"]);

        gs1encoder.set_sym(Symbology::DataBarExpanded).unwrap();
        assert_eq!(gs1encoder.get_sym(), Symbology::DataBarExpanded);
        assert_eq!(gs1encoder.get_scan_data().unwrap(), "]e00212312312312319");
    }

    #[test]
    fn test_version() {
        let gs1encoder = GS1Encoder::new().unwrap();

        let version = gs1encoder.get_version();
        assert!(!version.is_empty());
    }

    #[test]
    fn test_defaults() {
        let gs1encoder = GS1Encoder::new().unwrap();

        assert_eq!(gs1encoder.get_sym(), Symbology::None);
        assert_eq!(gs1encoder.get_data_str(), "");
        assert!(!gs1encoder.get_add_check_digit());
        assert!(!gs1encoder.get_permit_unknown_ais());
        assert!(!gs1encoder.get_permit_zero_suppressed_gtin_in_dl_uris());
        assert!(!gs1encoder.get_include_data_titles_in_hri());
        assert!(gs1encoder.get_ai_data_str().is_none());
        assert!(gs1encoder.get_scan_data().is_none());
        assert!(gs1encoder.get_hri().is_empty());
        assert!(gs1encoder.get_dl_ignored_query_params().is_empty());
        assert_eq!(gs1encoder.get_err_markup(), "");
    }

    #[test]
    fn test_boolean_setters() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        assert!(!gs1encoder.get_add_check_digit());
        gs1encoder.set_add_check_digit(true).unwrap();
        assert!(gs1encoder.get_add_check_digit());
        gs1encoder.set_add_check_digit(false).unwrap();
        assert!(!gs1encoder.get_add_check_digit());

        assert!(!gs1encoder.get_permit_unknown_ais());
        gs1encoder.set_permit_unknown_ais(true).unwrap();
        assert!(gs1encoder.get_permit_unknown_ais());
        gs1encoder.set_permit_unknown_ais(false).unwrap();
        assert!(!gs1encoder.get_permit_unknown_ais());

        assert!(!gs1encoder.get_permit_zero_suppressed_gtin_in_dl_uris());
        gs1encoder
            .set_permit_zero_suppressed_gtin_in_dl_uris(true)
            .unwrap();
        assert!(gs1encoder.get_permit_zero_suppressed_gtin_in_dl_uris());
        gs1encoder
            .set_permit_zero_suppressed_gtin_in_dl_uris(false)
            .unwrap();
        assert!(!gs1encoder.get_permit_zero_suppressed_gtin_in_dl_uris());

        assert!(!gs1encoder.get_include_data_titles_in_hri());
        gs1encoder.set_include_data_titles_in_hri(true).unwrap();
        assert!(gs1encoder.get_include_data_titles_in_hri());
        gs1encoder.set_include_data_titles_in_hri(false).unwrap();
        assert!(!gs1encoder.get_include_data_titles_in_hri());
    }

    #[test]
    fn test_validations() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        assert!(gs1encoder.get_validation_enabled(Validation::MutexAis));
        assert!(gs1encoder.get_validation_enabled(Validation::RequisiteAis));
        assert!(gs1encoder.get_validation_enabled(Validation::RepeatedAis));
        assert!(gs1encoder.get_validation_enabled(Validation::DigSigSerialKey));
        assert!(gs1encoder.get_validation_enabled(Validation::UnknownAiNotDlAttr));

        gs1encoder
            .set_validation_enabled(Validation::RequisiteAis, false)
            .unwrap();
        assert!(!gs1encoder.get_validation_enabled(Validation::RequisiteAis));
        gs1encoder
            .set_validation_enabled(Validation::RequisiteAis, true)
            .unwrap();
        assert!(gs1encoder.get_validation_enabled(Validation::RequisiteAis));

        let err = gs1encoder
            .set_validation_enabled(Validation::RepeatedAis, false)
            .unwrap_err();
        assert!(matches!(err, GS1EncoderError::GS1ParameterError(_)));
    }

    #[test]
    fn test_set_scan_data() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        gs1encoder
            .set_scan_data("]e0011231231231233310ABC123\x1D99XYZ")
            .unwrap();
        assert_eq!(gs1encoder.get_sym(), Symbology::DataBarExpanded);
        assert_eq!(gs1encoder.get_data_str(), "^011231231231233310ABC123^99XYZ");
        assert_eq!(
            gs1encoder.get_ai_data_str().unwrap(),
            "(01)12312312312333(10)ABC123(99)XYZ"
        );
    }

    #[test]
    fn test_non_ai_data() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        gs1encoder.set_data_str("TESTING").unwrap();
        assert!(gs1encoder.get_ai_data_str().is_none());
        assert!(gs1encoder.get_scan_data().is_none());
        assert!(gs1encoder.get_hri().is_empty());
        assert!(gs1encoder.get_dl_ignored_query_params().is_empty());
        let err = gs1encoder.get_dl_uri(None).unwrap_err();
        assert!(matches!(err, GS1EncoderError::GS1DigitalLinkError(_)));
        assert!(
            err.to_string().contains("without a primary key"),
            "Expected 'without a primary key' in error: {err}"
        );
    }

    #[test]
    fn test_get_dl_uri_with_stem() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        gs1encoder.set_ai_data_str("(01)12312312312319").unwrap();
        let custom_uri = gs1encoder.get_dl_uri(Some("https://example.com")).unwrap();
        assert!(
            custom_uri.starts_with("https://example.com/"),
            "Expected custom stem in URI: {custom_uri}"
        );
        let default_uri = gs1encoder.get_dl_uri(None).unwrap();
        assert!(
            default_uri.starts_with("https://id.gs1.org/"),
            "Expected default stem in URI: {default_uri}"
        );
    }

    #[test]
    fn test_dl_ignored_query_params() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        gs1encoder
            .set_data_str("https://a/01/12312312312333/22/TESTING?singleton&99=ABC&compound=XYZ")
            .unwrap();
        assert_eq!(
            gs1encoder.get_dl_ignored_query_params(),
            vec!["singleton", "compound=XYZ"]
        );
        assert_eq!(
            gs1encoder.get_hri(),
            vec!["(01) 12312312312333", "(22) TESTING", "(99) ABC"]
        );
    }

    #[test]
    fn test_err_markup() {
        let mut gs1encoder = GS1Encoder::new().unwrap();

        let err = gs1encoder
            .set_data_str("^011234567890128399ABC")
            .unwrap_err();
        assert!(matches!(err, GS1EncoderError::GS1ParameterError(_)));
        assert!(
            err.to_string().contains("check digit"),
            "Expected 'check digit' in error: {err}"
        );
        assert!(!gs1encoder.get_err_markup().is_empty());
        assert!(
            gs1encoder.get_err_markup().contains('|'),
            "Error markup should contain '|' delimiters: {}",
            gs1encoder.get_err_markup()
        );
    }
}
