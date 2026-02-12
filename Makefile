# --- CONFIGURATION ---
NAME      := bit
SRC_DIR   := src
OBJ_DIR   := obj
BIN_DIR   := bin
INC_DIR   := include

# Détection de l'OS pour le binaire par défaut
ifeq ($(OS),Windows_NT)
    TARGET_EXTENSION := .exe
else
    TARGET_EXTENSION :=
endif

# --- COMPILATEURS & FLAGS ---
CC        := gcc
# Flags de performance et de debug
CFLAGS    := -Wall -Wextra -O3 -std=c99 -D_DEFAULT_SOURCE -I$(INC_DIR)
# Flags de sécurité (Stack protection, hardening)
CFLAGS    += -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Werror=format-security
# Librairies
LDFLAGS   := -lcurl

# --- SOURCES ---
SRCS      := $(wildcard $(SRC_DIR)/*.c)
OBJS      := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# --- COULEURS ---
RESET     := \033[0m
GREEN     := \033[32m
CYAN      := \033[36m
YELLOW    := \033[33m

# --- CIBLES ---

.PHONY: all clean fclean re platforms linux-arm windows macos

all: $(BIN_DIR)/$(NAME)$(TARGET_EXTENSION)

$(BIN_DIR)/$(NAME)$(TARGET_EXTENSION): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@printf "$(CYAN)Linking $(NAME)...$(RESET)\n"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@printf "$(GREEN)✔ Build successful: $@$(RESET)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@printf "$(YELLOW)Compiling $<...$(RESET)\n"
	@$(CC) $(CFLAGS) -c $< -o $@

# --- MULTI-PLATEFORMES (Cross-Compilation) ---

# Linux ARM (Pour Raspberry Pi / Termux)
linux-arm:
	@printf "$(CYAN)Building for Linux ARM...$(RESET)\n"
	arm-linux-gnueabihf-gcc $(CFLAGS) $(SRCS) -o $(BIN_DIR)/$(NAME)-arm-linux $(LDFLAGS) -static

# Windows (Requiert mingw-w64)
windows:
	@printf "$(CYAN)Building for Windows (x64)...$(RESET)\n"
	x86_64-w64-mingw32-gcc $(CFLAGS) $(SRCS) -o $(BIN_DIR)/$(NAME)-win64.exe -lcurl

# macOS (Si sur un Mac, compile pour Intel et Apple Silicon)
macos:
	@printf "$(CYAN)Building Universal Binary for macOS...$(RESET)\n"
	$(CC) $(CFLAGS) -arch x86_64 -arch arm64 $(SRCS) -o $(BIN_DIR)/$(NAME)-macos-universal $(LDFLAGS)

platforms: all linux-arm windows macos

# --- NETTOYAGE ---

clean:
	@rm -rf $(OBJ_DIR)
	@printf "$(YELLOW)Object files removed.$(RESET)\n"

fclean: clean
	@rm -rf $(BIN_DIR)
	@printf "$(YELLOW)Binaries removed.$(RESET)\n"

re: fclean all

