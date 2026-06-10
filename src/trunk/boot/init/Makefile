BOOT_INIT_DIR := $(SRC_DIR)/trunk/boot/init

BOOT_INIT_SRCS_NASM := $(wildcard $(BOOT_INIT_DIR)/*.asm)
BOOT_INIT_SRCS_ASM  := $(wildcard $(BOOT_INIT_DIR)/*.S)

BOOT_INIT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_INIT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.S,   $(OBJ_DIR)/%.S.o,   $(BOOT_INIT_SRCS_ASM))

TRUNK_OBJS += $(BOOT_INIT_OBJS)