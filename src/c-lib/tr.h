/**
 * GS1 Barcode Syntax Engine
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

#ifndef TR_H
#define TR_H

#include "enc-private.h"

#ifndef GS1_ENCODERS_ERR_LANG
#define GS1_ENCODERS_ERR_LANG EN
#endif

// "#include "tr_<LANG>.h"
#define INC_STRINGIFY(LANG) #LANG
#define INC_TR_EXPAND(LANG) INC_STRINGIFY(tr_##LANG.h)
#define INC_TR(LANG) INC_TR_EXPAND(LANG)
#include INC_TR(GS1_ENCODERS_ERR_LANG)
#undef INC_TR
#undef INC_TR_EXPAND
#undef INC_STRINGIFY

#define ERR_CONCAT(a,b,c,d) a ## b ## c ## d
#define ERR_TR_EXPAND(l,x) ERR_CONCAT(TR_,l,_,x)
#define ERR_TR(x) ERR_TR_EXPAND(GS1_ENCODERS_ERR_LANG,x)

#define SET_ERR_V(x, ...) do {							\
	ctx->err = gs1_encoder_e##x;						\
	snprintf(ctx->errMsg, sizeof(ctx->errMsg), ERR_TR(x), __VA_ARGS__);	\
} while (0)

#define SET_ERR(x) do {								\
	ctx->err = gs1_encoder_e##x;						\
	snprintf(ctx->errMsg, sizeof(ctx->errMsg), ERR_TR(x));			\
} while (0)

#endif  /* TR_H */
