CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra -std=c11 -g
LDFLAGS = -lncurses -lm

SRC_DIR = src
INCLUDE_DIR = include

TARGET = riscv_sim

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

YELLOW = \033[1;33m
GREEN = \033[1;32m
RED = \033[1;31m
NC = \033[0m

all: $(TARGET)

build: clean all
	@rm -f $(SRC_DIR)/*.o
	
$(TARGET): $(OBJS)
	@printf "$(YELLOW)[Linking] %s$(NC)\n" "$@"
	@$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@printf "$(GREEN)[Build Complete]$(NC)\n"

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/*.h
	@printf "$(YELLOW)[Compiling] %s$(NC)\n" "$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@printf "$(RED)[Cleaning]$(NC)\n"
	@rm -f $(SRC_DIR)/*.o $(TARGET)
	@printf "$(GREEN)[Clean Complete]$(NC)\n"

debug: CFLAGS += -g -DDEBUG
debug: clean all

release: CFLAGS += -O2
release: clean all

.PHONY: all clean debug release build
