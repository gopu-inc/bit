# =====================================================
#                GOD TIER MAKEFILE
# =====================================================

NAME      := bit

SRC_DIR   := src
OBJ_DIR   := obj
BIN_DIR   := bin
INC_DIR   := include

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# =====================================================
#                 COMPILERS
# =====================================================

CC        := gcc
WIN_CC    := i686-w64-mingw32-gcc
ARM_CC    := aarch64-linux-gnu-gcc

# =====================================================
#                 FLAGS GOD MODE
# =====================================================

BASE_FLAGS := -std=c11 \
              -Wall \
              -Wextra \
              -Wpedantic \
              -Wshadow \
              -Wformat=2 \
              -Wconversion \
              -Wimplicit-fallthrough \
              -Wnull-dereference \
              -Wdouble-promotion \
              -Wcast-align \
              -Wstrict-prototypes \
              -Wmissing-prototypes \
              -Wwrite-strings \
              -Wstack-protector \
              -fstack-protector-strong \
              -fstack-clash-protection \
              -fcf-protection \
              -D_FORTIFY_SOURCE=3 \
              -D_DEFAULT_SOURCE \
              -Winvalid-pch \
              -fno-omit-frame-pointer \
              -fPIC \
              -pipe \
              -I$(INC_DIR)

# ================= PERFORMANCE =================

RELEASE_FLAGS := -O3 \
                 -march=native \
                 -mtune=native \
                 -flto \
                 -ffast-math \
                 -funroll-loops \
                 -falign-functions=32 \
                 -falign-loops=32 \
                 -falign-jumps=32 \
                 -fipa-pta

DEBUG_FLAGS := -O0 -g3 -ggdb3

CFLAGS := $(BASE_FLAGS) $(RELEASE_FLAGS)

# ================= LINK =================

LDFLAGS := -lcurl -flto

STRIP := strip

# =====================================================
#                 COLORS
# =====================================================

GREEN  := \033[32m
CYAN   := \033[36m
YELLOW := \033[33m
RESET  := \033[0m

# =====================================================
#                 DEFAULT BUILD
# =====================================================

all: release

release: CFLAGS := $(BASE_FLAGS) $(RELEASE_FLAGS)
release: build
	@$(STRIP) $(BIN_DIR)/$(NAME)
	@printf "$(GREEN)✔ RELEASE READY$(RESET)\n"

debug: CFLAGS := $(BASE_FLAGS) $(DEBUG_FLAGS)
debug: build
	@printf "$(YELLOW)✔ DEBUG BUILD READY$(RESET)\n"

build: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@printf "$(CYAN)Linking...$(RESET)\n"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@printf "$(YELLOW)Compiling $<$(RESET)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

# =====================================================
#                 WINDOWS (STATIC)
# =====================================================

WIN_CURL := cross/win64/curl

WIN_LIBS := -lcurl -lssl -lcrypto -lz -lssh2 -lpsl \
            -lws2_32 -lcrypt32 -lbcrypt

windows:
	@printf "$(CYAN)Building Windows STATIC...$(RESET)\n"
	@mkdir -p $(BIN_DIR)

	$(WIN_CC) $(BASE_FLAGS) -O3 -flto \
	$(SRCS) \
	-I$(WIN_CURL)/include \
	-L$(WIN_CURL)/lib \
	-o $(BIN_DIR)/$(NAME)-win64.exe \
	$(WIN_LIBS) \
	-static \
	-s

	@printf "$(GREEN)✔ Windows build done$(RESET)\n"

# =====================================================
#                 LINUX ARM
# =====================================================

linux-arm:
	@printf "$(CYAN)Building Linux ARM64...$(RESET)\n"
	@mkdir -p $(BIN_DIR)

	$(ARM_CC) $(CFLAGS) $(SRCS) \
	-o $(BIN_DIR)/$(NAME)-arm64 \
	$(LDFLAGS) \
	-static

# =====================================================
#                 MACOS
# =====================================================

macos:
	@printf "$(CYAN)Building macOS Universal...$(RESET)\n"
	clang $(CFLAGS) \
	-arch x86_64 \
	-arch arm64 \
	$(SRCS) \
	-o $(BIN_DIR)/$(NAME)-macos \
	$(LDFLAGS)

# =====================================================
#                 POWER TARGET
# =====================================================

platforms:
	make release
	make windows
	make linux-arm
	make macos

# =====================================================
#                 CLEAN
# =====================================================

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BIN_DIR)

re: fclean release
