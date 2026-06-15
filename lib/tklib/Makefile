TKLIB_SRC_ROOT := lib/tklib
TKLIB_SRC_DIR  := $(TKLIB_SRC_ROOT)/src/tklib

TKLIB_SRCS_CXX := $(wildcard $(TKLIB_SRC_DIR)/*.cpp)

TKLIB_OBJS     := $(patsubst $(TKLIB_SRC_DIR)/%.cpp, $(OBJ_DIR)/lib/tklib/src/%.o, $(TKLIB_SRCS_CXX))

$(OBJ_DIR)/lib/tklib/src/%.o: $(TKLIB_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(call _cxx,$<)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ 2>> $(LOG_BUILD_DIR)/compile.log