# Makefile
###############################################################
# make              - Create bin/main or bin/main.exe
# make clean        - Clear the obj, dep, and executable files
# make PLATFORM=web - Compile for web
###############################################################
OPTIONS =  # !!! Must 'make clean' after changing an option !!!
OPTIONS += RES=256
###############################################################

SHELL := /usr/bin/bash
PLATFORM ?= native

OBJDIR = obj
INCDIR = inc
SRCDIR = src
BINDIR = bin

MAIN = main

# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    RM = rm -f
    MKDIR = mkdir -p
    EXE = 
    SLASH = /
    TOUCH = touch
else
    RM = cmd.exe /C del /Q
    TOUCH = cmd.exe /C echo.>
    MKDIR = mkdir
    EXE = .exe
    SLASH = \\
endif

# Raylib
RAYLIB_INC = raylib/include

RAYLIB_LIB_NATIVE = raylib/lib
RAYLIB_LIB_WEB    = raylib/web
SHELL_FILE        = raylib/web/shell.html
ICO_FILE          = raylib/web/favicon.ico

INC_LOCATIONS = $(shell find "$(INCDIR)" "$(SRCDIR)" "$(RAYLIB_INC)" -type d)
INC_FLAGS     = $(addprefix -I,$(INC_LOCATIONS))

ifeq ($(PLATFORM),web)
	CC = emcc
    RAYLIB_LIB = $(RAYLIB_LIB_WEB)
	RAYLIB_FLAGS = -L$(RAYLIB_LIB) -lraylib -sUSE_GLFW=3 -sEXPORTED_RUNTIME_METHODS=ccall \
				   -sSTACK_SIZE=1048576 -sALLOW_MEMORY_GROWTH=1 --shell-file $(SHELL_FILE)
	CFLAGS = $(INC_FLAGS) -std=c99 -O3 -MMD -MP -DPLATFORM_WEB
	TARGET = index.html
else
	CC = gcc
    RAYLIB_LIB = $(RAYLIB_LIB_NATIVE)
    RAYLIB_FLAGS = -L$(RAYLIB_LIB) -lraylib \
                   -lopengl32 -lgdi32 -lwinmm
	CFLAGS = $(INC_FLAGS) -std=c99 -O2 -MMD -MP
	TARGET = main$(EXE)
endif

# Recursive source file discovery
srcs      = $(shell find "$(SRCDIR)" -name "*.c")
src_objs  = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(srcs))
deps      = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.d, $(srcs))

# Compilation options
CFLAGS += $(addprefix -D,$(OPTIONS))

all: $(BINDIR)/$(TARGET)


$(BINDIR)/$(TARGET): $(src_objs)
	@echo Linking $@
	@$(CC) $^ -o $@ -s $(RAYLIB_FLAGS)
ifeq ($(PLATFORM),web)
	@cp $(ICO_FILE) $(BINDIR)
endif


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo Compiling $< to $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(deps)

.PHONY: clean all silent

silent: ;

clean:
ifeq ($(UNAME_S),Linux)
	$(RM) -f $(src_objs) $(deps) $(BINDIR)/*
	$(TOUCH) $(OBJDIR)/.gitkeep $(BINDIR)/.gitkeep
else
	$(RM) $(subst /,$(SLASH),$(src_objs)) \
	      $(subst /,$(SLASH),$(deps)) \
	      $(BINDIR)$(SLASH)* 2>nul || exit 0
	$(TOUCH) $(OBJDIR)$(SLASH).gitkeep
	$(TOUCH) $(BINDIR)$(SLASH).gitkeep
endif