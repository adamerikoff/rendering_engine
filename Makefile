# --- Project Configuration ---
PROJECT_NAME = raytracer_app

# --- Compiler & Flags ---
CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -pedantic
INCLUDES = -I$(SRC_DIR)

# SDL2 specific flags
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2)

# --- Directories ---
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# List all subdirectories containing source files
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

# Find all .c files
C_SRCS = $(foreach dir,$(SRCS_SUBDIRS),$(wildcard $(dir)/*.c))
C_SRCS += $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding .o file names
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SRCS)))

# Define VPATH
VPATH = $(SRCS_SUBDIRS) $(SRC_DIR)

# Output target
FULL_TARGET_PATH = $(BIN_DIR)/$(PROJECT_NAME)

# Phony targets
.PHONY: all clean run debug

# Default target
all: $(FULL_TARGET_PATH)

# --- Build Rules ---

# Create directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Link the object files into the executable
$(FULL_TARGET_PATH): $(OBJS) | $(BIN_DIR)
	@echo "Linking $(PROJECT_NAME)..."
	$(CC) $(CFLAGS) $^ -o $@ $(SDL_LIBS)
	@echo "Build successful: $(FULL_TARGET_PATH)"

# Compile .c files into .o files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -MM -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<

# Include dependencies
-include $(wildcard $(BUILD_DIR)/*.d)

# Run rule
run: $(FULL_TARGET_PATH)
	@echo "Running $(PROJECT_NAME)..."
	@./$(FULL_TARGET_PATH)

# Clean rule
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete."