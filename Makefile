# Compiler to use
CC = gcc

# Build directory
BUILD_DIR = build

# Source files
# Add all your .c files here. The wildcard will find them recursively.
SRCS = $(shell find . -name "*.c")

# Object files will be placed in the build directory, mirroring source structure
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

# Executable name
TARGET = $(BUILD_DIR)/main

# SDL2 flags
# CFLAGS for include paths
# LDFLAGS for library paths and linking
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2)

# Compiler flags
# -I. to include current directory for headers
# -Isrc/app for app.h
# -Wall for all warnings
# -g for debugging info
# -std=c17 for C17 standard compliance
CFLAGS = -Wall -g $(SDL_CFLAGS) -I. -Isrc/app -std=c17

# Linker flags
LDFLAGS = $(SDL_LIBS)

# --- Targets ---

.PHONY: build exec run clean

# The 'build' command compiles and links the executable.
build: $(BUILD_DIR) $(TARGET)

# Rule to create the top-level build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Rule to link the executable
$(TARGET): $(OBJS)
	@echo "Linking $@"
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Rule to compile .c files into .o files
# This rule handles files in the current directory (main.c) and subdirectories (src/app/app.c)
# It now includes a command to create the necessary output directory for each object file.
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo "Compiling $< to $@"
	$(CC) $(CFLAGS) -c $< -o $@

# The 'exec' command runs the compiled executable.
exec: $(TARGET)
	@echo "Executing $(TARGET)"
	@$(TARGET)

# The 'run' command first builds, then executes the code.
run: build exec

# Clean up build files
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
