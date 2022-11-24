using System;
using System.Runtime.InteropServices;

namespace GS1.Encoders
{

    /// <summary>
    /// Wrapper class for accessing the GS1 Syntax Engine native library from C#.
    ///
    /// Copyright (c) 2021 GS1 AISBL.
    ///
    /// Licensed under the Apache License, Version 2.0 (the "License");
    /// you may not use this file except in compliance with the License.
    ///
    /// You may obtain a copy of the License at
    ///
    ///     http://www.apache.org/licenses/LICENSE-2.0
    ///
    /// Unless required by applicable law or agreed to in writing, software
    /// distributed under the License is distributed on an "AS IS" BASIS,
    /// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    /// See the License for the specific language governing permissions and
    /// limitations under the License.
    ///
    ///
    /// This class implements a P/Invoke wrapper around the GS1 Syntax Engine
    /// native C library that presents its functionality in the form of a
    /// typical C# object interface.
    ///
    /// This class is a very lightweight shim around the native library,
    /// therefore the C# interface is described here in terms of the public
    /// API functions of the native library that each method or property
    /// getter/setter invokes.
    ///
    /// The API reference for the native C library is available here:
    ///
    /// https://gs1.github.io/gs1-syntax-engine/
    ///
    /// </summary>
    public class GS1Encoder
    {

        /// <summary>
        /// List of symbology types, mirroring the corresponding list in the
        /// C library.
        ///
        /// See the native library documentation for details:
        ///
        ///   - enum gs1_encoder_symbologies
        ///
        /// </summary>
        public enum Symbology
        {
            /// <summary>None defined</summary>
            NONE = -1,
            /// <summary>GS1 DataBar Omnidirectional</summary>
            DataBarOmni,
            /// <summary>GS1 DataBar Truncated</summary>
            DataBarTruncated,
            /// <summary>GS1 DataBar Stacked</summary>
            DataBarStacked,
            /// <summary>GS1 DataBar Stacked Omnidirectional</summary>
            DataBarStackedOmni,
            /// <summary>GS1 DataBar Limited</summary>
            DataBarLimited,
            /// <summary>GS1 DataBar Expanded (Stacked)</summary>
            DataBarExpanded,
            /// <summary>UPC-A</summary>
            UPCA,
            /// <summary>UPC-E</summary>
            UPCE,
            /// <summary>EAN-13</summary>
            EAN13,
            /// <summary>EAN-8</summary>
            EAN8,
            /// <summary>GS1-128 with CC-A or CC-B</summary>
            GS1_128_CCA,
            /// <summary>GS1-128 with CC</summary>
            GS1_128_CCC,
            /// <summary>(GS1) QR Code</summary>
            QR,
            /// <summary>(GS1) Data Matrix</summary>
            DM,
            /// <summary>Value is the number of symbologies</summary>
            NUMSYMS,
        };

        /// <summary>
        /// The expected name of the GS1 Syntax Engine dynamic-link library
        /// </summary>
        private const String gs1_dll = "gs1encoders.dll";

        /// <summary>
        /// An opaque pointer used by the native code to represent an
        /// "instance" of the library. It is hidden behind the object
        /// interface that is provided to users of this wrapper.
        ///
        /// See the native library documentation for details:
        ///
        ///   - typedef struct gs1_encoder
        ///
        /// </summary>
        private readonly IntPtr ctx;

        /*
         *  Functions imported from the native GS1 Syntax Engine dynamic-link library
         *
         */
        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_init", CallingConvention = CallingConvention.Cdecl)]
        private static extern System.IntPtr gs1_encoder_init(IntPtr mem);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getVersion", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getVersion();

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getErrMsg", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getErrMsg(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getErrMarkup", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getErrMarkup(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getSym", CallingConvention = CallingConvention.Cdecl)]
        private static extern int gs1_encoder_getSym(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setSym", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setSym(IntPtr ctx, int sym);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getAddCheckDigit", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_getAddCheckDigit(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setAddCheckDigit", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setAddCheckDigit(IntPtr ctx, [MarshalAs(UnmanagedType.U1)] bool addCheckDigit);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getIncludeDataTitlesInHRI", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_getIncludeDataTitlesInHRI(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setIncludeDataTitlesInHRI", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setIncludeDataTitlesInHRI(IntPtr ctx, [MarshalAs(UnmanagedType.U1)] bool addCheckDigit);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getPermitUnknownAIs", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_getPermitUnknownAIs(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setPermitUnknownAIs", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setPermitUnknownAIs(IntPtr ctx, [MarshalAs(UnmanagedType.U1)] bool permitUnknownAIs);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getValidateAIassociations", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_getValidateAIassociations(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setValidateAIassociations", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setValidateAIassociations(IntPtr ctx, [MarshalAs(UnmanagedType.U1)] bool validateAIasssociations);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getDataStr", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getDataStr(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setDataStr", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setDataStr(IntPtr ctx, string dataStr);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setAIdataStr", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setAIdataStr(IntPtr ctx, string aiData);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getAIdataStr", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getAIdataStr(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getScanData", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getScanData(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setScanData", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setScanData(IntPtr ctx, string scanData);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getDLuri", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getDLuri(IntPtr ctx, string stem);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getHRI", CallingConvention = CallingConvention.Cdecl)]
        private static extern int gs1_encoder_getHRI(IntPtr ctx, ref IntPtr hri);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getDLignoredQueryParams", CallingConvention = CallingConvention.Cdecl)]
        private static extern int gs1_encoder_getDLignoredQueryParams(IntPtr ctx, ref IntPtr qp);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_getDataFile", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr gs1_encoder_getDataFile(IntPtr ctx);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_setDataFile", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool gs1_encoder_setDataFile(IntPtr ctx, string dataFile);

        [DllImport(gs1_dll, EntryPoint = "gs1_encoder_free", CallingConvention = CallingConvention.Cdecl)]
        private static extern void gs1_encoder_free(IntPtr ctx);


        /*
         *  Methods to provide a wrapper around the functional interface imported from the native library
         *
         */

        // This C# wrapper library throws an excpetion containing the error message whenever
        // an error is returned by the native library. Therefore direct access to the native
        // error message is not necessary.
        private string ErrMsg
        {
            get
            {
                return Marshal.PtrToStringAnsi(gs1_encoder_getErrMsg(ctx));
            }
        }


        /// <summary>
        /// Constructor that creates an object wrapping an "instance" of the library managed by the native code.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_init()
        ///
        /// </summary>
        public GS1Encoder()
        {
            ctx = gs1_encoder_init(IntPtr.Zero);
            if (ctx == IntPtr.Zero)
                throw new GS1EncoderGeneralException("Failed to initalise the native library");
        }

        /// <summary>
        /// Returns the version of the native library.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getVersion()
        ///
        /// </summary>
        public string Version
        {
            get
            {
                return Marshal.PtrToStringAnsi(gs1_encoder_getVersion());
            }
        }

        /// <summary>
        /// Get/set the symbology type.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getSym()
        ///   - gs1_encoder_setSym()
        ///
        /// </summary>
        public int Sym
        {
            get
            {
                return gs1_encoder_getSym(ctx);
            }
            set
            {
                if (!gs1_encoder_setSym(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }

        /// <summary>
        /// Get/set the "add check digit" mode for EAN/UPC and GS1 DataBar symbols.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getAddCheckDigit()
        ///   - gs1_encoder_setAddCheckDigit()
        ///
        /// </summary>
        public bool AddCheckDigit
        {
            get
            {
                return gs1_encoder_getAddCheckDigit(ctx);
            }
            set
            {
                if (!gs1_encoder_setAddCheckDigit(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }


        /// <summary>
        /// Get/set the "include data titles in HRI" flag.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getIncludeDataTitlesInHRI()
        ///   - gs1_encoder_setIncludeDataTitlesInHRI()
        ///
        /// </summary>
        public bool IncludeDataTitlesInHRI
        {
            get
            {
                return gs1_encoder_getIncludeDataTitlesInHRI(ctx);
            }
            set
            {
                if (!gs1_encoder_setIncludeDataTitlesInHRI(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }


        /// <summary>
        /// Get/set the "permit unknown AIs" mode.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getPermitUnknownAIs()
        ///   - gs1_encoder_setPermitUnknownAIs()
        ///
        /// </summary>
        public bool PermitUnknownAIs
        {
            get
            {
                return gs1_encoder_getPermitUnknownAIs(ctx);
            }
            set
            {
                if (!gs1_encoder_setPermitUnknownAIs(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }

        /// <summary>
        /// Get/set the "validate AI associations" flag.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getValidateAIassociations()
        ///   - gs1_encoder_setValidateAIassociations()
        ///
        /// </summary>
        public bool ValidateAIassociations
        {
            get
            {
                return gs1_encoder_getValidateAIassociations(ctx);
            }
            set
            {
                if (!gs1_encoder_setValidateAIassociations(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }

        /// <summary>
        /// Get/set the raw barcode data input buffer.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getDataStr()
        ///   - gs1_encoder_setDataStr()
        ///
        /// </summary>
        public string DataStr
        {
            get
            {
                return Marshal.PtrToStringAnsi(gs1_encoder_getDataStr(ctx));
            }
            set
            {
                if (!gs1_encoder_setDataStr(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }

        /// <summary>
        /// Get/set the barcode data input buffer using GS1 AI syntax.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getAIdataStr()
        ///   - gs1_encoder_setAIdataStr()
        ///
        /// </summary>
        public string AIdataStr
        {
            get
            {
                return Marshal.PtrToStringAnsi(gs1_encoder_getAIdataStr(ctx));
            }
            set
            {
                if (!gs1_encoder_setAIdataStr(ctx, value))
                    throw new GS1EncoderParameterException(ErrMsg);
            }
        }

        /// <summary>
        /// Get/set the barcode data input buffer using barcode scan data format.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getScanData()
        ///   - gs1_encoder_setScanData()
        ///
        /// </summary>
        public string ScanData
        {
            get
            {
                return Marshal.PtrToStringAnsi(gs1_encoder_getScanData(ctx));
            }
            set
            {
                if (!gs1_encoder_setScanData(ctx, value))
                    throw new GS1EncoderScanDataException(ErrMsg);
            }
        }


        /// <summary>
        /// Read the error markup generated when parsing AI data fails due to a
        /// linting failure.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getErrMarkup()
        ///
        /// </summary>
        public string ErrMarkup
        {
            get
            {
                return Marshal.PtrToStringAnsi(gs1_encoder_getErrMarkup(ctx));
            }
        }


        /// <summary>
        /// Get a GS1 Digital Link URI that represents the AI-based input data.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getDLuri()
        ///
        /// </summary>
        public string GetDLuri(string Stem)
        {
            string uri = Marshal.PtrToStringAnsi(gs1_encoder_getDLuri(ctx, Stem));
            if (uri == null)
                throw new GS1EncoderDigitalLinkException(ErrMsg);
            return uri;
        }

        /// <summary>
        /// Get the Human-Readable Interpretation ("HRI") text for the current data input buffer as an array of strings.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getHRI()
        ///
        /// </summary>
        public string[] HRI
        {
            get
            {
                IntPtr p = IntPtr.Zero;
                int numAIs = gs1_encoder_getHRI(ctx, ref p);
                IntPtr[] pAI = new IntPtr[numAIs];
                Marshal.Copy(p, pAI, 0, numAIs);
                string[] hri = new string[numAIs];
                for (int i = 0; i < numAIs; i++)
                {
                    hri[i] = Marshal.PtrToStringAnsi(pAI[i]);
                }
                return hri;
            }
        }

        /// <summary>
        /// Get the non-numeric (ignored) query parameters from a GS1 Digital Link URI in the current data input buffer as an array of strings.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_getDLignoredQueryParams()
        ///
        /// </summary>
        public string[] DLignoredQueryParams
        {
            get
            {
                IntPtr p = IntPtr.Zero;
                int numAIs = gs1_encoder_getDLignoredQueryParams(ctx, ref p);
                IntPtr[] pAI = new IntPtr[numAIs];
                Marshal.Copy(p, pAI, 0, numAIs);
                string[] qp = new string[numAIs];
                for (int i = 0; i < numAIs; i++)
                {
                    qp[i] = Marshal.PtrToStringAnsi(pAI[i]);
                }
                return qp;
            }
        }

        /// <summary>
        /// Destructor that will release the resources allocated by the native library.
        ///
        /// See the native library documentation for details:
        ///
        ///   - gs1_encoder_free()
        ///
        /// </summary>
        ~GS1Encoder()
        {
            gs1_encoder_free(ctx);
        }

    }

    /// <summary>
    /// A custom exception class that is thrown to indicate a general problem
    /// initialising the library, such as when the dynamic-link library isn't accessible.
    /// </summary>
    public class GS1EncoderGeneralException : Exception
    {
        /// <summary>
        /// Error constructor.
        /// </summary>
        /// <param name="message">
        /// Description of the error.
        /// </param>
        public GS1EncoderGeneralException(string message)
           : base(message)
        {
        }
    }

    /// <summary>
    /// A custom exception class that is thrown to indicate a problem with barcode
    /// options that are being set.
    /// </summary>
    public class GS1EncoderParameterException : Exception
    {
        /// <summary>
        /// Error constructor.
        /// </summary>
        /// <param name="message">
        /// Description of the error.
        /// </param>
        public GS1EncoderParameterException(string message)
           : base(message)
        {
        }
    }

    /// <summary>
    /// A custom exception class that is thrown to indicate a problem generating
    /// a barcode symbol.
    /// </summary>
    public class GS1EncoderEncodeException : Exception
    {
        /// <summary>
        /// Error constructor.
        /// </summary>
        /// <param name="message">
        /// Description of the error.
        /// </param>
        public GS1EncoderEncodeException(string message)
           : base(message)
        {
        }
    }

    /// <summary>
    /// A custom exception class that is thrown to indicate an error processing
    /// barcode scan data.
    /// </summary>
    public class GS1EncoderScanDataException : Exception
    {
        /// <summary>
        /// Error constructor.
        /// </summary>
        /// <param name="message">
        /// Description of the error.
        /// </param>
        public GS1EncoderScanDataException(string message)
           : base(message)
        {
        }
    }

    /// <summary>
    /// A custom exception class that is thrown to indicate an error processing
    /// GS1 Digital Link data.
    /// </summary>
    public class GS1EncoderDigitalLinkException : Exception
    {
        /// <summary>
        /// Error constructor.
        /// </summary>
        /// <param name="message">
        /// Description of the error.
        /// </param>
        public GS1EncoderDigitalLinkException(string message)
           : base(message)
        {
        }
    }

}
