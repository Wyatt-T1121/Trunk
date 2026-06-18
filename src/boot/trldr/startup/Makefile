BOOT_STARTUP_DIR := $(SRC_DIR)/boot/trldr/startup

BOOT_STARTUP_SRCS_NASM := $(wildcard $(BOOT_STARTUP_DIR)/*.asm)
BOOT_STARTUP_SRCS_CXX  := $(wildcard $(BOOT_STARTUP_DIR)/*.cpp)

BOOT_STARTUP_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_STARTUP_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(BOOT_STARTUP_SRCS_CXX))

CBK_OBJS += $(BOOT_STARTUP_OBJS)