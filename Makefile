# --- Project Configuration ---
RASTER_PROJECT_NAME = rasterizing_engine
RAY_PROJECT_NAME = ray_casting_engine # Keep for future reference

# --- Compiler & Flags ---
CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -pedantic

# SDL2 specific flags
# -I$(RASTER_SRC_DIR)/app : Tells the compiler to look for header files (like app.h) in this directory.
SDL_CFLAGS = $(shell pkg-config --cflags sdl2) -I$(RASTER_SRC_DIR)/app
# -lm : Links the math library (required for fabs, round functions in app.c).
SDL_LIBS = $(shell pkg-config --libs sdl2) -lm

# --- Directories ---
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# --- Rasterizing Engine Configuration ---
RASTER_SRC_DIR = $(SRC_DIR)/rasterizing_engine
RASTER_SRCS_SUBDIRS = \
    $(RASTER_SRC_DIR)/app \
	$(RASTER_SRC_DIR)/geometry

# Collect all .c source files for the rasterizing engine, including those in subdirectories.
RASTER_C_SRCS = $(wildcard $(RASTER_SRC_DIR)/*.c)
RASTER_C_SRCS += $(foreach dir,$(RASTER_SRCS_SUBDIRS),$(wildcard $(dir)/*.c))

# Generate the paths for the object files.
# Example: src/rasterizing_engine/app/app.c -> build/rasterizing_engine/app/app.o
RASTER_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(RASTER_C_SRCS))

RASTER_TARGET = $(BIN_DIR)/$(RASTER_PROJECT_NAME)

# --- Ray Casting Engine Configuration (Kept for completeness, adjust if needed) ---
RAY_SRC_DIR = $(SRC_DIR)/ray_casting_engine
RAY_SRCS_SUBDIRS = \
    $(RAY_SRC_DIR)/app \
    $(RAY_SRC_DIR)/camera \
    $(RAY_SRC_DIR)/canvas \
    $(RAY_SRC_DIR)/color \
    $(RAY_SRC_DIR)/engine \
    $(RAY_SRC_DIR)/light \
    $(RAY_SRC_DIR)/object \
    $(RAY_SRC_DIR)/scene \
    $(RAY_SRC_DIR)/vector

RAY_C_SRCS = $(foreach dir,$(RAY_SRCS_SUBDIRS),$(wildcard $(dir)/*.c))
RAY_C_SRCS += $(wildcard $(RAY_SRC_DIR)/*.c)
RAY_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(RAY_C_SRCS)) # Changed to use SRC_DIR for patsubst
RAY_TARGET = $(BIN_DIR)/$(RAY_PROJECT_NAME)


# --- Phony Targets ---
.PHONY: all build_raster build_ray run_raster run_ray clean

all: build_raster # Default target if 'make' is run without arguments

# --- Build Targets ---

# Build Rasterizing Engine executable
build_raster: $(RASTER_TARGET)
	@echo "Rasterizing Engine build process complete."

# Build Ray Casting Engine (if you implement it later)
build_ray: $(RAY_TARGET)
	@echo "Ray Casting Engine build process complete."

# --- Build Rules for Rasterizing Engine and Ray Casting Engine ---

# Rule to link the rasterizing engine executable
$(RASTER_TARGET): $(RASTER_OBJS) | $(BIN_DIR)
	@echo "Linking $(RASTER_PROJECT_NAME)..."
	$(CC) $(CFLAGS) $^ -o $@ $(SDL_LIBS)
	@echo "Build successful: $(RASTER_TARGET)"

# Rule to link the ray casting engine executable
$(RAY_TARGET): $(RAY_OBJS) | $(BIN_DIR)
	@echo "Linking $(RAY_PROJECT_NAME)..."
	$(CC) $(CFLAGS) $^ -o $@ $(SDL_LIBS) # Assuming SDL_LIBS are also needed for ray casting, adjust if not
	@echo "Build successful: $(RAY_TARGET)"

# Generic rule for compiling .c files into .o files.
# This pattern matches any .c file under $(SRC_DIR) and outputs the .o file
# under $(BUILD_DIR), preserving the relative path.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) # Ensure the target directory exists for this specific object file
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) $(SDL_CFLAGS) -MM -MP -MT $@ -MF $(@:.o=.d) $<


# --- General Build Directories ---
$(BIN_DIR):
	@mkdir -p $@

# --- Run Targets ---

# Run Rasterizing Engine
run_raster: build_raster
	@echo "Running $(RASTER_PROJECT_NAME)..."
	@./$(RASTER_TARGET)

# Run Ray Casting Engine
run_ray: build_ray
	@echo "Running $(RAY_PROJECT_NAME)..."
	@./$(RAY_TARGET)

# --- Clean Rule ---
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete."

# Include dependency files generated by -MM -MP flags.
# This automatically tracks header dependencies.
-include $(wildcard $(BUILD_DIR)/*/*.d)
