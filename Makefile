# GENERAL VARIABLES
NAME            := polecat
VERSION         := 0.1.0
TARGET          ?= debug
DEBUG           := 0
ifeq ($(TARGET),debug)
    DEBUG       := 1
endif

PRETTY_OUTPUT   ?= 1
STATIC          ?= 0

# CROSS COMPILATION SETUP
CROSS           ?=

# COMMAND VARIABLES
RM              := rm -rf
MKDIR           := mkdir -p
DOXYGEN         := doxygen
PKGCONFIG       := $(CROSS)pkg-config
CURLCONFIG      := $(CROSS)curl-config


ifndef PLATFORM
    PLATFORM := $(HOSTPLATFORM)
endif

# CROSS COMPILATION ADAPTION

# DIRECTORIES
BIN_DIR         := bin
OBJ_DIR         := obj

SRC_DIR         := src

FILES           := $(filter-out $(BIN_DIR) $(OBJ_DIR), $(wildcard *))

# FLAGS
COMMONFLAGS     :=
ifneq ($(STATIC), 0)
    COMMONFLAGS += -static
endif
ifeq ($(DEBUG),0)
    COMMONFLAGS += -O2
else
    COMMONFLAGS += -g
endif
CFLAGS          :=  $(COMMONFLAGS) -Wall `$(PKGCONFIG) json-c --cflags` `$(CURLCONFIG) --cflags`
LDFLAGS         := `$(PKGCONFIG) json-c --libs` `$(CURLCONFIG) --libs`
DEFINES         := -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\"
ifeq ($(DEBUG),1)
    DEFINES     += -DDEBUG
endif

# SOURCE CODE AND OBJECT FILES
CC_SRC_FILES    := $(wildcard   $(SRC_DIR)/*.c) 

OBJ_FILES       := $(patsubst $(SRC_DIR)/%.c,  $(OBJ_DIR)/$(TARGET)/%.o, $(CC_SRC_FILES))


# TARGETS
default: $(BIN_DIR)/$(TARGET)/$(NAME)$(OUT_EXT)

all: $(BIN_DIR)/$(TARGET)/$(NAME)$(OUT_EXT) docs test

$(BIN_DIR)/$(TARGET):
	${MKDIR} $@

$(OBJ_DIR)/$(TARGET):
	${MKDIR} $@

$(BIN_DIR)/$(TARGET)/$(NAME)$(OUT_EXT): $(OBJ_FILES) | $(BIN_DIR)/$(TARGET)
	${LINK_STATUS}
	${RECIPE_IF} ${CROSS}${CC} -o$@ $^ ${CFLAGS} ${DEFINES} ${LDFLAGS} ${RECIPE_RESULT_LINK}

$(OBJ_DIR)/$(TARGET)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)/$(TARGET)
	${COMPILE_STATUS}
	${RECIPE_IF} ${CROSS}${CC} -c -o$@ $< ${CFLAGS} ${DEFINES} ${RECIPE_RESULT_COMPILE}

clean:
	${RM} ${BIN_DIR} ${OBJ_DIR} ${NAME}.tar 2> /dev/null

loc:
	-find ${SRC_DIR} ${INC_DIR} -name '*.cpp' -o -name '*.c' -o -name '*.h' -o -name '*.hpp' -type f | xargs wc -l

tar: ${NAME}.tar

dist: tar

${NAME}.tar:
	${ARCHIVE_STATUS}
	$(RECIPE_IF) tar -cf $@ ${FILES} $(RECIPE_RESULT_ARCHIVE)

.PHONY: default all clean docs loc tar dist


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
