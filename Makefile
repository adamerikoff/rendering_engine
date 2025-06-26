# --- Project Configuration ---
PROJECT_NAME = raytracer_app # A more descriptive name for your executable

# --- Compiler & Flags ---
CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -pedantic
# Assuming all headers are reachable relative to SRC_DIR (e.g., #include "camera/camera.h")
INCLUDES = -I$(SRC_DIR)

# SDL2 specific flags - these are correctly defined
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2)

# --- Directories ---
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# List all subdirectories containing source files (.c files)
SRCS_SUBDIRS = \
    $(SRC_DIR)/app \
    $(SRC_DIR)/camera \
    $(SRC_DIR)/canvas \
    $(SRC_DIR)/color \
    $(SRC_DIR)/engine \
    $(SRC_DIR)/light \
    $(SRC_DIR)/object \
    $(SRC_DIR)/scene \
    $(SRC_DIR)/vector

# --- Automatic Source and Object File Discovery ---
# Find all .c files in the specified source subdirectories
C_SRCS = $(foreach dir,$(SRCS_SUBDIRS),$(wildcard $(dir)/*.c))
# Add .c files directly in SRC_DIR if any (e.g., src/main.c)
C_SRCS += $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding .o file names. All object files will be placed in BUILD_DIR.
# `notdir` extracts just the filename (e.g., "main.c" from "src/app/main.c").
# `patsubst` then changes the extension to .o and prepends BUILD_DIR.
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SRCS)))

# Define VPATH to tell Make where to look for source files (the .c files).
# This makes generic rules like 'build/file.o: file.c' work by searching these paths.
VPATH = $(SRCS_SUBDIRS) $(SRC_DIR)

# --- Output Target ---
# This will now correctly resolve to "./bin/raytracer_app"
FULL_TARGET_PATH = $(BIN_DIR)/$(PROJECT_NAME)

# --- Phony Targets ---
# .PHONY targets are not actual files, they are just commands.
.PHONY: all clean run debug

# --- Default Target ---
# 'all' builds the executable
all: $(FULL_TARGET_PATH)

# --- Build Rules ---

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Rule to create the binary output directory if it doesn't exist
# Changed this to 'bin_dir_create' to avoid conflict with the variable name.
# It's an order-only prerequisite for the full target path.
.PHONY: bin_dir_create
bin_dir_create:
	@mkdir -p $(BIN_DIR)

# Link the object files into the executable
# Added 'bin_dir_create' as an order-only prerequisite.
$(FULL_TARGET_PATH): $(OBJS) | $(BUILD_DIR) bin_dir_create
	@echo "Linking $(PROJECT_NAME)..."
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(SDL_LIBS)
	@echo "Build successful: $(FULL_TARGET_PATH)"

# Generic rule to compile .c files into .o files.
# Make uses VPATH to find the corresponding .c file.
# The `| $(BUILD_DIR)` ensures the build directory exists before compilation.
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

# --- Run Rule ---
run: $(FULL_TARGET_PATH)
	@echo "Running $(PROJECT_NAME)..."
	@./$(FULL_TARGET_PATH)

# --- Clean Rule ---
# Removes all generated build files, including object files, dependency files, and the executable.
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR) $(FULL_TARGET_PATH) $(DEP_FILES)
	@echo "Clean complete."

# --- Automatic Dependency Generation ---
# This feature automatically tracks header file dependencies.
# If a header changes, Make will know which .c files need to be recompiled.

# Generate .d files (dependency files) for each .o file.
# These will be placed in the BUILD_DIR alongside the .o files.
DEP_FILES = $(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJS))

# Rule to create dependency files during compilation.
# -MMD: Generates a .d file with dependencies, excluding system headers.
# -MP: Adds a phony target for each header, preventing errors if a header is deleted.
# -MF $@: Specifies the name of the dependency file (the target, e.g., build/main.d).
# -MT $(patsubst %.d,$(BUILD_DIR)/%.o,$@): Specifies the actual target name for the dependency rule (the .o file).
$(BUILD_DIR)/%.d: %.c | $(BUILD_DIR)
	@echo "Generating dependencies for $<..."
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -MMD -MP -MF $@ -MT $(patsubst %.d,$(BUILD_DIR)/%.o,$@) $<

# Include all generated dependency files.
# The hyphen before 'include' tells Make to ignore errors if these files don't exist yet (e.g., on the first build).
-include $(DEP_FILES)