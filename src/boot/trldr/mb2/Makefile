BOOT_MB2_DIR := $(SRC_DIR)/boot/trldr/mb2

BOOT_MB2_SRCS_NASM := $(wildcard $(BOOT_MB2_DIR)/*.asm)
BOOT_MB2_SRCS_CXX  := $(wildcard $(BOOT_MB2_DIR)/*.cpp)

BOOT_MB2_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_MB2_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(BOOT_MB2_SRCS_CXX))

CBK_OBJS += $(BOOT_MB2_OBJS)