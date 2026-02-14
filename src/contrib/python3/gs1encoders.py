#
#  GS1 Barcode Syntax Engine
#
#  This is a contributed example that shows how a Python 3 language binding might
#  be developed.
#
#
#  @author Copyright (c) 2021-2026 GS1 AISBL.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

import ctypes
import enum
import warnings


class Symbology(enum.IntEnum):
    NONE = -1
    DataBarOmni = 0
    DataBarTruncated = 1
    DataBarStacked = 2
    DataBarStackedOmni = 3
    DataBarLimited = 4
    DataBarExpanded = 5
    UPCA = 6
    UPCE = 7
    EAN13 = 8
    EAN8 = 9
    GS1_128_CCA = 10
    GS1_128_CCC = 11
    QR = 12
    DM = 13
    DotCode = 14


class Validation(enum.IntEnum):
    MutexAIs = 0
    RequisiteAIs = 1
    RepeatedAIs = 2
    DigSigSerialKey = 3
    UnknownAInotDLattr = 4


class GS1Encoder:
    __ctx = None
    __api = ctypes.cdll.LoadLibrary("libgs1encoders.so")

    __api.gs1_encoder_getVersion.argtypes = []
    __api.gs1_encoder_getVersion.restype = ctypes.c_char_p

    __api.gs1_encoder_init.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_init.restype = ctypes.POINTER(ctypes.c_void_p)

    __api.gs1_encoder_free.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_free.restype = None

    __api.gs1_encoder_getErrMsg.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getErrMsg.restype = ctypes.c_char_p

    __api.gs1_encoder_getErrMarkup.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getErrMarkup.restype = ctypes.c_char_p

    __api.gs1_encoder_getSym.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getSym.restype = ctypes.c_int

    __api.gs1_encoder_setSym.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_int,
    ]
    __api.gs1_encoder_setSym.restype = ctypes.c_bool

    __api.gs1_encoder_getAddCheckDigit.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getAddCheckDigit.restype = ctypes.c_bool

    __api.gs1_encoder_setAddCheckDigit.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setAddCheckDigit.restype = ctypes.c_bool

    __api.gs1_encoder_getPermitUnknownAIs.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getPermitUnknownAIs.restype = ctypes.c_bool

    __api.gs1_encoder_setPermitUnknownAIs.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setPermitUnknownAIs.restype = ctypes.c_bool

    __api.gs1_encoder_getPermitZeroSuppressedGTINinDLuris.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getPermitZeroSuppressedGTINinDLuris.restype = ctypes.c_bool

    __api.gs1_encoder_setPermitZeroSuppressedGTINinDLuris.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setPermitZeroSuppressedGTINinDLuris.restype = ctypes.c_bool

    __api.gs1_encoder_getValidationEnabled.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_int,
    ]
    __api.gs1_encoder_getValidationEnabled.restype = ctypes.c_bool

    __api.gs1_encoder_setValidationEnabled.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_int,
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setValidationEnabled.restype = ctypes.c_bool

    __api.gs1_encoder_getIncludeDataTitlesInHRI.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getIncludeDataTitlesInHRI.restype = ctypes.c_bool

    __api.gs1_encoder_setIncludeDataTitlesInHRI.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setIncludeDataTitlesInHRI.restype = ctypes.c_bool

    __api.gs1_encoder_getDataStr.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getDataStr.restype = ctypes.c_char_p

    __api.gs1_encoder_setDataStr.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_char_p,
    ]
    __api.gs1_encoder_setDataStr.restype = ctypes.c_bool

    __api.gs1_encoder_getAIdataStr.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getAIdataStr.restype = ctypes.c_char_p

    __api.gs1_encoder_setAIdataStr.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_char_p,
    ]
    __api.gs1_encoder_setAIdataStr.restype = ctypes.c_bool

    __api.gs1_encoder_getScanData.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_getScanData.restype = ctypes.c_char_p

    __api.gs1_encoder_setScanData.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_char_p,
    ]
    __api.gs1_encoder_setScanData.restype = ctypes.c_bool

    __api.gs1_encoder_getDLuri.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_char_p,
    ]
    __api.gs1_encoder_getDLuri.restype = ctypes.c_char_p

    __api.gs1_encoder_getDLignoredQueryParams.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)),
    ]
    __api.gs1_encoder_getDLignoredQueryParams.restype = ctypes.c_int

    __api.gs1_encoder_getHRI.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)),
    ]
    __api.gs1_encoder_getHRI.restype = ctypes.c_int

    def get_err_msg(self):
        return self.__api.gs1_encoder_getErrMsg(self.__ctx).decode("utf-8")

    def __init__(self):
        self.__ctx = self.__api.gs1_encoder_init(None)
        if self.__ctx is None:
            raise GS1EncoderGeneralException("Failed to initialise the native library")

    def __del__(self):
        self.free()

    def free(self):
        if self.__ctx is not None:
            self.__api.gs1_encoder_free(self.__ctx)
            self.__ctx = None

    def get_version(self):
        return self.__api.gs1_encoder_getVersion().decode("utf-8")

    def get_err_markup(self):
        return self.__api.gs1_encoder_getErrMarkup(self.__ctx).decode("utf-8")

    def get_sym(self):
        return Symbology(self.__api.gs1_encoder_getSym(self.__ctx))

    def set_sym(self, sym):
        ret = self.__api.gs1_encoder_setSym(self.__ctx, int(sym))
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_add_check_digit(self):
        return bool(self.__api.gs1_encoder_getAddCheckDigit(self.__ctx))

    def set_add_check_digit(self, value):
        ret = self.__api.gs1_encoder_setAddCheckDigit(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_permit_unknown_ais(self):
        return bool(self.__api.gs1_encoder_getPermitUnknownAIs(self.__ctx))

    def set_permit_unknown_ais(self, value):
        ret = self.__api.gs1_encoder_setPermitUnknownAIs(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_permit_zero_suppressed_gtin_in_dl_uris(self):
        return bool(self.__api.gs1_encoder_getPermitZeroSuppressedGTINinDLuris(self.__ctx))

    def set_permit_zero_suppressed_gtin_in_dl_uris(self, value):
        ret = self.__api.gs1_encoder_setPermitZeroSuppressedGTINinDLuris(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_validation_enabled(self, validation):
        return bool(self.__api.gs1_encoder_getValidationEnabled(self.__ctx, int(validation)))

    def set_validation_enabled(self, validation, enabled):
        ret = self.__api.gs1_encoder_setValidationEnabled(self.__ctx, int(validation), 1 if enabled else 0)
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_validate_ai_associations(self):
        warnings.warn(
            "Use get_validation_enabled(Validation.RequisiteAIs) instead",
            DeprecationWarning,
            stacklevel=2,
        )
        return self.get_validation_enabled(Validation.RequisiteAIs)

    def set_validate_ai_associations(self, value):
        warnings.warn(
            "Use set_validation_enabled(Validation.RequisiteAIs, value) instead",
            DeprecationWarning,
            stacklevel=2,
        )
        self.set_validation_enabled(Validation.RequisiteAIs, value)

    def set_include_data_titles_in_hri(self, value):
        ret = self.__api.gs1_encoder_setIncludeDataTitlesInHRI(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_include_data_titles_in_hri(self):
        return bool(self.__api.gs1_encoder_getIncludeDataTitlesInHRI(self.__ctx))

    def get_data_str(self):
        return self.__api.gs1_encoder_getDataStr(self.__ctx).decode("utf-8")

    def set_data_str(self, value):
        ret = self.__api.gs1_encoder_setDataStr(self.__ctx, value.encode("utf-8"))
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_ai_data_str(self):
        ret = self.__api.gs1_encoder_getAIdataStr(self.__ctx)
        if not ret:
            return None
        return ret.decode("utf-8")

    def set_ai_data_str(self, value):
        ret = self.__api.gs1_encoder_setAIdataStr(self.__ctx, value.encode("utf-8"))
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_scan_data(self):
        ret = self.__api.gs1_encoder_getScanData(self.__ctx)
        if not ret:
            return None
        return ret.decode("utf-8")

    def set_scan_data(self, value):
        ret = self.__api.gs1_encoder_setScanData(self.__ctx, value.encode("utf-8"))
        if not ret:
            raise GS1EncoderScanDataException(self.get_err_msg())

    def get_dl_uri(self, stem=None):
        c_stem = stem.encode("utf-8") if stem else None
        ret = self.__api.gs1_encoder_getDLuri(self.__ctx, c_stem)
        if not ret:
            return None
        return ret.decode("utf-8")

    def get_dl_ignored_query_params(self):
        ptr = ctypes.pointer(ctypes.c_char_p())
        size = self.__api.gs1_encoder_getDLignoredQueryParams(self.__ctx, ctypes.byref(ptr))
        params = []
        for i in range(size):
            params.append(ptr[i].decode("utf-8"))
        return params

    def get_hri(self):
        ptr = ctypes.pointer(ctypes.c_char_p())
        size = self.__api.gs1_encoder_getHRI(self.__ctx, ctypes.byref(ptr))
        hri = []
        for i in range(size):
            hri.append(ptr[i].decode("utf-8"))
        return hri


class GS1EncoderGeneralException(Exception):
    pass


class GS1EncoderParameterException(Exception):
    pass


class GS1EncoderDigitalLinkException(Exception):
    pass


class GS1EncoderScanDataException(Exception):
    pass
