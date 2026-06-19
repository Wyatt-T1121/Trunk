CBK_KERN_INIT_DIR := $(SRC_DIR)/cbk/kern/init

CBK_KERN_INIT_SRCS_CXX := $(wildcard $(CBK_KERN_INIT_DIR)/*.cpp)

CBK_KERN_INIT_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CBK_KERN_INIT_SRCS_CXX))

CBK_OBJS += $(CBK_KERN_INIT_OBJS)