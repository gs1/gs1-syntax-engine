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

class GS1Encoder:

    __ctx = None
    __api = ctypes.cdll.LoadLibrary("libgs1encoders.so")


    __api.gs1_encoder_getVersion.argtypes = []
    __api.gs1_encoder_getVersion.restype  = ctypes.c_char_p

    __api.gs1_encoder_init.argtypes = [ ctypes.POINTER(ctypes.c_void_p) ]
    __api.gs1_encoder_init.restype  = ctypes.POINTER(ctypes.c_void_p)

    __api.gs1_encoder_free.argtypes = [ ctypes.POINTER(ctypes.c_void_p) ]
    __api.gs1_encoder_free.restype  = None

    __api.gs1_encoder_getErrMsg.argtypes = [ ctypes.POINTER(ctypes.c_void_p) ]
    __api.gs1_encoder_getErrMsg.restype  = ctypes.c_char_p

    __api.gs1_encoder_setIncludeDataTitlesInHRI.argtypes = [ ctypes.POINTER(ctypes.c_void_p), ctypes.c_bool ]
    __api.gs1_encoder_setIncludeDataTitlesInHRI.restype  = ctypes.c_bool

    __api.gs1_encoder_getDataStr.argtypes = [ ctypes.POINTER(ctypes.c_void_p) ]
    __api.gs1_encoder_getDataStr.restype  = ctypes.c_char_p

    __api.gs1_encoder_setDataStr.argtypes = [ ctypes.POINTER(ctypes.c_void_p), ctypes.c_char_p ]
    __api.gs1_encoder_setDataStr.restype  = ctypes.c_bool

    __api.gs1_encoder_getAIdataStr.argtypes = [ ctypes.POINTER(ctypes.c_void_p) ]
    __api.gs1_encoder_getAIdataStr.restype  = ctypes.c_char_p

    __api.gs1_encoder_getHRI.argtypes = [ ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)) ]
    __api.gs1_encoder_getHRI.restype  = ctypes.c_int


    def get_err_msg(self):
        return self.__api.gs1_encoder_getErrMsg(self.__ctx).decode("utf-8")

    def __init__(self):
        self.__ctx = self.__api.gs1_encoder_init(None)
        if self.__ctx == None:
            raise GS1EncoderGeneralException('Failed to initialise the native library')

    def __del__(self):
        self.free()

    def free(self):
        self.__api.gs1_encoder_free(self.__ctx)
        self.__ctx = None

    def get_version(self):
        return self.__api.gs1_encoder_getVersion().decode("utf-8")

    def set_include_data_titles_in_hri(self, value):
        ret = self.__api.gs1_encoder_setIncludeDataTitlesInHRI(self.__ctx, 1 if value else 0)
        if not ret:
            raise GS1EncoderParameterException(self.get_err_msg())

    def get_include_data_titles_in_hri(self):
        return self.__api.gs1_encoder_getIncludeDataTitlesInHRI(self.__ctx) == 1

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

    def get_hri(self):
        ptr = ctypes.pointer(ctypes.c_char_p())
        size = self.__api.gs1_encoder_getHRI(self.__ctx, ctypes.byref(ptr))
        hri = [None] * size
        for i in range(size):
            hri[i] = ptr[i].decode("utf-8")
        return hri


class GS1EncoderGeneralException(Exception):
    pass

class GS1EncoderParameterException(Exception):
    pass

class GS1EncoderDigitalLinkException(Exception):
    pass

class GS1EncoderScanDataException(Exception):
    pass

