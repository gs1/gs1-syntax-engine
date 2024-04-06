#
# GS1 Syntax Engine
#
# @author Copyright (c) 2021-2022 GS1 AISBL.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
#
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

MAJOR := 1
MINOR := 0
VERSION := $(MAJOR).$(MINOR)

NAME = gs1encoders

BUILD_DIR = build

ifeq ($(DEBUG),yes)
DEBUG_CFLAGS = -DPRNT
endif

ifeq ($(MAKECMDGOALS),test)
BUILD_DIR = build-test
UNIT_TEST_CFLAGS = -DUNIT_TESTS
endif

WASM_DIR = ../js-wasm
WASM_JS = $(BUILD_DIR)/gs1encoder-wasm.mjs
WASM_JS_DIST = $(WASM_DIR)/gs1encoder-wasm.mjs
WASM_WASM = $(WASM_JS:.mjs=.wasm)
WASM_WASM_DIST = $(WASM_JS_DIST:.mjs=.wasm)
WASM_OUT_FILES = $(WASM_JS) $(WASM_WASM)
WASM_DIST_FILES = $(WASM_JS_DIST) $(WASM_WASM_DIST)

ifeq ($(MAKECMDGOALS),wasm)
DO_WASM = 1
ifeq ($(JSONLY),yes)
DO_WASM = 0
WASM_OUT_FILES = $(WASM_JS)
WASM_DIST_FILES = $(WASM_JS_DIST)
endif
CC = emcc
CFLAGS_G =
CFLAGS_O = -O3
BUILD_DIR = build-wasm
LDFLAGS_WASM = -s WASM=$(DO_WASM) -s MODULARIZE=1 -s EXPORT_NAME=createGS1encoderModule -s EXPORT_ES6=1 -s EXPORTED_RUNTIME_METHODS='["cwrap","getValue","UTF8ToString"]' -s EXPORTED_FUNCTIONS='["_malloc","_free"]' -s ALLOW_MEMORY_GROWTH=1
SYN_DICT = gs1-syntax-dictionary.txt
else
CFLAGS_G = -g
CFLAGS_O = -O2
endif

ifeq ($(MAKECMDGOALS),clean-test)
BUILD_DIR = build-test
endif

ifeq ($(MAKECMDGOALS),clean-wasm)
BUILD_DIR = build-wasm
endif

ifeq ($(MAKECMDGOALS),fuzzer)
SANITIZE = yes
FUZZER_SAN_OPT = ,fuzzer
FUZZER_CORPUS = corpus
endif


ifeq ($(SANITIZE),yes)

CC=clang
SAN_LDFLAGS = -fuse-ld=lld

ifneq ($(shell uname -s),Darwin)
SAN_CFLAGS = -fsanitize=address,leak,undefined$(FUZZER_SAN_OPT) -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1
SAN_ENV = ASAN_OPTIONS="symbolize=1 detect_leaks=1" LSAN_OPTIONS="fast_unwind_on_malloc=0:malloc_context_size=50" ASAN_SYMBOLIZER_PATH="$(shell which llvm-symbolizer)"
else
# Leak detection is not supported on MacOS builds of LLVM
SAN_CFLAGS = -fsanitize=address,undefined$(FUZZER_SAN_OPT) -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1
SAN_ENV = ASAN_OPTIONS="symbolize=1" LSAN_OPTIONS="fast_unwind_on_malloc=0:malloc_context_size=50" MallocNanoZone=0 ASAN_SYMBOLIZER_PATH="$(shell which llvm-symbolizer)"
endif

endif


ifeq ($(SLOW_TESTS),yes)
SLOW_TESTS_CFLAGS=-DSLOW_TESTS
endif

ifneq ($(shell uname -s),Darwin)
LDFLAGS = -Wl,--as-needed -Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now $(SAN_LDFLAGS)
LDFLAGS_SO = -shared -Wl,-soname,lib$(NAME).so.$(MAJOR)
CFLAGS_FORTIFY = -D_FORTIFY_SOURCE=2
NPROC = nproc
else
LDFLAGS =
LDFLAGS_SO = -shared -Wl,-install_name,lib$(NAME).so.$(MAJOR)
CFLAGS_FORTIFY =
NPROC = sysctl -n hw.ncpu
endif

LDLIBS = -lc
CFLAGS = $(CFLAGS_G) $(CFLAGS_O) $(CFLAGS_FORTIFY) -Wall -Wextra -Wconversion -Wformat -Wformat-security -Wdeclaration-after-statement -pedantic -Werror -MMD -fPIC -DGS1_LINTER_ERR_STR_EN $(SAN_CFLAGS) $(UNIT_TEST_CFLAGS) $(DEBUG_CFLAGS) $(SLOW_TESTS_CFLAGS)

APP = $(BUILD_DIR)/$(NAME).bin
APP_STATIC = $(BUILD_DIR)/$(NAME)-linux.bin

TEST_BIN = $(BUILD_DIR)/$(NAME)-test

LIB_STATIC = $(BUILD_DIR)/lib$(NAME).a
LIB_SHARED = $(BUILD_DIR)/lib$(NAME).so.$(VERSION) $(BUILD_DIR)/lib$(NAME).so $(BUILD_DIR)/lib$(NAME).so.$(MAJOR)

PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib

APP_SRC = gs1encoders-app.c
APP_OBJ = $(BUILD_DIR)/$(APP_SRC:.c=.o)

TEST_SRC = gs1encoders-test.c
TEST_OBJ = $(BUILD_DIR)/$(TEST_SRC:.c=.o)

LINTER_TEST_SRC = syntax/gs1syntaxdictionary-test.c

FUZZER_PREFIX = $(NAME)-fuzzer-
FUZZER_SRCS = gs1encoders-fuzzer-ais.c gs1encoders-fuzzer-dl.c gs1encoders-fuzzer-scandata.c gs1encoders-fuzzer-syn.c
FUZZER_OBJS = $(addprefix $(BUILD_DIR)/, $(FUZZER_SRCS:.c=.o))
FUZZER_BINS = $(BUILD_DIR)/$(FUZZER_PREFIX)ais $(BUILD_DIR)/$(FUZZER_PREFIX)dl $(BUILD_DIR)/$(FUZZER_PREFIX)scandata $(BUILD_DIR)/$(FUZZER_PREFIX)syn

FUZZER_CORPUS_PREFIX = corpus-
FUZZER_CORPUSES = $(FUZZER_CORPUS_PREFIX)ais/ $(FUZZER_CORPUS_PREFIX)dl/ $(FUZZER_CORPUS_PREFIX)scandata/ $(FUZZER_CORPUS_PREFIX)syn/

ALL_SRCS = $(wildcard *.c) $(wildcard syntax/*.c)
SRCS = $(filter-out $(APP_SRC) $(TEST_SRC) $(LINTER_TEST_SRC) $(FUZZER_SRCS), $(ALL_SRCS))
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS = $(addprefix $(BUILD_DIR)/, $(ALL_SRCS:.c=.d)) $(FUZZER_OBJS:.o=.d)


.PHONY: all clean app app-static lib libshared libstatic install install-static install-shared uninstall test clean-test wasm clean-wasm fuzzer docs

default: lib app-static
all: lib app app-static
lib: libshared libstatic
libshared: $(LIB_SHARED)
libstatic: $(LIB_STATIC)
app: $(APP)
app-static: $(APP_STATIC)


$(BUILD_DIR)/syntax/:
	mkdir -p $@

$(BUILD_DIR)/syntax/%.o: syntax/%.c | $(BUILD_DIR)/syntax/
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/:
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)/
	$(CC) $(CFLAGS) -c $< -o $@


#
#  Shared library
#
$(BUILD_DIR)/lib$(NAME).so: $(BUILD_DIR)/lib$(NAME).so.$(VERSION) $(BUILD_DIR)/lib$(NAME).so.$(MAJOR)
	ln -sf $(notdir $<) $@

$(BUILD_DIR)/lib$(NAME).so.$(MAJOR): $(BUILD_DIR)/lib$(NAME).so.$(VERSION)
	ln -sf $(notdir $<) $@

$(BUILD_DIR)/lib$(NAME).so.$(VERSION): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDFLAGS_SO) $(LDLIBS) $^ -o $@


#
#  Static library
#
$(BUILD_DIR)/lib$(NAME).a: $(OBJS)
	$(AR) cr $@ $^
	ranlib $@


#
#  Application built to use shared library
#
$(APP): $(BUILD_DIR)/lib$(NAME).so $(APP_SRC)
	$(CC) $(CFLAGS) $(APP_SRC) -o $@ -L$(BUILD_DIR) -l$(NAME)


#
#  Standalone application
#
$(APP_STATIC): $(OBJS) $(APP_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(APP_OBJ) -o $(APP_STATIC)


#
#  Test binary
#
$(TEST_BIN): $(OBJS) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_OBJ) -o $(TEST_BIN)


#
#  WASM JS
#
$(WASM_JS): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS_WASM) $(LDLIBS) $^ -o $@ --embed-file $(SYN_DICT)


#
#  Fuzzer binaries
#

$(FUZZER_CORPUS_PREFIX)ais/:
	mkdir -p $@

$(BUILD_DIR)/$(FUZZER_PREFIX)ais: $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)ais.o
	$(CC) $(CFLAGS) $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)ais.o -o $(BUILD_DIR)/$(FUZZER_PREFIX)ais


$(FUZZER_CORPUS_PREFIX)dl/:
	mkdir -p $@

$(BUILD_DIR)/$(FUZZER_PREFIX)dl: $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)dl.o
	$(CC) $(CFLAGS) $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)dl.o -o $(BUILD_DIR)/$(FUZZER_PREFIX)dl


$(FUZZER_CORPUS_PREFIX)scandata/:
	mkdir -p $@

$(BUILD_DIR)/$(FUZZER_PREFIX)scandata: $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)scandata.o
	$(CC) $(CFLAGS) $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)scandata.o -o $(BUILD_DIR)/$(FUZZER_PREFIX)scandata


$(FUZZER_CORPUS_PREFIX)syn/:
	mkdir -p $@

$(BUILD_DIR)/$(FUZZER_PREFIX)syn: $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)syn.o
	$(CC) $(CFLAGS) $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)syn.o -o $(BUILD_DIR)/$(FUZZER_PREFIX)syn


#
#  Utility targets
#
wasm: $(WASM_JS)
	@cp -f $(WASM_OUT_FILES) $(WASM_DIR)/

test: $(TEST_BIN)
	$(SAN_ENV) ./$(TEST_BIN) $(TEST)

fuzzer: $(FUZZER_BINS) | $(FUZZER_CORPUSES)
	@echo
	@echo Start fuzzing as follows:
	@echo
	@for sym in $^ ; do \
		echo '$(SAN_ENV)' $$sym -jobs=`$(NPROC)` -workers=`$(NPROC)` $(FUZZER_CORPUS_PREFIX)$${sym##*-} ; echo ; \
	done
	@echo

clean:
	$(RM) $(OBJS) $(APP_OBJ) $(APP) $(APP_STATIC) $(TEST_BIN) $(TEST_OBJ) $(FUZZER_BINS) $(FUZZER_OBJS) $(LIB_STATIC) $(LIB_SHARED) $(DEPS)

clean-test:
	$(RM) $(OBJS) $(APP_OBJ) $(APP) $(APP_STATIC) $(TEST_BIN) $(TEST_OBJ) $(FUZZER_BINS) $(FUZZER_OBJS) $(LIB_STATIC) $(LIB_SHARED) $(DEPS)

clean-wasm:
	$(RM) $(OBJS) $(WASM_JS) $(WASM_WASM) $(WASM_DIST_FILES) $(DEPS)

install: install-static install-shared

install-headers:
	install -d $(DESTDIR)$(PREFIX)/include
	install -m 0644 $(NAME).h $(DESTDIR)$(PREFIX)/include

install-static: libstatic install-headers
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0644 $(BUILD_DIR)/lib$(NAME).a $(DESTDIR)$(LIBDIR)

install-shared: libshared install-headers
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0644 $(BUILD_DIR)/lib$(NAME).so.$(VERSION) $(DESTDIR)$(LIBDIR)
	cd $(DESTDIR)$(LIBDIR) && ln -sf lib$(NAME).so.$(VERSION) lib$(NAME).so
	cd $(DESTDIR)$(LIBDIR) && ln -sf lib$(NAME).so.$(VERSION) lib$(NAME).so.$(MAJOR)
	-ldconfig

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/include/$(NAME).h
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).so.$(VERSION)
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).so.$(MAJOR)
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).so
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).a
	-ldconfig

docs:
	doxygen
	cp docstmp/gs1encoders_8h.html ../../docs/index.html
	cp docstmp/gs1encoders_8h.html ../../docs
	cp docstmp/doxygen.css ../../docs
	cp docstmp/doxygen.svg ../../docs
	cp docstmp/dynsections.js ../../docs
	cp docstmp/jquery.js ../../docs
	cp docstmp/menu.js ../../docs
	cp docstmp/menudata.js ../../docs
	cp docstmp/nav_f.png ../../docs
	cp docstmp/nav_g.png ../../docs
	cp docstmp/nav_h.png ../../docs
	cp docstmp/tabs.css ../../docs
	docker run --rm -v $$(pwd):/src -u $$(id -u):$$(id -g) emscripten/emsdk  make wasm JSONLY=yes
	cp ../js-wasm/example.html ../../docs/demo
	cp ../js-wasm/example.mjs ../../docs/demo
	cp ../js-wasm/gs1encoder.mjs ../../docs/demo
	cp ../js-wasm/gs1encoder-wasm.mjs ../../docs/demo

-include $(DEPS)
