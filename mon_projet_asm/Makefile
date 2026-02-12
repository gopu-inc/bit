# Variables
ASM = nasm
LD = ld
FLAGS = -f elf32
LDFLAGS = -m elf_i386
SRC_DIR = src
OBJ_DIR = obj
BIN = programme

# Cibles
all: $(BIN)

$(BIN): $(OBJ_DIR)/main.o
	$(LD) $(LDFLAGS) $(OBJ_DIR)/main.o -o $(BIN)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.asm
	mkdir -p $(OBJ_DIR)
	$(ASM) $(FLAGS) $(SRC_DIR)/main.asm -o $(OBJ_DIR)/main.o

clean:
	rm -rf $(OBJ_DIR) $(BIN)

