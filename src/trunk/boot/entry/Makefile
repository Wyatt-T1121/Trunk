BOOT_ENTRY_DIR := $(SRC_DIR)/trunk/boot/entry

BOOT_ENTRY_SRCS_NASM := $(wildcard $(BOOT_ENTRY_DIR)/*.asm)
BOOT_ENTRY_SRCS_ASM  := $(wildcard $(BOOT_ENTRY_DIR)/*.S)

BOOT_ENTRY_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_ENTRY_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.S,   $(OBJ_DIR)/%.S.o,   $(BOOT_ENTRY_SRCS_ASM))

TRUNK_OBJS += $(BOOT_ENTRY_OBJS)