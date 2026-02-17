#
# GS1 Barcode Syntax Engine
#
# @author Copyright (c) 2021-2026 GS1 AISBL.
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

.DEFAULT_GOAL := default

VERSION := $(shell egrep -m1 '^[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+$(dollar)' ../../CHANGES)

MAJOR = $(word 1,$(subst ., ,$(VERSION)))
MINOR = $(word 2,$(subst ., ,$(VERSION)))
PATCH = $(word 3,$(subst ., ,$(VERSION)))

NAME = gs1encoders

BUILD_DIR = build


ARCH := $(shell $(CC) -dumpmachine)
ifneq (, $(findstring darwin, $(ARCH)))
ARCH_OS := darwin
else ifneq (, $(findstring mingw, $(ARCH)))
ARCH_OS := windows
else ifneq (, $(findstring windows, $(ARCH)))
ARCH_OS := windows
else
ARCH_OS := linux
endif


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
CFLAGS_V =
BUILD_DIR = build-wasm
LDFLAGS_WASM = -s WASM=$(DO_WASM) -s MODULARIZE=1 -s EXPORT_NAME=createGS1encoderModule -s EXPORT_ES6=1 -s EXPORTED_RUNTIME_METHODS='["cwrap","getValue","UTF8ToString"]' -s EXPORTED_FUNCTIONS='["_malloc","_free"]' -s ALLOW_MEMORY_GROWTH=1
SYN_DICT = gs1-syntax-dictionary.txt
else
CFLAGS_G = -g
CFLAGS_O = -O2
CFLAGS_V = -fvisibility=hidden
endif

ifeq ($(MAKECMDGOALS),clean-test)
BUILD_DIR = build-test
endif

ifeq ($(MAKECMDGOALS),clean-fuzzer)
BUILD_DIR = build-fuzzer
endif

ifeq ($(MAKECMDGOALS),clean-wasm)
BUILD_DIR = build-wasm
endif

ifneq ($(filter fuzzer fuzzer-% fuzzer-corpus-seeds,$(MAKECMDGOALS)),)
BUILD_DIR = build-fuzzer
SANITIZE = yes
FUZZER_SAN_OPT = ,fuzzer
FUZZER_CORPUS = corpus
endif


ifeq ($(SANITIZE),yes)

CC=clang
SAN_LDFLAGS = -fuse-ld=lld

ifeq ($(ARCH_OS), darwin)
# Leak detection is not supported on MacOS builds of LLVM
SAN_CFLAGS = -fsanitize=address,undefined$(FUZZER_SAN_OPT) -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1
SAN_ENV = ASAN_OPTIONS="symbolize=1" LSAN_OPTIONS="fast_unwind_on_malloc=0:malloc_context_size=50" MallocNanoZone=0 ASAN_SYMBOLIZER_PATH="$(shell which llvm-symbolizer)"
else
SAN_CFLAGS = -fsanitize=address,leak,undefined$(FUZZER_SAN_OPT) -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1
SAN_ENV = ASAN_OPTIONS="symbolize=1 detect_leaks=1" LSAN_OPTIONS="fast_unwind_on_malloc=0:malloc_context_size=50" ASAN_SYMBOLIZER_PATH="$(shell which llvm-symbolizer)"
endif

endif


ifeq ($(SLOW_TESTS),yes)
SLOW_TESTS_CFLAGS=-DSLOW_TESTS
endif


ifeq ($(ARCH_OS), darwin)
LIB_DYN_SUFFIX = dylib
LIB_STATIC_SUFFIX = a
BIN_SUFFIX = bin
LDLIBS = -lc
LDFLAGS =
LDFLAGS_SO = -shared -Wl,-install_name,lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR)
LDFLAGS_APP_STATIC =
CFLAGS_FORTIFY =
NPROC = sysctl -n hw.ncpu
else ifeq ($(ARCH_OS), windows)
LIB_DYN_SUFFIX = dll
LIB_STATIC_SUFFIX = a
BIN_SUFFIX = exe
LDLIBS =
LDFLAGS = -s -Wl,--as-needed -Wl,-Bsymbolic-functions $(SAN_LDFLAGS)
LDFLAGS_SO = -shared -Wl,-soname,lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR)
LDFLAGS_APP_STATIC = -s
CFLAGS_FORTIFY = -D_FORTIFY_SOURCE=2
NPROC = nproc
else
LIB_DYN_SUFFIX = so
LIB_STATIC_SUFFIX = a
BIN_SUFFIX = bin
LDLIBS = -lc
LDFLAGS = -s -Wl,--as-needed -Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now $(SAN_LDFLAGS)
LDFLAGS_SO = -shared -Wl,-soname,lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR)
LDFLAGS_APP_STATIC = -s
CFLAGS_FORTIFY = -D_FORTIFY_SOURCE=2
NPROC = nproc
endif

CFLAGS = $(CFLAGS_G) $(CFLAGS_O) $(CFLAGS_FORTIFY) $(CFLAGS_V) -Wall -Wextra -Wconversion -Wformat=2 -Wshadow -Wdeclaration-after-statement -pedantic -Werror -MMD -fPIC -DGS1_LINTER_ERR_STR_EN $(SAN_CFLAGS) $(UNIT_TEST_CFLAGS) $(DEBUG_CFLAGS) $(SLOW_TESTS_CFLAGS)

APP = $(BUILD_DIR)/$(NAME).$(BIN_SUFFIX)
APP_STATIC = $(BUILD_DIR)/$(NAME)-static.$(BIN_SUFFIX)

TEST_BIN = $(BUILD_DIR)/$(NAME)-test.$(BIN_SUFFIX)

LIB_STATIC = $(BUILD_DIR)/lib$(NAME).$(LIB_STATIC_SUFFIX)

ifeq ($(ARCH_OS), windows)
LIB_SHARED = $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX)
else
LIB_SHARED = $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(VERSION) $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX) $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR)
endif

PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib

APP_SRC = gs1encoders-app.c
APP_OBJ = $(BUILD_DIR)/$(APP_SRC:.c=.o)

TEST_SRC = gs1encoders-test.c
TEST_OBJ = $(BUILD_DIR)/$(TEST_SRC:.c=.o)

LINTER_TEST_SRC = syntax/gs1syntaxdictionary-test.c

FUZZER_PREFIX = $(NAME)-fuzzer-
FUZZER_SRCS = gs1encoders-fuzzer-ais.c gs1encoders-fuzzer-data.c gs1encoders-fuzzer-dl.c gs1encoders-fuzzer-scandata.c gs1encoders-fuzzer-syn.c
FUZZER_OBJS = $(addprefix $(BUILD_DIR)/, $(FUZZER_SRCS:.c=.o))
FUZZER_NAMES = $(patsubst gs1encoders-fuzzer-%.c,%,$(FUZZER_SRCS))
FUZZER_BINS = $(addprefix $(BUILD_DIR)/$(FUZZER_PREFIX),$(FUZZER_NAMES))

FUZZER_CORPUS_PREFIX = corpus-
FUZZER_CORPUSES = $(addsuffix /,$(addprefix $(FUZZER_CORPUS_PREFIX),$(FUZZER_NAMES)))
FUZZER_SEED_SOURCES = *test*.c dl.c ai.c scandata.c syn.c

ALL_SRCS = $(wildcard *.c) $(wildcard syntax/*.c)
SRCS = $(filter-out $(APP_SRC) $(TEST_SRC) $(LINTER_TEST_SRC) $(FUZZER_SRCS), $(ALL_SRCS))
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS = $(addprefix $(BUILD_DIR)/, $(ALL_SRCS:.c=.d)) $(FUZZER_OBJS:.o=.d)


#
#  General goals
#
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
ifeq ($(ARCH_OS), windows)

$(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDFLAGS_SO) $(LDLIBS) $^ -o $@

else

$(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX): $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(VERSION) $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR)
	ln -sf $(notdir $<) $@

$(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR): $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(VERSION)
	ln -sf $(notdir $<) $@

$(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(VERSION): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDFLAGS_SO) $(LDLIBS) $^ -o $@

endif


#
#  Static library
#
$(BUILD_DIR)/lib$(NAME).$(LIB_STATIC_SUFFIX): $(OBJS)
	$(AR) cr $@ $^
	ranlib $@


#
#  Application built to use shared library
#
$(APP): $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX) $(APP_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS_APP_STATIC) $(APP_SRC) -o $@ -L$(BUILD_DIR) -l$(NAME)


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

$(FUZZER_CORPUS_PREFIX)data/:
	mkdir -p $@

$(BUILD_DIR)/$(FUZZER_PREFIX)data: $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)data.o
	$(CC) $(CFLAGS) $(OBJS) $(BUILD_DIR)/$(FUZZER_PREFIX)data.o -o $(BUILD_DIR)/$(FUZZER_PREFIX)data

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
.PHONY: default
default: lib app-static

.PHONY: all
all: lib app app-static

.PHONY: lib
lib: libshared libstatic

.PHONY: libshared
libshared: $(LIB_SHARED)

.PHONY: libstatic
libstatic: $(LIB_STATIC)

.PHONY: app
app: $(APP)

.PHONY: app-static
app-static: $(APP_STATIC)

.PHONY: .wasm
wasm: $(WASM_JS)
	@cp -f $(WASM_OUT_FILES) $(WASM_DIR)/

.PHONY: test
test: $(TEST_BIN)
	$(SAN_ENV) ./$(TEST_BIN) $(TEST)

# Generate seeds for a specific fuzzer (internal target)
.PHONY: fuzzer-corpus-seeds-%
fuzzer-corpus-seeds-%: $(BUILD_DIR)/$(FUZZER_PREFIX)% | $(FUZZER_CORPUS_PREFIX)%/
	@corpus_dir="$(FUZZER_CORPUS_PREFIX)$*" && \
	if [ -z "$$(ls -A $$corpus_dir 2>/dev/null)" ]; then \
		temp_dir="$$(mktemp -d)" && \
		awk ' \
		{ \
			# Extract all quoted strings from each line \
			while (match($$0, /"[^"\\\\]*(\\\.[^"\\\\]*)*"/)) { \
				str = substr($$0, RSTART, RLENGTH); \
				$$0 = substr($$0, RSTART + RLENGTH); \
				# Remove surrounding quotes \
				gsub(/^"|"$$/, "", str); \
				# Unescape common sequences \
				gsub(/\\\\"/, "\"", str); gsub(/\\\\\\\\/, "\\\\", str); \
				# Skip empty strings \
				if (length(str) > 0 && !seen[str]++) { \
					filename = sprintf("'"$$temp_dir"'/seeds-%05d", ++count); \
					printf "%s", str > filename; \
					close(filename); \
				} \
			} \
		}' $(FUZZER_SEED_SOURCES) 2>/dev/null && \
		if [ -n "$$(ls $$temp_dir/seeds-* 2>/dev/null)" ]; then \
			echo "Merging extracted strings into $$corpus_dir" && \
			$(SAN_ENV) $(BUILD_DIR)/$(FUZZER_PREFIX)$* -merge=1 "$$corpus_dir" "$$temp_dir" >/dev/null 2>&1; \
		fi && \
		rm -rf "$$temp_dir"; \
	else \
		echo "Corpus $$corpus_dir already exists, skipping seed generation"; \
	fi

.PHONY: fuzzer-corpus-seeds
fuzzer-corpus-seeds: $(addprefix fuzzer-corpus-seeds-,$(FUZZER_NAMES))

.PHONY: $(addprefix fuzzer-,$(FUZZER_NAMES))
$(addprefix fuzzer-,$(FUZZER_NAMES)): fuzzer-%: $(BUILD_DIR)/$(FUZZER_PREFIX)% fuzzer-corpus-seeds-% | $(FUZZER_CORPUS_PREFIX)%/
	@echo
	@echo "Start $* fuzzer as follows:"
	@echo
	@echo '$(SAN_ENV)' $(BUILD_DIR)/$(FUZZER_PREFIX)$* -jobs=`$(NPROC)` -workers=`$(NPROC)` $(FUZZER_CORPUS_PREFIX)$*
	@echo

.PHONY: fuzzer
fuzzer: $(FUZZER_BINS) | $(FUZZER_CORPUSES) fuzzer-corpus-seeds
	@echo
	@echo "Start fuzzing as follows:"
	@echo
	@for sym in $^ ; do \
		echo '$(SAN_ENV)' $$sym -jobs=`$(NPROC)` -workers=`$(NPROC)` $(FUZZER_CORPUS_PREFIX)$${sym##*-} ; echo ; \
	done
	@echo

.PHONY: clean
clean:
	$(RM) $(OBJS) $(APP_OBJ) $(APP) $(APP_STATIC) $(TEST_BIN) $(TEST_OBJ) $(FUZZER_BINS) $(FUZZER_OBJS) $(LIB_STATIC) $(LIB_SHARED) $(DEPS)

.PHONY: clean-test
clean-test:
	$(RM) $(OBJS) $(APP_OBJ) $(APP) $(APP_STATIC) $(TEST_BIN) $(TEST_OBJ) $(FUZZER_BINS) $(FUZZER_OBJS) $(LIB_STATIC) $(LIB_SHARED) $(DEPS)

.PHONY: clean-fuzzer
clean-fuzzer:
	$(RM) $(OBJS) $(APP_OBJ) $(APP) $(APP_STATIC) $(TEST_BIN) $(TEST_OBJ) $(FUZZER_BINS) $(FUZZER_OBJS) $(LIB_STATIC) $(LIB_SHARED) $(DEPS)

.PHONY: clean-fuzzer-corpus
clean-fuzzer-corpus:
	$(RM) -r $(FUZZER_CORPUSES)

.PHONY: clean-wasm
clean-wasm:
	$(RM) $(OBJS) $(WASM_JS) $(WASM_WASM) $(WASM_DIST_FILES) $(DEPS)

.PHONY: install
install: install-static install-shared

.PHONY: .install-headers
install-headers:
	install -d $(DESTDIR)$(PREFIX)/include
	install -m 0644 $(NAME).h $(DESTDIR)$(PREFIX)/include

.PHONY: install-static
install-static: libstatic install-headers
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0644 $(BUILD_DIR)/lib$(NAME).$(LIB_STATIC_SUFFIX) $(DESTDIR)$(LIBDIR)

.PHONY: install-shared
install-shared: libshared install-headers
	install -d $(DESTDIR)$(LIBDIR)
	install -m 0644 $(BUILD_DIR)/lib$(NAME).$(LIB_DYN_SUFFIX).$(VERSION) $(DESTDIR)$(LIBDIR)
	-ldconfig

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/include/$(NAME).h
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).$(LIB_DYN_SUFFIX).$(VERSION)
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).$(LIB_DYN_SUFFIX).$(MAJOR)
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).$(LIB_DYN_SUFFIX)
	$(RM) $(DESTDIR)$(PREFIX)/lib/lib$(NAME).$(LIB_STATIC_SUFFIX)
	-ldconfig

.PHONY: docs-clib
docs-clib:
	../../maintenance/docker/c-docs/docker-build.sh
	cp ../../maintenance/docker/c-docs/build/index.html ../../docs/
	cp ../../maintenance/docker/c-docs/build/gs1encoders_8h.html ../../docs/
	cp ../../maintenance/docker/c-docs/build/structgs1__encoder__init__opts.html ../../docs/
	cp ../../maintenance/docker/c-docs/build/input_output_buffers.svg ../../docs/
	cp ../../maintenance/docker/c-docs/build/*.css ../../docs/
	cp ../../maintenance/docker/c-docs/build/*.js ../../docs/
	cp ../../maintenance/docker/c-docs/build/*.svg ../../docs/
	cp ../../maintenance/docker/c-docs/build/*.png ../../docs/

.PHONY: docs-demo
docs-demo:
	(cd ../.. && docker run --rm -v $$(pwd):/src -u $$(id -u):$$(id -g) emscripten/emsdk  make -C src/c-lib wasm JSONLY=yes)
	mkdir -p ../../docs/demo
	cp ../js-wasm/example.html ../../docs/demo
	cp ../js-wasm/example.mjs ../../docs/demo
	cp ../js-wasm/gs1encoder.mjs ../../docs/demo
	cp ../js-wasm/gs1encoder-wasm.mjs ../../docs/demo

.PHONY: docs-cs
docs-cs:
	../../maintenance/docker/cs-docs/docker-build.sh
	cp ../../docs/cs/GS1.Encoders.html ../../docs/cs/index.html

.PHONY: docs-java
docs-java:
	../../maintenance/docker/java-docs/docker-build.sh
	cat ../../maintenance/docker/java-docs/custom.css >> ../../docs/java/stylesheet.css
	cp ../../maintenance/docker/java-docs/input_output_buffers.svg ../../docs/java/

.PHONY: docs-swift
docs-swift:
	../../maintenance/docker/swift-docs/docker-build.sh
	cp ../../maintenance/docker/swift-docs/input_output_buffers.svg ../../docs/swift/

.PHONY: docs-js-wasm
docs-js-wasm:
	../../maintenance/docker/js-wasm-docs/docker-build.sh
	cp ../../docs/js-wasm/module-gs1encoder.html ../../docs/js-wasm/index.html

.PHONY: docs
docs: docs-clib docs-cs docs-java docs-swift docs-js-wasm docs-demo

.PHONY: copyright
copyright:
	git ls-files -z .. | xargs -0 -I{} sed -i --follow-symlinks -e "s/Copyright (c) \([[:digit:]]\{4\}\)\(-[[:digit:]]\{4\}\)\{0,1\} GS1 AISBL/Copyright (c) \1-$$(date +'%Y') GS1 AISBL/" {}

.PHONY: setversion
setversion:
	(cd ../js-wasm && npm version --allow-same-version $(VERSION))
	sed -i 's/name="Implementation-Version" value="[[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+"/name="Implementation-Version" value="$(VERSION)"/' ../java/build.xml

.PHONY: superlinter
superlinter:
	docker run --rm \
		-e RUN_LOCAL=true \
		-e VALIDATE_ALL_CODEBASE=true \
		-e DEFAULT_BRANCH=master \
		-e IGNORE_GITIGNORED_FILES=true \
		-e LINTER_RULES_PATH=.github/linters \
		-e 'FILTER_REGEX_EXCLUDE=(src/c-lib/syntax/|src/c-lib/aitable\.|docs/|maintenance/docker/.*/build/|node_modules/|\.gradle/|Pods/|\.build/|src/c-lib/build|src/android/gradlew|corpus-|src/android/app/src/main/cpp/gs1encoders(/|$$)|src/swift/Sources/CGS1Encoders/(c-lib|include)(/|$$))' \
		$(shell grep -v '^\#' ../../.github/linters/superlinter-disabled.env | grep '=' | sed 's/^/-e /') \
		-v `pwd`/../..:/tmp/lint \
		ghcr.io/super-linter/super-linter:v8

.PHONY: syncsyntaxdict
syncsyntaxdict:
	cp ../../../gs1-syntax-dictionary/gs1-syntax-dictionary.txt .
	cp ../../../gs1-syntax-dictionary/src/acutest.h syntax/
	cp ../../../gs1-syntax-dictionary/src/gs1syntaxdictionary.c syntax/
	cp ../../../gs1-syntax-dictionary/src/gs1syntaxdictionary.h syntax/
	cp ../../../gs1-syntax-dictionary/src/gs1syntaxdictionary-utils.h syntax/
	cp ../../../gs1-syntax-dictionary/src/lint_*.c syntax/
	cp ../../../gs1-syntax-dictionary/src/unittest.h syntax/
	mv aitable.inc aitable.$$(date +%s)
	cat gs1-syntax-dictionary.txt | ./build-embedded-ai-table.pl > aitable.inc.new && mv aitable.inc.new aitable.inc


-include $(DEPS)
