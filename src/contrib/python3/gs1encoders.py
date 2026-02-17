"""Python 3 binding for the GS1 Barcode Syntax Engine native C library."""

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

from __future__ import annotations

import ctypes
import enum
import warnings

__all__ = [
    "GS1Encoder",
    "GS1EncoderDigitalLinkException",
    "GS1EncoderGeneralException",
    "GS1EncoderParameterException",
    "GS1EncoderScanDataException",
]


class Symbology(enum.IntEnum):
    """GS1 barcode symbology types."""

    NONE = -1
    DATA_BAR_OMNI = 0
    DATA_BAR_TRUNCATED = 1
    DATA_BAR_STACKED = 2
    DATA_BAR_STACKED_OMNI = 3
    DATA_BAR_LIMITED = 4
    DATA_BAR_EXPANDED = 5
    UPCA = 6
    UPCE = 7
    EAN13 = 8
    EAN8 = 9
    GS1_128_CCA = 10
    GS1_128_CCC = 11
    QR = 12
    DM = 13
    DOT_CODE = 14


class Validation(enum.IntEnum):
    """GS1 AI data validation types."""

    MUTEX_AIS = 0
    REQUISITE_AIS = 1
    REPEATED_AIS = 2
    DIG_SIG_SERIAL_KEY = 3
    UNKNOWN_AI_NOT_DL_ATTR = 4


class GS1Encoder:
    """Wrapper around the GS1 Barcode Syntax Engine native C library."""

    __ctx = None
    __api: ctypes.CDLL = ctypes.cdll.LoadLibrary("libgs1encoders.so")

    __api.gs1_encoder_getVersion.argtypes = []
    __api.gs1_encoder_getVersion.restype = ctypes.c_char_p

    __api.gs1_encoder_init.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_init.restype = ctypes.POINTER(ctypes.c_void_p)

    __api.gs1_encoder_free.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    __api.gs1_encoder_free.restype = None

    __api.gs1_encoder_getErrMsg.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getErrMsg.restype = ctypes.c_char_p

    __api.gs1_encoder_getErrMarkup.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getErrMarkup.restype = ctypes.c_char_p

    __api.gs1_encoder_getSym.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getSym.restype = ctypes.c_int

    __api.gs1_encoder_setSym.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_int,
    ]
    __api.gs1_encoder_setSym.restype = ctypes.c_bool

    __api.gs1_encoder_getAddCheckDigit.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getAddCheckDigit.restype = ctypes.c_bool

    __api.gs1_encoder_setAddCheckDigit.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setAddCheckDigit.restype = ctypes.c_bool

    __api.gs1_encoder_getPermitUnknownAIs.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getPermitUnknownAIs.restype = ctypes.c_bool

    __api.gs1_encoder_setPermitUnknownAIs.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setPermitUnknownAIs.restype = ctypes.c_bool

    __api.gs1_encoder_getPermitZeroSuppressedGTINinDLuris.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
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

    __api.gs1_encoder_getIncludeDataTitlesInHRI.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getIncludeDataTitlesInHRI.restype = ctypes.c_bool

    __api.gs1_encoder_setIncludeDataTitlesInHRI.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_bool,
    ]
    __api.gs1_encoder_setIncludeDataTitlesInHRI.restype = ctypes.c_bool

    __api.gs1_encoder_getDataStr.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getDataStr.restype = ctypes.c_char_p

    __api.gs1_encoder_setDataStr.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_char_p,
    ]
    __api.gs1_encoder_setDataStr.restype = ctypes.c_bool

    __api.gs1_encoder_getAIdataStr.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
    __api.gs1_encoder_getAIdataStr.restype = ctypes.c_char_p

    __api.gs1_encoder_setAIdataStr.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.c_char_p,
    ]
    __api.gs1_encoder_setAIdataStr.restype = ctypes.c_bool

    __api.gs1_encoder_getScanData.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
    ]
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

    def __init__(self) -> None:
        """Create a new GS1 encoder instance."""
        self.__ctx = self.__api.gs1_encoder_init(None)
        if self.__ctx is None:
            raise GS1EncoderGeneralException("Failed to initialise the native library")

    def __del__(self) -> None:
        """Release the native library context on garbage collection."""
        self.free()

    def __enter__(self) -> "GS1Encoder":
        """Enter a context manager block."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> bool:
        """Exit a context manager block, freeing native resources."""
        self.free()
        return False

    def free(self) -> None:
        """Release the native library context."""
        if self.__ctx is not None:
            self.__api.gs1_encoder_free(self.__ctx)
            self.__ctx = None

    @property
    def version(self) -> str:
        """The library version string."""
        ret: bytes = self.__api.gs1_encoder_getVersion()
        return ret.decode("utf-8")

    # This Python wrapper library raises an exception containing the error
    # message whenever an error is returned by the native library.  Therefore
    # direct access to the native error message is not necessary.
    def _get_err_msg(self) -> str:
        ret: bytes = self.__api.gs1_encoder_getErrMsg(self.__ctx)
        return ret.decode("utf-8")

    @property
    def err_markup(self) -> str:
        """The current error markup from the library context."""
        ret: bytes = self.__api.gs1_encoder_getErrMarkup(self.__ctx)
        return ret.decode("utf-8")

    @property
    def sym(self) -> Symbology:
        """The currently selected symbology."""
        return Symbology(self.__api.gs1_encoder_getSym(self.__ctx))

    @sym.setter
    def sym(self, value: Symbology) -> None:
        ret: bool = self.__api.gs1_encoder_setSym(self.__ctx, int(value))
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def add_check_digit(self) -> bool:
        """Whether automatic check digit addition is enabled."""
        return bool(self.__api.gs1_encoder_getAddCheckDigit(self.__ctx))

    @add_check_digit.setter
    def add_check_digit(self, value: bool) -> None:
        ret: bool = self.__api.gs1_encoder_setAddCheckDigit(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def permit_unknown_ais(self) -> bool:
        """Whether unknown AIs are permitted."""
        return bool(self.__api.gs1_encoder_getPermitUnknownAIs(self.__ctx))

    @permit_unknown_ais.setter
    def permit_unknown_ais(self, value: bool) -> None:
        ret: bool = self.__api.gs1_encoder_setPermitUnknownAIs(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def permit_zero_suppressed_gtin_in_dl_uris(self) -> bool:
        """Whether zero-suppressed GTINs are permitted in DL URIs."""
        return bool(self.__api.gs1_encoder_getPermitZeroSuppressedGTINinDLuris(self.__ctx))

    @permit_zero_suppressed_gtin_in_dl_uris.setter
    def permit_zero_suppressed_gtin_in_dl_uris(self, value: bool) -> None:
        val: int = 1 if value else 0
        ret: bool = self.__api.gs1_encoder_setPermitZeroSuppressedGTINinDLuris(self.__ctx, val)
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    def get_validation_enabled(self, validation: Validation) -> bool:
        """Return whether the specified validation is enabled."""
        return bool(self.__api.gs1_encoder_getValidationEnabled(self.__ctx, int(validation)))

    def set_validation_enabled(self, validation: Validation, enabled: bool) -> None:
        """Enable or disable the specified validation."""
        val: int = 1 if enabled else 0
        ret: bool = self.__api.gs1_encoder_setValidationEnabled(self.__ctx, int(validation), val)
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def validate_ai_associations(self) -> bool:
        """Whether AI association validation is enabled.

        .. deprecated::
            Use ``get_validation_enabled(Validation.REQUISITE_AIS)``
            instead.
        """
        msg: str = "Use get_validation_enabled(Validation.REQUISITE_AIS) instead"
        warnings.warn(msg, DeprecationWarning, stacklevel=2)
        return self.get_validation_enabled(Validation.REQUISITE_AIS)

    @validate_ai_associations.setter
    def validate_ai_associations(self, value: bool) -> None:
        msg: str = "Use set_validation_enabled(Validation.REQUISITE_AIS, value) instead"
        warnings.warn(msg, DeprecationWarning, stacklevel=2)
        self.set_validation_enabled(Validation.REQUISITE_AIS, value)

    @property
    def include_data_titles_in_hri(self) -> bool:
        """Whether data titles are included in HRI output."""
        return bool(self.__api.gs1_encoder_getIncludeDataTitlesInHRI(self.__ctx))

    @include_data_titles_in_hri.setter
    def include_data_titles_in_hri(self, value: bool) -> None:
        ret: bool = self.__api.gs1_encoder_setIncludeDataTitlesInHRI(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def data_str(self) -> str:
        """The current raw data string."""
        ret: bytes = self.__api.gs1_encoder_getDataStr(self.__ctx)
        return ret.decode("utf-8")

    @data_str.setter
    def data_str(self, value: str) -> None:
        ret: bool = self.__api.gs1_encoder_setDataStr(self.__ctx, value.encode("utf-8"))
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def ai_data_str(self) -> str | None:
        """The current AI data string, or None if unavailable."""
        ret: bytes | None = self.__api.gs1_encoder_getAIdataStr(self.__ctx)
        if not ret:
            return None
        return ret.decode("utf-8")

    @ai_data_str.setter
    def ai_data_str(self, value: str) -> None:
        ret: bool = self.__api.gs1_encoder_setAIdataStr(self.__ctx, value.encode("utf-8"))
        if not ret:
            raise GS1EncoderParameterException(self._get_err_msg())

    @property
    def scan_data(self) -> str | None:
        """The scan data string, or None if no symbology is set."""
        ret: bytes | None = self.__api.gs1_encoder_getScanData(self.__ctx)
        if not ret:
            return None
        return ret.decode("utf-8")

    @scan_data.setter
    def scan_data(self, value: str) -> None:
        ret: bool = self.__api.gs1_encoder_setScanData(self.__ctx, value.encode("utf-8"))
        if not ret:
            raise GS1EncoderScanDataException(self._get_err_msg())

    def get_dl_uri(self, stem: str | None = None) -> str:
        """Return a GS1 Digital Link URI.

        Raises:
            GS1EncoderDigitalLinkException: If a Digital Link URI cannot
                be generated from the current data.
        """
        c_stem: bytes | None = stem.encode("utf-8") if stem else None
        ret: bytes | None = self.__api.gs1_encoder_getDLuri(self.__ctx, c_stem)
        if not ret:
            raise GS1EncoderDigitalLinkException(self._get_err_msg())
        return ret.decode("utf-8")

    @property
    def dl_ignored_query_params(self) -> list[str]:
        """Non-AI query parameters from a GS1 Digital Link URI."""
        ptr = ctypes.pointer(ctypes.c_char_p())
        size: int = self.__api.gs1_encoder_getDLignoredQueryParams(self.__ctx, ctypes.byref(ptr))
        params: list[str] = []
        for i in range(size):
            params.append(ptr[i].decode("utf-8"))
        return params

    @property
    def hri(self) -> list[str]:
        """The Human Readable Interpretation strings."""
        ptr = ctypes.pointer(ctypes.c_char_p())
        size: int = self.__api.gs1_encoder_getHRI(self.__ctx, ctypes.byref(ptr))
        hri: list[str] = []
        for i in range(size):
            hri.append(ptr[i].decode("utf-8"))
        return hri


class GS1EncoderGeneralException(Exception):
    """Raised for general library initialisation failures."""


class GS1EncoderParameterException(Exception):
    """Raised when an invalid parameter is provided to the library."""


class GS1EncoderDigitalLinkException(Exception):
    """Raised for GS1 Digital Link URI processing errors."""


class GS1EncoderScanDataException(Exception):
    """Raised for barcode scan data processing errors."""
