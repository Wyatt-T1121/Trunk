BOOT_LME_DIR := $(SRC_DIR)/trunk/boot/lme

BOOT_LME_SRCS_NASM := $(wildcard $(BOOT_LME_DIR)/*.asm)
BOOT_LME_SRCS_ASM  := $(wildcard $(BOOT_LME_DIR)/*.S)

BOOT_LME_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_LME_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.S,   $(OBJ_DIR)/%.S.o,   $(BOOT_LME_SRCS_ASM))

TRUNK_OBJS += $(BOOT_LME_OBJS)