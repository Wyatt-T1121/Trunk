TROS_KERN_INIT_DIR := $(SRC_DIR)/trunk/tros/kern/init

TROS_KERN_INIT_SRCS_CXX := $(wildcard $(TROS_KERN_INIT_DIR)/*.cpp)

TROS_KERN_INIT_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TROS_KERN_INIT_SRCS_CXX))

TRUNK_OBJS += $(TROS_KERN_INIT_OBJS)