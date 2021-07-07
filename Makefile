# GENERAL VARIABLES
NAME            ?= polecat
VERSION		?= 0.1.9
TARGET          ?= release
DEBUG           := 0
ifeq ($(TARGET),debug)
	DEBUG       := 0
endif

PRETTY_OUTPUT   := 1
STATIC          := 0

# COMMAND VARIABLES
DOXYGEN		:= doxygen
RM              := rm -rf
MKDIR           := mkdir -p
PKGCONFIG       := pkg-config

ifndef HOSTPLATFORM
	HOSTPLATFORM := UNKNOWN
	ifeq ($(findstring Windows,$(OS)),Windows)
		HOSTPLATFORM := WINDOWS
	else
		uname := $(strip $(shell uname -s))
		ifeq ($(findstring Linux,$(uname)),Linux)
			HOSTPLATFORM := LINUX
		else ifeq ($(findstring MINGW,$(uname)),MINGW)
			HOSTPLATFORM := WINDOWS
		else ifeq ($(findstring MSYS,$(uname)),MSYS)
			HOSTPLATFORM := WINDOWS
		endif
	endif
endif

ifndef PLATFORM
	PLATFORM := $(HOSTPLATFORM)
endif

# CROSS COMPILATION
CROSS		:=
OBJ_EXT         := o

ifeq ($(PLATFORM),WINDOWS)
	OUT_EXT     := .exe
	STATIC      := 1
endif

# DIRECTORIES
BUILD_DIR	:= build
BIN_DIR         := $(BUILD_DIR)
OBJ_DIR         := $(BUILD_DIR)/$(TARGET)

SRC_DIR         := src

FILES           := cmake CMakeLists.txt LICENSE Makefile polecat.spec README.md ${SRC_DIR}

# FLAGS
COMMONFLAGS     :=
ifneq ($(STATIC), 0)
	COMMONFLAGS += -static
endif
ifneq ($(DEBUG), 1)
	COMMONFLAGS += -O2
else
	COMMONFLAGS += -ggdb3 -Og
endif
CFLAGS          := $(COMMONFLAGS) `$(PKGCONFIG) --cflags json-c` `$(PKGCONFIG) --cflags libarchive` `$(PKGCONFIG) --cflags libcurl`  
WARNFLAGS       := -Wall -Wextra -pedantic -Wconversion -Wshadow -Wstrict-aliasing  -Winit-self -Wcast-align -Wpointer-arith -Wmissing-declarations -Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized
LIBS            := `$(PKGCONFIG) --libs json-c` `$(PKGCONFIG) --libs libarchive` `$(PKGCONFIG) --libs libcurl` 
BINFLAGS        := $(LIBS)
INCLUDEFLAGS    := 
DEFINES         := -DNAME=\"${NAME}\" -DVERSION=\"${VERSION}\"
ifneq ($(DEBUG), 1)
	DEFINES     += -DNDEBUG
endif

# OBJECT FILES
CC_SRC_FILES    := $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES       := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.$(OBJ_EXT), $(CC_SRC_FILES))


.PHONY: default
default: $(BIN_DIR)/$(NAME)$(OUT_EXT)

.PHONY: all
all: $(BIN_DIR)/$(NAME)$(OUT_EXT) docs

$(BIN_DIR):
	${MKDIR} $@

$(OBJ_DIR):
	${MKDIR} $@

$(BIN_DIR)/$(NAME)$(OUT_EXT): $(OBJ_FILES) | $(BIN_DIR)
	${LINK_STATUS}
	${RECIPE_IF} ${CROSS}${CC}    -o$@ $^ ${CFLAGS} ${WARNFLAGS} ${STATICFLAGS} ${INCLUDEFLAGS} ${DEFINES} ${BINFLAGS} ${RECIPE_RESULT_LINK}


$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.c | $(OBJ_DIR)
	${COMPILE_STATUS}
	${RECIPE_IF} ${CROSS}${CC} -c -o$@ $< ${CFLAGS} ${WARNFLAGS} ${STATICFLAGS} ${INCLUDEFLAGS} ${DEFINES} ${RECIPE_RESULT_COMPILE}

.PHONY: cmake
cmake: $(BIN_DIR)
	cmake -S "." -B "${BIN_DIR}"
	cmake --build "${BIN_DIR}"

.PHONY: clean clean-bin-dir clean-bin clean-obj clean-tar
clean: clean-bin clean-obj clean-tar

clean-bin-dir:
	-${RM} ${BIN_DIR}

clean-bin:
	-${RM} ${BIN_DIR}/${NAME}${OUT_EXT}

clean-obj:
	-${RM} ${OBJ_DIR}

clean-tar:
	-${RM} ${NAME}.tar

.PHONY: docs
docs: Doxyfile
	-${DOXYGEN}

.PHONY: loc
loc:
	-find ${SRC_DIR} -type f | xargs wc -l

.PHONY: loc
tar: ${NAME}.tar

.PHONY: dist
dist: tar

${NAME}.tar:
	${ARCHIVE_STATUS}
	$(RECIPE_IF) tar -cf $@ ${FILES} $(RECIPE_RESULT_ARCHIVE)


ifeq ($(PRETTY_OUTPUT),1)
.SILENT:
RECIPE_IF = if
COMPILE_STATUS = printf "\033[K\033[0mBuilding object \033[1m$@\033[0m...\033[0m\r"
COMPILE_OK = printf "\033[K\033[0;32mBuilt object \033[1;32m$@\033[0;32m \033[0m\n"
COMPILE_FAILED = printf "\033[K\033[0;31mFailed building \033[1;31m$@\033[0;31m from\033[0m \033[1;31m$<\033[0;31m!\033[0m\n"; exit 1
RECIPE_RESULT_COMPILE = ; then $(COMPILE_OK); else $(COMPILE_FAILED); fi
ARCHIVE_STATUS = printf "\033[K\033[0mCreating library archive \033[1m$@\033[0m...\033[0m\r"
ARCHIVE_OK = printf "\033[K\033[0;32mCreated library archive \033[1;32m$@\033[0;32m \033[0m\n"
ARCHIVE_FAILED = printf "\033[K\033[0;31mFailed creating library archive \033[1;31m$@\033[0;31m from\033[0m \033[1;31m$<\033[0;31m!\033[0m\n"; exit 1
RECIPE_RESULT_ARCHIVE = ; then $(ARCHIVE_OK); else $(ARCHIVE_FAILED); fi
LINK_STATUS = printf "\033[K\033[0;0mLinking \033[1m$@\033[0;0m...\033[0m\r"
LINK_OK = printf "\033[K\033[0;32mLinked \033[1;32m$@\033[0;32m \033[0m\n"
LINK_FAILED = printf "\033[K\033[0;31mFailed linking \033[1;31m$@\033[0;31m!\nIf the build options, environment, or system packages have changed, run \'\033[1;31mmake clean\033[0;31m\' and try again.\033[0m\n"; exit 1
RECIPE_RESULT_LINK = ; then $(LINK_OK); else $(LINK_FAILED); fi
else
RECIPE_IF =
BUILD_STARTED =
COMPILE_STATUS =
COMPILE_OK = true
COMPILE_FAILED = false; exit 1
RECIPE_RESULT_COMPILE =
ARCHIVE_STATUS =
ARCHIVE_OK = true
ARCHIVE_FAILED = false; exit 1
RECIPE_RESULT_ARCHIVE =
LINK_STATUS =
LINK_OK = true
LINK_FAILED = false; exit 1
RECIPE_RESULT_LINK =
endif
