BOOT_VERIFY_DIR := $(SRC_DIR)/trunk/boot/trldr/safety

BOOT_VERIFY_SRCS_NASM := $(wildcard $(BOOT_VERIFY_DIR)/*.asm)
BOOT_VERIFY_SRCS_CXX  := $(wildcard $(BOOT_VERIFY_DIR)/*.cpp)

BOOT_VERIFY_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_VERIFY_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(BOOT_VERIFY_SRCS_CXX))

TRUNK_OBJS += $(BOOT_VERIFY_OBJS)