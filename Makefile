# Trunk — Root Makefile
#
# Build chain:
#
#   ISO (DVD/CD boot):
#     make MODE=DEBUG
#     make run-iso
#
#   Disk (HDD boot):
#     make MODE=DEBUG
#     make disk
#     make install
#     make run-disk       (or run-kvm, run-gdb, run-headless)
#
# All targets:
#   make / make MODE=DEBUG   Build kernel + ISO
#   make kernel              Build ELF only
#   make iso                 Build ISO only
#   make disk                Create GPT disk image
#   make install             Install GRUB + kernel onto disk image
#   make run-iso             Boot ISO in QEMU (DVD/CD)
#   make run-disk            Boot disk image in QEMU (HDD)
#   make run-kvm             Boot disk image in QEMU (KVM)
#   make run-gdb             Boot disk image in QEMU (GDB :1234)
#   make run-headless        Boot disk image in QEMU (headless)
#   make clean               Remove build/ entirely
#   make mrproper            Remove build/ + disk images
#   make check-deps          Verify toolchain + system dependencies
#   make info                Show build configuration
#   make list-srcs           List all discovered source files
#   make disasm              Disassemble kernel
#   make sym                 Dump symbol table
#   make headers             Show ELF headers

# --- Configuration -----------------------------------------------------------

include config/build.cfg
include config/toolchain.cfg

# --- Mode default ------------------------------------------------------------

MODE ?= DEBUG

# --- ANSI colors -------------------------------------------------------------

_R  := \033[0;31m
_G  := \033[0;32m
_Y  := \033[0;33m
_B  := \033[0;34m
_M  := \033[0;35m
_C  := \033[0;36m
_BD := \033[1m
_RS := \033[0m

# --- Step helpers ------------------------------------------------------------

_ok    = @printf "  $(_G)[ OK ]$(_RS)  %s\n" "$1"
_info  = @printf "  $(_C)[ INFO ]$(_RS)  %s\n" "$1"
_warn  = @printf "  $(_Y)[ WARN ]$(_RS)  %s\n" "$1"
_step  = @printf "  $(_B)[ .... ]$(_RS)  %s\n" "$1"
_cxx   = @printf "  $(_M)[ CXX ]$(_RS)  %s\n" "$1"
_asm   = @printf "  $(_C)[ ASM ]$(_RS)  %s\n" "$1"
_nasm  = @printf "  $(_C)[ NASM ]$(_RS)  %s\n" "$1"
_ld    = @printf "  $(_Y)[ LD ]$(_RS)  %s\n" "$1"
_iso   = @printf "  $(_B)[ ISO ]$(_RS)  %s\n" "$1"
_disk  = @printf "  $(_B)[ DISK ]$(_RS)  %s\n" "$1"
_clean = @printf "  $(_R)[ CLEAN ]$(_RS)  %s\n" "$1"

# --- Directories -------------------------------------------------------------

SRC_DIR      := src
INCLUDE_DIR  := include
SETUP_DIR    := setup
SCRIPTS_DIR  := scripts

BUILD_DIR    := build
OBJ_DIR      := $(BUILD_DIR)/obj
ELF_DIR      := $(BUILD_DIR)/elf
ISO_DIR      := $(BUILD_DIR)/iso
IMG_DIR      := $(BUILD_DIR)/img
LOG_DIR      := $(BUILD_DIR)/logs
LOG_BUILD    := $(LOG_DIR)/build
LOG_QEMU     := $(LOG_DIR)/qemu

LINKER_DIR   := $(SETUP_DIR)/linker
GRUB_DIR     := $(SETUP_DIR)/grub

# --- Artifacts ---------------------------------------------------------------

KERNEL_ELF    := $(ELF_DIR)/$(KERNEL_NAME).elf
KERNEL_BIN    := $(ELF_DIR)/$(KERNEL_NAME).bin
ISO_IMAGE     := $(ISO_DIR)/$(KERNEL_NAME).iso
DISK_IMAGE    := $(IMG_DIR)/$(KERNEL_NAME).img
LINKER_SCRIPT := $(LINKER_DIR)/trunk.ld

# --- Build mode --------------------------------------------------------------

ifeq ($(MODE),DEBUG)
    BUILD_MODE := DEBUG
    OPT_FLAGS  := -O0 -g -ggdb
    MODE_DEFS  := -DTRUNK_DEBUG
else ifeq ($(MODE),RELEASE)
    BUILD_MODE := RELEASE
    OPT_FLAGS  := -O2
    MODE_DEFS  := -DTRUNK_RELEASE
else
    $(error Unknown MODE: $(MODE). Use MODE=DEBUG or MODE=RELEASE)
endif

MODE_DEFS += -DTRUNK_VERSION=\"$(VERSION)\" -DTRUNK_ARCH=\"$(ARCH)\"

# --- Compiler flags ----------------------------------------------------------

INCLUDES := -I$(INCLUDE_DIR)

CXXFLAGS := \
    -std=c++20            \
    -ffreestanding        \
    -fno-exceptions       \
    -fno-rtti             \
    -fno-stack-protector  \
    -fno-pie              \
    -fno-pic              \
    -mno-red-zone         \
    -mno-mmx              \
    -mno-sse              \
    -mno-sse2             \
    -mcmodel=kernel       \
    -m64                  \
    -Wall                 \
    -Wextra               \
    -Wshadow              \
    -Wcast-align          \
    -Wunused              \
    -Wpedantic            \
    -Wno-unused-parameter \
    -MMD -MP              \
    $(OPT_FLAGS)          \
    $(MODE_DEFS)          \
    $(INCLUDES)

ASFLAGS   := $(INCLUDES)
NASMFLAGS := -f elf64

ifeq ($(MODE),DEBUG)
    NASMFLAGS += -g -F dwarf
endif

# --- Linker flags ------------------------------------------------------------

LDFLAGS := \
    -nostdlib             \
    -static               \
    -T $(LINKER_SCRIPT)   \
    -z max-page-size=0x1000

ifeq ($(MODE),DEBUG)
    LDFLAGS += --no-gc-sections
endif

# --- Source discovery --------------------------------------------------------

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) \
                $(filter $(subst *,%,$2),$d))

SRCS :=

include src/trunk/Makefile

SRCS += $(call rwildcard,$(SRC_DIR)/tklib,*.cpp)
SRCS += $(call rwildcard,$(SRC_DIR)/tklib,*.S)
SRCS += $(call rwildcard,$(SRC_DIR)/tklib,*.asm)

# --- Object file mapping -----------------------------------------------------

SRCS_CXX  := $(filter %.cpp, $(SRCS))
SRCS_ASM  := $(filter %.S,   $(SRCS))
SRCS_NASM := $(filter %.asm, $(SRCS))

OBJS_CXX  := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(SRCS_CXX))
OBJS_ASM  := $(patsubst $(SRC_DIR)/%.S,   $(OBJ_DIR)/%.S.o,   $(SRCS_ASM))
OBJS_NASM := $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(SRCS_NASM))

ALL_OBJS  := $(OBJS_CXX) $(OBJS_ASM) $(OBJS_NASM)

# --- Default target ----------------------------------------------------------

.PHONY: all
all: _dirs $(ISO_IMAGE)
	@printf "\n"
	@printf "  $(_BD)$(_G)Trunk v$(VERSION) — build complete$(_RS)\n"
	@printf "  $(_C)  Mode :$(_RS) $(BUILD_MODE)\n"
	@printf "  $(_C)  Arch :$(_RS) $(ARCH)\n"
	@printf "  $(_C)  ELF  :$(_RS) $(KERNEL_ELF)\n"
	@printf "  $(_C)  ISO  :$(_RS) $(ISO_IMAGE)\n"
	@printf "\n"

# --- Directory creation ------------------------------------------------------

.PHONY: _dirs
_dirs:
	@mkdir -p $(OBJ_DIR) $(ELF_DIR) $(ISO_DIR)/boot/grub \
	          $(IMG_DIR) $(LOG_BUILD) $(LOG_QEMU)

# --- Compilation -------------------------------------------------------------

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@) $(LOG_BUILD)
	$(call _cxx,$<)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ 2>> $(LOG_BUILD)/compile.log

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@) $(LOG_BUILD)
	$(call _asm,$<)
	@$(CXX) $(CXXFLAGS) -x assembler-with-cpp -c $< -o $@ \
		2>> $(LOG_BUILD)/compile.log

$(OBJ_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@) $(LOG_BUILD)
	$(call _nasm,$<)
	@$(NASM) $(NASMFLAGS) $< -o $@ 2>> $(LOG_BUILD)/compile.log

# --- Linking -----------------------------------------------------------------

$(KERNEL_ELF): $(ALL_OBJS) $(LINKER_SCRIPT)
	$(call _ld,$(KERNEL_ELF))
	@$(LD) $(LDFLAGS) $(ALL_OBJS) -o $@ 2>> $(LOG_BUILD)/link.log
	$(call _ok,Kernel linked → $(KERNEL_ELF))
	@$(READELF) -h $@ | grep "Entry point" | \
		awk '{printf "  \033[0;36m[ INFO ]\033[0m  Entry   : %s\n", $$4}'
	@$(NM) $@ | wc -l | \
		awk '{printf "  \033[0;36m[ INFO ]\033[0m  Symbols : %s\n", $$1}'
	@ls -lh $@ | \
		awk '{printf "  \033[0;36m[ INFO ]\033[0m  Size    : %s\n", $$5}'

$(KERNEL_BIN): $(KERNEL_ELF)
	$(call _step,Generating binary...)
	@$(OBJCOPY) -O binary $< $@
	$(call _ok,Binary → $(KERNEL_BIN))

# --- ISO image ---------------------------------------------------------------

$(ISO_IMAGE): $(KERNEL_ELF)
	$(call _iso,Building $(KERNEL_NAME).iso...)
	@cp $(KERNEL_ELF)        $(ISO_DIR)/boot/$(KERNEL_NAME).elf
	@cp $(GRUB_DIR)/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	@$(GRUB_MKRESCUE) -o $@ $(ISO_DIR) 2>$(LOG_BUILD)/grub.log
	$(call _ok,ISO ready → $(ISO_IMAGE))
	@ls -lh $@ | \
		awk '{printf "  \033[0;36m[ INFO ]\033[0m  Size    : %s\n", $$5}'

# --- Disk image --------------------------------------------------------------

.PHONY: disk
disk: _dirs
	$(call _disk,Creating GPT disk image...)
	@bash $(SCRIPTS_DIR)/create/create_disk.sh
	$(call _ok,Disk image ready → $(DISK_IMAGE))

.PHONY: install
install: $(KERNEL_ELF)
	@test -f $(DISK_IMAGE) || \
		(printf "  $(_R)[ FAIL ]$(_RS)  Disk not found. Run: make disk\n" && exit 1)
	$(call _step,Installing GRUB + kernel to disk...)
	@sudo bash $(SCRIPTS_DIR)/installer/installer.sh
	$(call _ok,Installation complete)

# --- Convenience targets -----------------------------------------------------

.PHONY: kernel
kernel: _dirs $(KERNEL_ELF)

.PHONY: iso
iso: _dirs $(ISO_IMAGE)

# --- Run targets  ------------------------------------

.PHONY: run-iso
run-iso:
	@test -f $(ISO_IMAGE) || \
		(printf "  $(_R)[ FAIL ]$(_RS)  ISO not found. Run: make\n" && exit 1)
	$(call _info,Booting from ISO \(DVD/CD\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_basic.sh --iso

.PHONY: run-disk
run-disk:
	@test -f $(DISK_IMAGE) || \
		(printf "  $(_R)[ FAIL ]$(_RS)  Disk not found. Run: make disk && make install\n" \
		 && exit 1)
	$(call _info,Booting from disk image \(HDD\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_basic.sh --disk

.PHONY: run-kvm
run-kvm:
	@test -f $(DISK_IMAGE) || \
		(printf "  $(_R)[ FAIL ]$(_RS)  Disk not found. Run: make disk && make install\n" \
		 && exit 1)
	$(call _info,Booting from disk image \(KVM\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_kvm.sh --disk

.PHONY: run-gdb
run-gdb:
	@test -f $(DISK_IMAGE) || \
		(printf "  $(_R)[ FAIL ]$(_RS)  Disk not found. Run: make disk && make install\n" \
		 && exit 1)
	$(call _info,Booting from disk image \(GDB :1234\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_gdb.sh --disk

.PHONY: run-headless
run-headless:
	@test -f $(DISK_IMAGE) || \
		(printf "  $(_R)[ FAIL ]$(_RS)  Disk not found. Run: make disk && make install\n" \
		 && exit 1)
	$(call _info,Booting from disk image \(headless\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_headless.sh --disk

# --- Analysis ----------------------------------------------------------------

.PHONY: disasm
disasm: $(KERNEL_ELF)
	$(call _info,Disassembling kernel...)
	@$(OBJDUMP) -d -M intel $< > $(LOG_BUILD)/disasm.txt
	$(call _ok,→ $(LOG_BUILD)/disasm.txt)

.PHONY: sym
sym: $(KERNEL_ELF)
	$(call _info,Dumping symbol table...)
	@$(NM) -n $< > $(LOG_BUILD)/symbols.txt
	$(call _ok,→ $(LOG_BUILD)/symbols.txt)

.PHONY: headers
headers: $(KERNEL_ELF)
	@$(READELF) -h $<
	@printf "\n"
	@$(READELF) -S $<

# --- Dependency check --------------------------------------------------------

.PHONY: check-deps
check-deps:
	$(call _info,Checking dependencies...)
	@$(PYTHON) $(SCRIPTS_DIR)/tools/check_deps.py

# --- Clean -------------------------------------------------------------------

.PHONY: clean
clean:
	$(call _clean,Removing build/...)
	@rm -rf $(BUILD_DIR)
	$(call _ok,Clean complete)

.PHONY: mrproper
mrproper: clean
	$(call _ok,Full wipe complete)

# --- Info --------------------------------------------------------------------

.PHONY: info
info:
	@printf "\n"
	@printf "  $(_BD)Trunk — Build Info$(_RS)\n"
	@printf "  $(_C)  Kernel  :$(_RS) $(KERNEL_NAME) v$(VERSION)\n"
	@printf "  $(_C)  Mode    :$(_RS) $(BUILD_MODE)\n"
	@printf "  $(_C)  Arch    :$(_RS) $(ARCH)\n"
	@printf "  $(_C)  Chain   :$(_RS) $(CXX)\n"
	@printf "  $(_C)  C++ std :$(_RS) C++20\n"
	@printf "  $(_C)  C++ src :$(_RS) $(words $(SRCS_CXX))\n"
	@printf "  $(_C)  ASM src :$(_RS) $(words $(SRCS_ASM)) GAS + $(words $(SRCS_NASM)) NASM\n"
	@printf "  $(_C)  Objects :$(_RS) $(words $(ALL_OBJS))\n"
	@printf "  $(_C)  ELF     :$(_RS) $(KERNEL_ELF)\n"
	@printf "  $(_C)  ISO     :$(_RS) $(ISO_IMAGE)\n"
	@printf "  $(_C)  Disk    :$(_RS) $(DISK_IMAGE)\n"
	@printf "\n"

.PHONY: list-srcs
list-srcs:
	@printf "  $(_BD)C++ Sources$(_RS)\n"
	@echo "$(SRCS_CXX)" | tr ' ' '\n' | sed 's/^/    /'
	@printf "  $(_BD)ASM Sources$(_RS)\n"
	@echo "$(SRCS_ASM) $(SRCS_NASM)" | tr ' ' '\n' | sed 's/^/    /'

# --- Dependency tracking -----------------------------------------------------

-include $(ALL_OBJS:.o=.d)