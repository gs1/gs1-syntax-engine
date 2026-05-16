/**
 * GS1 Barcode Syntax Engine
 *
 * @file gs1encoders.hpp
 * @author GS1 AISBL
 *
 * \copyright Copyright (c) 2026 GS1 AISBL.
 *
 * @licenseblock{License}
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
 * @endlicenseblock
 *
 * Header-only C++ wrapper for the GS1 Barcode Syntax Engine (requires
 * C++17 or later). Provides idiomatic C++ types (std::string, std::vector,
 * std::optional) and a typed exception hierarchy over the underlying
 * [C API](@ref capi); the native context is freed automatically when the
 * gs1encoders::GS1Encoder object is destroyed, whether that is at
 * end-of-scope or as part of a longer-lived holder. For an overview,
 * examples and usage instructions see the main documentation page.
 *
 */

/**
 * @defgroup cppapi C++ API
 * @brief Header-only C++ wrapper (requires C++17 or later) declared in
 * `gs1encoders.hpp`, in the @ref gs1encoders namespace.
 *
 * Wraps the [C API](@ref capi) with a move-only context class
 * (gs1encoders::GS1Encoder), a fluent gs1encoders::InitOpts builder,
 * a typed exception hierarchy rooted at
 * gs1encoders::GS1EncoderException, and `enum class` mirrors of the C
 * enums. The wrapper introduces no runtime overhead beyond the inline
 * calls into the underlying C library.
 *
 * The encoder owns the native context and releases it in its destructor,
 * so no manual cleanup call is needed regardless of how the object's
 * lifetime is managed: a function-scoped local, a member of a longer-lived
 * service object, or a `std::unique_ptr` / `std::shared_ptr` for shared
 * ownership all work.
 *
 * Usage:
 *
 * \code{.cpp}
 * gs1encoders::GS1Encoder gs;
 * gs.set_ai_data_str("(01)09521234543213(99)TESTING123");
 * std::string uri = gs.get_dl_uri("https://example.com");
 * \endcode
 *
 * To load a Syntax Dictionary file instead of the embedded AI table:
 *
 * \code{.cpp}
 * gs1encoders::GS1Encoder gs(gs1encoders::InitOpts{}
 *                                .syntax_dictionary("dict.txt")
 *                                .fallback_on_syndict_error(true));
 * \endcode
 */

#ifndef GS1ENCODERS_HPP
#define GS1ENCODERS_HPP

#include "gs1encoders.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

/// @addtogroup cppapi
/// @{

/// @brief Namespace containing the C++ wrapper for the GS1 Barcode Syntax Engine.
namespace gs1encoders {


/* ========================================================================
 *  Exceptions
 * ======================================================================== */

/// @ingroup cppapi
/// @brief Common base class for all exceptions thrown by this wrapper.
///
/// All wrapper exceptions derive from this type, which in turn derives
/// from `std::runtime_error`. Catching `gs1encoders::GS1EncoderException` is
/// sufficient to handle any failure originating in the wrapper or the
/// underlying library; catch a derived type to handle a specific category
/// of failure. The exception's `what()` carries the human-readable error
/// message produced by the library.
class GS1EncoderException : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

/// @ingroup cppapi
/// @brief Thrown to indicate a general problem initialising the library,
/// such as when the underlying AI table cannot be loaded.
class GS1EncoderGeneralException : public GS1EncoderException {
public:
	using GS1EncoderException::GS1EncoderException;
};

/// @ingroup cppapi
/// @brief Thrown to indicate a problem with the parameters or input data
/// passed to a setter. When the failure is a linting failure on AI data,
/// gs1encoders::GS1Encoder::err_markup() returns a marked-up rendering
/// of the offending AI.
class GS1EncoderParameterException : public GS1EncoderException {
public:
	using GS1EncoderException::GS1EncoderException;
};

/// @ingroup cppapi
/// @brief Thrown to indicate an error processing GS1 Digital Link data.
class GS1EncoderDigitalLinkException : public GS1EncoderException {
public:
	using GS1EncoderException::GS1EncoderException;
};

/// @ingroup cppapi
/// @brief Thrown to indicate an error processing barcode scan data.
class GS1EncoderScanDataException : public GS1EncoderException {
public:
	using GS1EncoderException::GS1EncoderException;
};


/* ========================================================================
 *  Enum mirrors
 * ======================================================================== */

/// @ingroup cppapi
/// @brief Recognised GS1 barcode formats ("symbologies") for processing scan data.
///
/// @see gs1encoders::GS1Encoder::sym()
/// @see gs1encoders::GS1Encoder::set_sym()
/// @see gs1encoders::GS1Encoder::set_scan_data()
enum class Symbology : int {
	None               = gs1_encoder_sNONE,			///< None defined.
	DataBarOmni        = gs1_encoder_sDataBarOmni,		///< GS1 DataBar Omnidirectional.
	DataBarTruncated   = gs1_encoder_sDataBarTruncated,	///< GS1 DataBar Truncated.
	DataBarStacked     = gs1_encoder_sDataBarStacked,	///< GS1 DataBar Stacked.
	DataBarStackedOmni = gs1_encoder_sDataBarStackedOmni,	///< GS1 DataBar Stacked Omnidirectional.
	DataBarLimited     = gs1_encoder_sDataBarLimited,	///< GS1 DataBar Limited.
	DataBarExpanded    = gs1_encoder_sDataBarExpanded,	///< GS1 DataBar Expanded (Stacked).
	UPCA               = gs1_encoder_sUPCA,			///< UPC-A.
	UPCE               = gs1_encoder_sUPCE,			///< UPC-E.
	EAN13              = gs1_encoder_sEAN13,		///< EAN-13.
	EAN8               = gs1_encoder_sEAN8,			///< EAN-8.
	GS1_128_CCA        = gs1_encoder_sGS1_128_CCA,		///< GS1-128 with CC-A or CC-B Composite Component.
	GS1_128_CCC        = gs1_encoder_sGS1_128_CCC,		///< GS1-128 with CC-C Composite Component.
	QR                 = gs1_encoder_sQR,			///< (GS1) QR Code.
	DM                 = gs1_encoder_sDM,			///< (GS1) Data Matrix.
	DotCode            = gs1_encoder_sDotCode,		///< (GS1) DotCode.
	NumSyms            = gs1_encoder_sNUMSYMS,		///< Number of defined symbologies; not itself a valid symbology.
};

/// @ingroup cppapi
/// @brief Optional AI validation procedures that may be applied to detect
/// invalid inputs.
///
/// These validation procedures are applied when AI data is supplied via
/// gs1encoders::GS1Encoder::set_ai_data_str(),
/// gs1encoders::GS1Encoder::set_data_str() or
/// gs1encoders::GS1Encoder::set_scan_data(). Some procedures are *locked*
/// (always enabled and not modifiable).
///
/// @see gs1encoders::GS1Encoder::validation_enabled()
/// @see gs1encoders::GS1Encoder::set_validation_enabled()
enum class Validation : int {
	MutexAIs           = gs1_encoder_vMUTEX_AIS,		///< Mutually exclusive AIs (locked: always enabled).
	RequisiteAIs       = gs1_encoder_vREQUISITE_AIS,	///< Mandatory associations between AIs.
	RepeatedAIs        = gs1_encoder_vREPEATED_AIS,		///< Repeated AIs having same value (locked: always enabled).
	DigSigSerialKey    = gs1_encoder_vDIGSIG_SERIAL_KEY,	///< Serialisation qualifier AIs must be present with Digital Signature (locked: always enabled).
	UnknownAInotDLattr = gs1_encoder_vUNKNOWN_AI_NOT_DL_ATTR,	///< Unknown AIs not permitted as GS1 DL URI data attributes.
	NumValidations     = gs1_encoder_vNUMVALIDATIONS,	///< Number of defined validation procedures; not itself a valid procedure.
};


/* ========================================================================
 *  Initialisation options
 * ======================================================================== */

class GS1Encoder;

/// @ingroup cppapi
/// @brief Initialisation options for the gs1encoders::GS1Encoder constructor.
///
/// Populated via the chained setters, then passed to the
/// gs1encoders::GS1Encoder constructor. New setters may be added in
/// future versions without breaking existing code.
struct InitOpts {

	/// @brief Set the path to a GS1 Syntax Dictionary file.
	/// If not set, the embedded AI table is used.
	///
	/// @param path path to the Syntax Dictionary file.
	/// @return reference to this InitOpts, for chaining.
	InitOpts &syntax_dictionary(std::string path) {
		syntax_dictionary_ = std::move(path);
		has_syntax_dictionary_ = true;
		return *this;
	}

	/// @brief Fall back to the embedded AI table if the Syntax Dictionary
	/// cannot be loaded. When enabled, the underlying load error is
	/// exposed via gs1encoders::GS1Encoder::init_fallback_warning() on
	/// successful construction.
	///
	/// @param enabled whether to enable fallback.
	/// @return reference to this InitOpts, for chaining.
	InitOpts &fallback_on_syndict_error(bool enabled) {
		fallback_on_syndict_error_ = enabled;
		return *this;
	}

	/// @brief Refuse to use the embedded AI table. Initialisation fails
	/// if no other AI table can be loaded.
	///
	/// @param enabled whether to disable the embedded AI table.
	/// @return reference to this InitOpts, for chaining.
	InitOpts &no_embedded(bool enabled) {
		no_embedded_ = enabled;
		return *this;
	}

private:
	friend class GS1Encoder;
	std::string syntax_dictionary_;
	bool has_syntax_dictionary_ = false;
	bool fallback_on_syndict_error_ = false;
	bool no_embedded_ = false;

};


/* ========================================================================
 *  GS1Encoder wrapper
 * ======================================================================== */

/// @ingroup cppapi
/// @brief Main class for processing GS1 barcode data, including validation,
/// format conversion, and generation of outputs such as GS1 Digital Link
/// URIs and Human-Readable Interpretation text.
///
/// Move-only: the underlying native context cannot be duplicated, but
/// ownership can be transferred. The destructor releases the native
/// resources, so the object can be used at any scope without manual
/// cleanup.
class GS1Encoder {
public:

	/// @brief Initialises a new instance of the GS1Encoder class.
	///
	/// Uses default options: the embedded AI table is loaded and no
	/// Syntax Dictionary file is read.
	///
	/// @throws GS1EncoderGeneralException if the library fails to initialise.
	/// @see GS1Encoder(const InitOpts &)
	GS1Encoder() : GS1Encoder(InitOpts{}) {}

	/// @brief Initialises a new instance of the GS1Encoder class with
	/// the given options.
	///
	/// If construction succeeds but the embedded AI table was loaded as
	/// a fallback (because the supplied
	/// gs1encoders::InitOpts::syntax_dictionary() failed to load and
	/// gs1encoders::InitOpts::fallback_on_syndict_error() was set), the
	/// underlying load error is exposed via init_fallback_warning().
	///
	/// @param opts initialisation options.
	/// @throws GS1EncoderGeneralException if the library fails to initialise.
	/// @see GS1Encoder()
	/// @see InitOpts
	/// @see init_fallback_warning()
	explicit GS1Encoder(const InitOpts &opts) {
		gs1_encoder_init_status_t status = GS1_ENCODERS_INIT_SUCCESS;
		char msg[MSG_BUF_SIZE] = { 0 };

		int flags = gs1_encoder_iDEFAULT;
		if (opts.fallback_on_syndict_error_)
			flags |= gs1_encoder_iFALLBACK_ON_SYNDICT_ERROR;
		if (opts.no_embedded_)
			flags |= gs1_encoder_iNO_EMBEDDED;

		gs1_encoder_init_opts_t native_opts = {};
		native_opts.struct_size      = sizeof(native_opts);
		native_opts.flags            = static_cast<gs1_encoder_init_flags_t>(flags);
		native_opts.status           = &status;
		native_opts.msgBuf           = msg;
		native_opts.msgBufSize       = sizeof(msg);
		native_opts.syntaxDictionary = opts.has_syntax_dictionary_
		                                   ? opts.syntax_dictionary_.c_str()
		                                   : nullptr;

		ctx_ = gs1_encoder_init_ex(nullptr, &native_opts);
		if (!ctx_)
			throw GS1EncoderGeneralException(
				*msg ? msg : "Failed to initialise the native library");

		if (status == GS1_ENCODERS_INIT_FALLBACK_TO_EMBEDDED_TABLE && *msg)
			init_fallback_warning_ = msg;
	}

	/// @brief Destructor. Releases the native context.
	///
	/// Safe to invoke on a moved-from object (which holds no context).
	///
	~GS1Encoder() {
		if (ctx_)
			gs1_encoder_free(ctx_);
	}

	/// @brief Copy construction is deleted: the underlying native
	/// context cannot be duplicated.
	GS1Encoder(const GS1Encoder &) = delete;

	/// @brief Copy assignment is deleted: the underlying native
	/// context cannot be duplicated.
	GS1Encoder &operator=(const GS1Encoder &) = delete;

	/// @brief Move-construct from another encoder.
	///
	/// Transfers ownership of the native context (and any
	/// init_fallback_warning() value) from `other`. After the move,
	/// `other` holds no context and must not be used for any operation
	/// other than destruction or move-assignment.
	GS1Encoder(GS1Encoder &&other) noexcept
		: ctx_(other.ctx_),
		  init_fallback_warning_(std::move(other.init_fallback_warning_)) {
		other.ctx_ = nullptr;
	}

	/// @brief Move-assign from another encoder.
	///
	/// Frees this object's existing native context (if any) and then
	/// transfers ownership of the native context (and any
	/// init_fallback_warning() value) from `other`. After the move,
	/// `other` holds no context and must not be used for any operation
	/// other than destruction or move-assignment. Self-assignment is a
	/// no-op.
	GS1Encoder &operator=(GS1Encoder &&other) noexcept {
		if (this != &other) {
			if (ctx_)
				gs1_encoder_free(ctx_);
			ctx_                   = other.ctx_;
			init_fallback_warning_ = std::move(other.init_fallback_warning_);
			other.ctx_             = nullptr;
		}
		return *this;
	}


	/* ----------------------------------------------------------------
	 *  Initialisation outcome
	 * ---------------------------------------------------------------- */

	/// @brief The warning message produced when initialisation fell back to
	/// the embedded AI table because the supplied Syntax Dictionary could
	/// not be loaded.
	///
	/// Set only when both gs1encoders::InitOpts::syntax_dictionary() and
	/// gs1encoders::InitOpts::fallback_on_syndict_error() were used, the
	/// dictionary file failed to open or parse, and the embedded AI table
	/// was loaded as a fallback. In that case the returned optional
	/// contains the load error reported by the underlying library
	/// (e.g. `"Cannot read file …"`); on plain successful initialisation,
	/// the returned optional is `std::nullopt`.
	///
	/// The value is fixed at construction; subsequent operations on the
	/// encoder do not modify it.
	///
	/// @return underlying load error message, or std::nullopt on plain success.
	/// @see GS1Encoder(const InitOpts &)
	/// @see InitOpts::fallback_on_syndict_error()
	std::optional<std::string> init_fallback_warning() const {
		if (init_fallback_warning_.empty())
			return std::nullopt;
		return init_fallback_warning_;
	}


	/* ----------------------------------------------------------------
	 *  Library-wide accessors
	 * ---------------------------------------------------------------- */

	/// @brief Get the version string of the library.
	///
	/// Returns a string containing the version of the library, typically
	/// the build date.
	///
	/// @return the version of the library.
	std::string version() const {
		const char *v = gs1_encoder_getVersion();
		return v ? v : std::string();
	}

	/// @brief Get the maximum size of the input data buffer for barcode
	/// message content.
	///
	/// This is an implementation limit; on memory-constrained systems it
	/// may be lowered by rebuilding the library. In practice each
	/// symbology has its own data capacity that may be somewhat less than
	/// this maximum.
	///
	/// @return the maximum number of bytes that may be supplied for encoding.
	/// @see set_data_str()
	/// @see set_ai_data_str()
	int max_data_str_length() const {
		return gs1_encoder_getMaxDataStrLength();
	}


	/* ----------------------------------------------------------------
	 *  Configuration getters/setters
	 * ---------------------------------------------------------------- */

	/// @brief Get the current symbology type.
	///
	/// May be set manually via set_sym() or automatically when processing
	/// scan data with set_scan_data().
	///
	/// @return the current symbology type.
	/// @see set_sym()
	/// @see set_scan_data()
	/// @see Symbology
	Symbology sym() const {
		return static_cast<Symbology>(gs1_encoder_getSym(ctx_));
	}
	/// @brief Set the current symbology type.
	///
	/// @param s the symbology type to set, from gs1encoders::Symbology.
	/// @throws GS1EncoderParameterException if an invalid symbology type
	///         is provided.
	/// @see sym()
	/// @see Symbology
	void set_sym(Symbology s) {
		check_param(gs1_encoder_setSym(
			ctx_, static_cast<gs1_encoder_symbologies_t>(s)));
	}

	/// @brief Get the current "add check digit" mode.
	///
	/// @return `true` if check digits will be generated automatically;
	///         `false` if the data must include a valid check digit.
	/// @see set_add_check_digit()
	bool add_check_digit() const {
		return gs1_encoder_getAddCheckDigit(ctx_);
	}
	/// @brief Enable or disable "add check digit" mode for EAN/UPC and
	/// GS1 DataBar symbols.
	///
	/// When `false` (the default), the input data must contain a valid
	/// check digit. When `true`, the input data must not contain a
	/// check digit; one is computed automatically.
	///
	/// This option is only meaningful for symbologies that accept
	/// fixed-length data — specifically EAN/UPC and GS1 DataBar except
	/// Expanded (Stacked).
	///
	/// @param v `true` to enable automatic check-digit generation;
	///          `false` to require it in the input.
	/// @throws GS1EncoderParameterException if the value is rejected.
	/// @see add_check_digit()
	void set_add_check_digit(bool v) {
		check_param(gs1_encoder_setAddCheckDigit(ctx_, v));
	}

	/// @brief Get the current "permit unknown AIs" mode.
	///
	/// @return `true` if unknown AIs are accepted in parsed input;
	///         `false` if all AIs in the input data must be known.
	/// @see set_permit_unknown_ais()
	bool permit_unknown_ais() const {
		return gs1_encoder_getPermitUnknownAIs(ctx_);
	}
	/// @brief Enable or disable acceptance of unknown AIs in parsed
	/// input.
	///
	/// When `false` (the default), all AIs represented by the input
	/// data must be known to the library's AI table. When `true`,
	/// unknown AIs (those not in the AI table) are accepted.
	///
	/// @note The option only applies to parsed input, specifically
	///       bracketed AI data supplied via set_ai_data_str() and GS1
	///       Digital Link URIs supplied via set_data_str(). Unbracketed
	///       AI element strings containing unknown AIs cannot be
	///       parsed because it is not possible to differentiate the AI
	///       from its data value when the length of the AI is uncertain.
	///
	/// @param v `true` to permit unknown AIs; `false` to reject them.
	/// @throws GS1EncoderParameterException if the value is rejected.
	/// @see permit_unknown_ais()
	/// @see set_ai_data_str()
	/// @see set_data_str()
	void set_permit_unknown_ais(bool v) {
		check_param(gs1_encoder_setPermitUnknownAIs(ctx_, v));
	}

	/// @brief Get the current "permit zero-suppressed GTIN in GS1 DL URIs" mode.
	///
	/// @return `true` if zero-suppressed GTINs are permitted in GS1
	///         Digital Link URIs; `false` otherwise.
	/// @see set_permit_zero_suppressed_gtin_in_dl_uris()
	bool permit_zero_suppressed_gtin_in_dl_uris() const {
		return gs1_encoder_getPermitZeroSuppressedGTINinDLuris(ctx_);
	}
	/// @brief Enable or disable acceptance of zero-suppressed GTINs in
	/// GS1 Digital Link URIs.
	///
	/// When `false` (the default), the value of a path component for
	/// AI (01) must be a full GTIN-14. When `true`, the value may be
	/// a GTIN-14 with leading zeros suppressed (a GTIN-13, GTIN-12 or
	/// GTIN-8).
	///
	/// This option only applies to parsed input data, specifically GS1
	/// Digital Link URIs. Since zero-suppressed GTINs are deprecated by
	/// the GS1 Digital Link standard, this option should only be enabled
	/// when it is necessary to accept legacy GS1 Digital Link URIs that
	/// carry zero-suppressed GTIN-14.
	///
	/// @param v `true` to accept zero-suppressed GTINs; `false` to
	///          require GTIN-14.
	/// @throws GS1EncoderParameterException if the value is rejected.
	/// @see permit_zero_suppressed_gtin_in_dl_uris()
	void set_permit_zero_suppressed_gtin_in_dl_uris(bool v) {
		check_param(gs1_encoder_setPermitZeroSuppressedGTINinDLuris(ctx_, v));
	}

	/// @brief Get the current "include data titles in HRI" mode.
	///
	/// @return `true` if HRI output includes AI Data Titles from the
	///         GS1 General Specifications; `false` otherwise.
	/// @see set_include_data_titles_in_hri()
	/// @see hri()
	bool include_data_titles_in_hri() const {
		return gs1_encoder_getIncludeDataTitlesInHRI(ctx_);
	}
	/// @brief Enable or disable inclusion of AI Data Titles in HRI output.
	///
	/// When `true`, AI data titles defined in the GS1 General
	/// Specifications are included in the HRI text returned by hri()
	/// to give "mixed HRI / non-HRI text" formatting. When `false`
	/// (the default), HRI is produced without data titles.
	///
	/// @param v `true` to include data titles; `false` to omit them.
	/// @throws GS1EncoderParameterException if the value is rejected.
	/// @see include_data_titles_in_hri()
	/// @see hri()
	void set_include_data_titles_in_hri(bool v) {
		check_param(gs1_encoder_setIncludeDataTitlesInHRI(ctx_, v));
	}

	/// @brief Get the current enabled status of an AI validation procedure.
	///
	/// Returns the status of one of the validation procedures defined in
	/// the gs1encoders::Validation enumeration. Some procedures are
	/// locked-on (always enabled and not modifiable); their getters
	/// always return `true`.
	///
	/// @param v the validation procedure to query.
	/// @return `true` if the procedure is currently enabled.
	/// @see set_validation_enabled()
	/// @see Validation
	bool validation_enabled(Validation v) const {
		return gs1_encoder_getValidationEnabled(
			ctx_, static_cast<gs1_encoder_validations_t>(v));
	}
	/// @brief Enable or disable an AI validation procedure.
	///
	/// Determines whether the given check is enforced when AI data is
	/// supplied via set_ai_data_str(), set_data_str() or set_scan_data().
	/// When `enabled` is `true` the validation will be enforced; when
	/// `false` it will not.
	///
	/// @note Some validation procedures are locked: always enabled and
	///       not modifiable. Attempting to change their status throws
	///       gs1encoders::GS1EncoderParameterException. The locked
	///       procedures are documented on each enumerator of
	///       gs1encoders::Validation.
	///
	/// @param v       the validation procedure to set.
	/// @param enabled `true` to enable; `false` to disable.
	/// @throws GS1EncoderParameterException if the procedure is locked
	///         or the value is otherwise rejected.
	/// @see validation_enabled()
	/// @see Validation
	void set_validation_enabled(Validation v, bool enabled) {
		check_param(gs1_encoder_setValidationEnabled(
			ctx_, static_cast<gs1_encoder_validations_t>(v), enabled));
	}


	/* ----------------------------------------------------------------
	 *  Data input / output
	 * ---------------------------------------------------------------- */

	/// @brief Get the raw data that would be directly encoded within a
	/// GS1 barcode message.
	///
	/// Returns the contents of the library's barcode message buffer.
	/// Equivalent to the value last passed to set_data_str(), or
	/// the encoding produced from set_ai_data_str() or set_scan_data().
	///
	/// @return the raw barcode data input buffer.
	/// @see set_data_str()
	/// @see ai_data_str()
	/// @see set_ai_data_str()
	/// @see set_scan_data()
	std::string data_str() const {
		const char *s = gs1_encoder_getDataStr(ctx_);
		return s ? s : std::string();
	}
	/// @brief Set the raw barcode message data.
	///
	/// A leading `"^"` indicates the data is in GS1 Application
	/// Identifier syntax; subsequent `"^"` characters represent the
	/// FNC1 non-data separator that delimits variable-length fields.
	///
	/// Inputs beginning with `"^"` are validated against the AI data
	/// syntax rules; on failure this setter throws
	/// gs1encoders::GS1EncoderParameterException, and when the failure
	/// is due to invalid AI content err_markup() returns a marked-up
	/// rendering of the offending AI.
	///
	/// Inputs beginning with `"http://"` or `"https://"` are parsed as
	/// GS1 Digital Link URIs; the corresponding AI element string is
	/// extracted and validated.
	///
	/// EAN/UPC, GS1 DataBar and GS1-128 support a Composite Component.
	/// The Composite Component must be specified in AI syntax. It is
	/// separated from the primary linear component with a `"|"`
	/// character and begins with FNC1 in first position, e.g.
	/// `"^0112345678901231|^10ABC123^11210630"` (linear component
	/// `(01)12345678901231`, composite component `(10)ABC123(11)210630`).
	///
	/// @note For GS1 data it is simpler and less error-prone to specify
	///       the input in human-friendly bracketed AI syntax via
	///       set_ai_data_str(), which inserts FNC1 characters
	///       automatically.
	///
	/// @param v the raw barcode data to be set.
	/// @throws GS1EncoderParameterException if the data is invalid;
	///         err_markup() identifies the offending AI on a linting
	///         failure.
	/// @see data_str()
	/// @see set_ai_data_str()
	/// @see err_markup()
	void set_data_str(const std::string &v) {
		check_param(gs1_encoder_setDataStr(ctx_, v.c_str()));
	}

	/// @brief Get the barcode data input rendered as a bracketed AI
	/// element string.
	///
	/// Returns the data as a human-friendly bracketed AI element string,
	/// for example `"(01)12312312312333(10)ABC123(99)TEST"`. Returns an
	/// empty string when the input data does not contain AI data.
	///
	/// @return the bracketed AI element string, or an empty string when
	///         no AI data is set.
	/// @see set_ai_data_str()
	/// @see data_str()
	std::string ai_data_str() const {
		const char *s = gs1_encoder_getAIdataStr(ctx_);
		return s ? s : std::string();
	}
	/// @brief Set the barcode data input from a bracketed GS1 AI
	/// element string.
	///
	/// The input is provided in human-friendly format **without** FNC1
	/// characters (which are inserted automatically), for example:
	///
	///     (01)12345678901231(10)ABC123(11)210630
	///
	/// This syntax harmonises the input format across all symbologies
	/// — for example `(01)00031234000054` is acceptable for EAN-13,
	/// UPC-A, UPC-E, any variant of the GS1 DataBar family, GS1 QR
	/// Code and GS1 Data Matrix.
	///
	/// The input is validated against the AI data syntax rules at
	/// the point of being set; on failure this setter throws
	/// gs1encoders::GS1EncoderParameterException, and when the failure
	/// is due to invalid AI content err_markup() returns a marked-up
	/// rendering of the offending AI.
	///
	/// Any literal `"("` characters in AI element values must be
	/// escaped as `"\("` to avoid being conflated with the start of
	/// the next AI.
	///
	/// For symbologies that support a composite component (all except
	/// Data Matrix, QR Code and DotCode) the linear and 2D components
	/// can be separated with a `"|"` character, e.g.
	/// `(01)12345678901231|(10)ABC123(11)210630`.
	///
	/// @param v the bracketed AI element string to be parsed.
	/// @throws GS1EncoderParameterException if the data is invalid;
	///         err_markup() identifies the offending AI on a linting
	///         failure.
	/// @see ai_data_str()
	/// @see set_data_str()
	/// @see err_markup()
	void set_ai_data_str(const std::string &v) {
		check_param(gs1_encoder_setAIdataStr(ctx_, v.c_str()));
	}

	/// @brief Render the current AI-based input data as a GS1 Digital
	/// Link URI.
	///
	/// Converts the current AI data into GS1 Digital Link URI form. For
	/// example, the AI data `(01)12345678901231(10)ABC123(11)210630`
	/// with stem `https://id.example.com/stem` might produce
	/// `https://id.example.com/stem/01/12345678901231?10=ABC123&11=210630`.
	///
	/// @param stem URI stem used as a prefix; pass an empty string to
	///             use the GS1 canonical stem (`https://id.gs1.org/`).
	/// @return the GS1 Digital Link URI for the current data.
	/// @throws GS1EncoderDigitalLinkException if the current data
	///         cannot be expressed as a GS1 Digital Link URI (for
	///         example because no AI data is set).
	/// @see set_ai_data_str()
	/// @see set_data_str()
	std::string get_dl_uri(const std::string &stem) const {
		const char *uri = gs1_encoder_getDLuri(
			ctx_, stem.empty() ? nullptr : stem.c_str());
		if (!uri)
			throw GS1EncoderDigitalLinkException(get_err_msg());
		return uri;
	}

	/// @brief Get the scan data string a reader would return for the
	/// current data and symbology.
	///
	/// Returns the string that a scanner would produce when reading a
	/// symbol of the currently selected symbology containing the
	/// current input data. The output is prefixed with the appropriate
	/// AIM Symbology Identifier (e.g. `]Q3...` for GS1 QR Code,
	/// `]C1...` for GS1-128 with composite). Returns an empty string
	/// when no symbology is selected.
	///
	/// @return the scan data string, or an empty string when no
	///         symbology is selected.
	/// @see set_scan_data()
	/// @see sym()
	std::string scan_data() const {
		const char *s = gs1_encoder_getScanData(ctx_);
		return s ? s : std::string();
	}
	/// @brief Process normalised scan data received from a barcode
	/// reader.
	///
	/// Parses scan data (such as the output of a barcode reader) with
	/// AIM Symbology Identifiers enabled, populates the input data
	/// buffer with the message received, and selects a symbology that
	/// is able to carry the received data. Performs syntax checks for
	/// GS1 Digital Link and AI data input.
	///
	/// Example scan data input: `]C1011231231231233310ABC123{GS}99TESTING`
	/// where `{GS}` represents ASCII character 29 (the FNC1 / GS
	/// separator).
	///
	/// A literal `"|"` character may be included in the scan data to
	/// indicate the separation between the first and second messages
	/// that would be transmitted by a reader configured to return the
	/// composite component when reading EAN/UPC symbols.
	///
	/// @note In some instances the symbology determined by this
	///       library will not match that of the image actually scanned:
	///       the AIM Symbology Identifier prefix does not always
	///       uniquely identify the source symbology. For example
	///       GS1-128 Composite symbols share the same identifier as
	///       the GS1 DataBar family, and will therefore be detected as
	///       such.
	///
	/// @param v the normalised scan data string, including the AIM
	///          Symbology Identifier prefix.
	/// @throws GS1EncoderScanDataException if the scan data cannot be
	///         processed (for example because the AIM identifier is
	///         missing or the embedded data fails validation).
	/// @see scan_data()
	/// @see sym()
	void set_scan_data(const std::string &v) {
		if (!gs1_encoder_setScanData(ctx_, v.c_str()))
			throw GS1EncoderScanDataException(get_err_msg());
	}

	/// @brief Get the Human-Readable Interpretation ("HRI") text for
	/// the current input data.
	///
	/// Returns one HRI line per AI in the order they appear in the
	/// data. For composite symbols a separator line `"--"` is inserted
	/// to distinguish the linear and 2D components, for example for
	/// input `^011231231231233310ABC123|^99XYZ(TM) CORP` the HRI is:
	///
	///     (01) 12312312312333
	///     (10) ABC123
	///     --
	///     (99) XYZ(TM) CORP
	///
	/// AI Data Titles from the GS1 General Specifications are
	/// included when include_data_titles_in_hri() is enabled.
	///
	/// @return one std::string per HRI line; empty vector when no AI
	///         data is set.
	/// @see set_include_data_titles_in_hri()
	/// @see set_ai_data_str()
	std::vector<std::string> hri() const {
		char **lines = nullptr;
		int n = gs1_encoder_getHRI(ctx_, &lines);
		std::vector<std::string> result;
		result.reserve(static_cast<size_t>(n));
		for (int i = 0; i < n; i++)
			result.emplace_back(lines[i]);
		return result;
	}

	/// @brief Get the non-numeric (ignored) query parameters from a
	/// GS1 Digital Link URI.
	///
	/// When set_data_str() parses a GS1 Digital Link URI, query
	/// parameters that are not numeric AI keys are ignored for the
	/// purposes of AI extraction. This method returns those ignored
	/// parameters so the application can present which sections of the
	/// URI were not interpreted as AI data.
	///
	/// For example, given the input data
	/// `https://a/01/12312312312333/22/ABC?name=Donald%2dDuck&99=ABC&testing&type=cartoon`,
	/// this method returns:
	///
	///     name=Donald%2dDuck
	///     testing
	///     type=cartoon
	///
	/// The returned strings are not URI-decoded.
	///
	/// @return one std::string per ignored query parameter; empty
	///         vector when no DL URI has been parsed or none were
	///         ignored.
	/// @see set_data_str()
	std::vector<std::string> dl_ignored_query_params() const {
		char **qp = nullptr;
		int n = gs1_encoder_getDLignoredQueryParams(ctx_, &qp);
		std::vector<std::string> result;
		result.reserve(static_cast<size_t>(n));
		for (int i = 0; i < n; i++)
			result.emplace_back(qp[i]);
		return result;
	}

	/// @brief Get the marked-up offending AI from the most recent
	/// linting failure.
	///
	/// When a setter throws gs1encoders::GS1EncoderParameterException
	/// because of a linting failure on AI-based data, this method
	/// returns a marked-up rendering of the offending AI. Where it is
	/// meaningful to identify the offending characters within the AI
	/// value, those characters are surrounded by `"|"` characters;
	/// otherwise the entire AI value is surrounded by `"|"` characters.
	///
	/// Returns an empty string when no linting failure has been
	/// recorded.
	///
	/// @return the marked-up offending AI, or an empty string.
	/// @see set_data_str()
	/// @see set_ai_data_str()
	std::string err_markup() const {
		const char *s = gs1_encoder_getErrMarkup(ctx_);
		return s ? s : std::string();
	}

private:

	static constexpr size_t MSG_BUF_SIZE = 256;

	gs1_encoder *ctx_ = nullptr;
	std::string init_fallback_warning_;

	std::string get_err_msg() const {
		const char *m = gs1_encoder_getErrMsg(ctx_);
		return m ? m : std::string();
	}

	void check_param(bool ok) const {
		if (!ok)
			throw GS1EncoderParameterException(get_err_msg());
	}

};

} /* namespace gs1encoders */

/// @}  // end of cppapi group

#endif /* GS1ENCODERS_HPP */
