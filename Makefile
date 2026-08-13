CC       ?= gcc
CFLAGS   ?= -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE -Iinclude
#LDFLAGS  ?= -pthread

# Project directories
SRC_DIR  := src
BUILD_DIR:= build
BIN_DIR  := bin

# exe file
TARGET   := $(BIN_DIR)/http_server

# Automating searching files
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS     := $(OBJS:.o=.d)

# Colors for better UX in the terminal
COLOR_RESET  := \033[0m
COLOR_GREEN  := \033[1;32m
COLOR_YELLOW := \033[1;33m
COLOR_BLUE   := \033[1;34m

.PHONY: all clean debug release valgrind run help

# Domyślny cel (Release)
all: release

CFLAGS += -MMD -MP

# Prod version with optimizations
release: CFLAGS += -O3
release: $(TARGET)

# Dev version with sanitanizers
debug: CFLAGS += -g -O0 -DDEBUG -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: $(TARGET)

# Linking
$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "$(COLOR_BLUE)[LINKING]$(COLOR_RESET) $@"
	@$(CC) $(OBJS) $(LDFLAGS) -o $@
	@echo "$(COLOR_GREEN)[BUILD SUCCESS]$(COLOR_RESET) exe file: $@"

# Compiling .c files to .o files in build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "$(COLOR_YELLOW)[COMPILING]$(COLOR_RESET) $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Creating essential directories
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# Cleaning
clean:
	@echo "$(COLOR_YELLOW)[CLEANING]$(COLOR_RESET) Cleaning compiled files"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$(COLOR_GREEN)[CLEAN COMPLETE]$(COLOR_RESET)"

# Memory leaks tests with valgrind
valgrind: CFLAGS += -g -O0 -DDEBUG
valgrind: $(TARGET)
	@echo "$(COLOR_BLUE)[VALGRIND]$(COLOR_RESET) Starting server with Valgrind control"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) 8080 www

run: release
	@echo "$(COLOR_GREEN)[RUNNING]$(COLOR_RESET) Starting server at port 8080"
	./$(TARGET) 8080 www

-include $(DEPS)

help:
	@echo "Available commands:"
	@echo "  make (or make release)  Compiles an optimized version (-O3)"
	@echo "  make debug              Compiles with debug symbols and AddressSanitizer"
	@echo "  make run                Compiles and starts the server on port 8080"
	@echo "  make valgrind           Starts the server in Valgrind"
	@echo "  make clean              Removes binary files and objects (.o)"
