include src/trunk/mergedcomp/config/build.cfg
include src/trunk/mergedcomp/config/toolchain.cfg

VERSION         := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

OBJ_DIR         := $(BUILD_DIR)/obj
ELF_DIR         := $(BUILD_DIR)/elf
LIB_DIR         := $(BUILD_DIR)/lib
ISO_DIR         := $(BUILD_DIR)/iso
IMG_DIR         := $(BUILD_DIR)/img
LOG_DIR         := $(BUILD_DIR)/logs
LOG_QEMU_DIR    := $(LOG_DIR)/qemu
LOG_BUILD_DIR   := $(LOG_DIR)/build

LINKER_DIR      := $(SETUP_DIR)/linker
GRUB_DIR        := $(SETUP_DIR)/grub

BOOT_ELF        := $(ELF_DIR)/trboot.elf
KERN_ELF        := $(ELF_DIR)/troskern.elf
TKLIB_A         := $(LIB_DIR)/tklib.a
ISO_IMAGE       := $(ISO_DIR)/trunk.iso
DISK_IMAGE      := $(IMG_DIR)/trunk.img
BOOT_LD         := $(LINKER_DIR)/boot.ld
KERN_LD         := $(LINKER_DIR)/trunk.ld

MODE           ?= DEBUG

_R  := \033[0;31m
_G  := \033[0;32m
_Y  := \033[0;33m
_B  := \033[0;34m
_M  := \033[0;35m
_C  := \033[0;36m
_BD := \033[1m
_RS := \033[0m

_ok    = @printf "  $(_G)[ OK ]$(_RS)  %s\n"   "$1"
_info  = @printf "  $(_C)[ INFO ]$(_RS)  %s\n" "$1"
_warn  = @printf "  $(_Y)[ WARN ]$(_RS)  %s\n" "$1"
_step  = @printf "  $(_B)[ .... ]$(_RS)  %s\n" "$1"
_cxx   = @printf "  $(_M)[ CXX ]$(_RS)  %s\n"  "$1"
_asm   = @printf "  $(_C)[ ASM ]$(_RS)  %s\n"  "$1"
_nasm  = @printf "  $(_C)[ NASM ]$(_RS)  %s\n" "$1"
_ld    = @printf "  $(_Y)[ LD ]$(_RS)  %s\n"   "$1"
_ar    = @printf "  $(_Y)[ AR ]$(_RS)  %s\n"   "$1"
_iso   = @printf "  $(_B)[ ISO ]$(_RS)  %s\n"  "$1"
_disk  = @printf "  $(_B)[ DISK ]$(_RS)  %s\n" "$1"
_clean = @printf "  $(_R)[ CLEAN ]$(_RS)  %s\n" "$1"

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

INCLUDES := -I$(INCLUDE_DIR)

CXXFLAGS := \
    -std=c++23            \
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

BOOT_LDFLAGS := \
    -nostdlib           \
    -static             \
    -T $(BOOT_LD)       \
    -z max-page-size=0x1000

KERN_LDFLAGS := \
    -nostdlib           \
    -static             \
    -T $(KERN_LD)       \
    -z max-page-size=0x1000

ifeq ($(MODE),DEBUG)
    BOOT_LDFLAGS += --no-gc-sections
    KERN_LDFLAGS += --no-gc-sections
endif

BOOT_OBJS  :=
KERN_OBJS  :=
TKLIB_OBJS :=

include src/tklib/Makefile
include src/trunk/Makefile

.PHONY: all
all: _dirs $(BOOT_ELF) $(KERN_ELF) $(ISO_IMAGE)
	@printf "\n"
	@printf "  $(_BD)$(_G)Trunk v$(VERSION) — build complete$(_RS)\n"
	$(call _info,Mode     : $(BUILD_MODE))
	$(call _info,Arch     : $(ARCH))
	$(call _info,Boot ELF : $(BOOT_ELF))
	$(call _info,Kern ELF : $(KERN_ELF))
	$(call _info,ISO      : $(ISO_IMAGE))
	@printf "\n"

.PHONY: _dirs
_dirs:
	@mkdir -p \
	    $(OBJ_DIR)          \
	    $(ELF_DIR)          \
	    $(LIB_DIR)          \
	    $(ISO_DIR)/boot/grub \
	    $(IMG_DIR)          \
	    $(LOG_BUILD_DIR)    \
	    $(LOG_QEMU_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(call _cxx,$<)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ 2>> $(LOG_BUILD_DIR)/compile.log

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	$(call _asm,$<)
	@$(CXX) $(CXXFLAGS) -x assembler-with-cpp -c $< -o $@ \
	    2>> $(LOG_BUILD_DIR)/compile.log

$(OBJ_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(call _nasm,$<)
	@$(NASM) $(NASMFLAGS) $< -o $@ 2>> $(LOG_BUILD_DIR)/compile.log

$(TKLIB_A): $(TKLIB_OBJS)
	$(call _ar,tklib.a)
	@$(AR) rcs $@ $^ 2>> $(LOG_BUILD_DIR)/compile.log
	$(call _ok,tklib archived → $(TKLIB_A))

$(BOOT_ELF): $(BOOT_OBJS) $(BOOT_LD)
	$(call _ld,trboot.elf)
	@$(LD) $(BOOT_LDFLAGS) $(BOOT_OBJS) -o $@ 2>> $(LOG_BUILD_DIR)/link.log
	$(call _ok,Boot linked → $(BOOT_ELF))
	$(call _info,Entry : $(shell $(READELF) -h $@ 2>/dev/null | awk '/Entry point/{print $$4}'))
	$(call _info,Size  : $(shell ls -lh $@ | awk '{print $$5}'))

$(KERN_ELF): $(KERN_OBJS) $(TKLIB_A) $(KERN_LD)
	$(call _ld,troskern.elf)
	@$(LD) $(KERN_LDFLAGS) $(KERN_OBJS) $(TKLIB_A) -o $@ 2>> $(LOG_BUILD_DIR)/link.log
	$(call _ok,Kernel linked → $(KERN_ELF))
	$(call _info,Entry   : $(shell $(READELF) -h $@ 2>/dev/null | awk '/Entry point/{print $$4}'))
	$(call _info,Symbols : $(shell $(NM) $@ 2>/dev/null | wc -l))
	$(call _info,Size    : $(shell ls -lh $@ | awk '{print $$5}'))

$(ISO_IMAGE): $(BOOT_ELF) $(KERN_ELF)
	$(call _iso,Building trunk.iso...)
	@cp $(BOOT_ELF)          $(ISO_DIR)/boot/trboot.elf
	@cp $(KERN_ELF)          $(ISO_DIR)/boot/troskern.elf
	@cp $(GRUB_DIR)/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	@$(GRUB_MKRESCUE) -o $@ $(ISO_DIR) 2>$(LOG_BUILD_DIR)/grub.log
	$(call _ok,ISO ready → $(ISO_IMAGE))
	$(call _info,Size : $(shell ls -lh $@ | awk '{print $$5}'))

.PHONY: disk
disk: _dirs
	$(call _disk,Creating GPT disk image...)
	@bash $(SCRIPTS_DIR)/create/create_disk.sh
	$(call _ok,Disk image ready → $(DISK_IMAGE))

.PHONY: install
install: $(BOOT_ELF) $(KERN_ELF)
	@test -f $(DISK_IMAGE) || \
	    ($(call _warn,Disk not found. Run: make disk) && exit 1)
	$(call _step,Installing GRUB + kernels to disk...)
	@sudo bash $(SCRIPTS_DIR)/installer/installer.sh
	$(call _ok,Installation complete)

.PHONY: kernel
kernel: _dirs $(BOOT_ELF) $(KERN_ELF)

.PHONY: iso
iso: _dirs $(ISO_IMAGE)

.PHONY: run-iso
run-iso:
	@test -f $(ISO_IMAGE) || \
	    ($(call _warn,ISO not found. Run: make) && exit 1)
	$(call _info,Booting from ISO...)
	@bash $(SCRIPTS_DIR)/run/qemu_basic.sh --iso

.PHONY: run-disk
run-disk:
	@test -f $(DISK_IMAGE) || \
	    ($(call _warn,Disk not found. Run: make disk && make install) && exit 1)
	$(call _info,Booting from disk image...)
	@bash $(SCRIPTS_DIR)/run/qemu_basic.sh --disk

.PHONY: run-kvm
run-kvm:
	@test -f $(DISK_IMAGE) || \
	    ($(call _warn,Disk not found. Run: make disk && make install) && exit 1)
	$(call _info,Booting from disk image \(KVM\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_kvm.sh --disk

.PHONY: run-gdb
run-gdb:
	@test -f $(DISK_IMAGE) || \
	    ($(call _warn,Disk not found. Run: make disk && make install) && exit 1)
	$(call _info,Booting from disk image \(GDB :1234\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_gdb.sh --disk

.PHONY: run-headless
run-headless:
	@test -f $(DISK_IMAGE) || \
	    ($(call _warn,Disk not found. Run: make disk && make install) && exit 1)
	$(call _info,Booting from disk image \(headless\)...)
	@bash $(SCRIPTS_DIR)/run/qemu_headless.sh --disk

.PHONY: disasm-boot
disasm-boot: $(BOOT_ELF)
	$(call _info,Disassembling boot stage...)
	@$(OBJDUMP) -d -M intel $< > $(LOG_BUILD_DIR)/disasm_boot.txt
	$(call _ok,→ $(LOG_BUILD_DIR)/disasm_boot.txt)

.PHONY: disasm-kern
disasm-kern: $(KERN_ELF)
	$(call _info,Disassembling kernel...)
	@$(OBJDUMP) -d -M intel $< > $(LOG_BUILD_DIR)/disasm_kern.txt
	$(call _ok,→ $(LOG_BUILD_DIR)/disasm_kern.txt)

.PHONY: sym
sym: $(KERN_ELF)
	$(call _info,Dumping symbol table...)
	@$(NM) -n $< > $(LOG_BUILD_DIR)/symbols.txt
	$(call _ok,→ $(LOG_BUILD_DIR)/symbols.txt)

.PHONY: headers
headers: $(BOOT_ELF) $(KERN_ELF)
	@printf "\n  $(_BD)trboot.elf$(_RS)\n"
	@$(READELF) -h $(BOOT_ELF)
	@printf "\n  $(_BD)troskern.elf$(_RS)\n"
	@$(READELF) -h $(KERN_ELF)

.PHONY: check-deps
check-deps:
	$(call _info,Checking dependencies...)
	@$(PYTHON) $(SCRIPTS_DIR)/tools/check_deps.py

.PHONY: info
info:
	@printf "\n"
	@printf "  $(_BD)Trunk — Build Info$(_RS)\n"
	$(call _info,Kernel   : Trunk v$(VERSION))
	$(call _info,Mode     : $(BUILD_MODE))
	$(call _info,Arch     : $(ARCH))
	$(call _info,Chain    : $(CXX))
	$(call _info,C++ std  : C++23)
	$(call _info,Boot ELF : $(BOOT_ELF))
	$(call _info,Kern ELF : $(KERN_ELF))
	$(call _info,tklib    : $(TKLIB_A))
	$(call _info,ISO      : $(ISO_IMAGE))
	$(call _info,Disk     : $(DISK_IMAGE))
	@printf "\n"

.PHONY: clean
clean:
	$(call _clean,Removing build/...)
	@rm -rf $(BUILD_DIR)
	$(call _ok,Clean complete)

.PHONY: mrproper
mrproper: clean
	$(call _ok,Full wipe complete)

-include $(BOOT_OBJS:.o=.d)
-include $(KERN_OBJS:.o=.d)
-include $(TKLIB_OBJS:.o=.d)