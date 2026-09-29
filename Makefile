ifeq ($(strip $(BLOCKSDS)),)
$(error "Environment variable BLOCKSDS not found")
endif

NAME := AetherCore1
GAME_TITLE := AetherCore1
GAME_SUBTITLE1 := DSi AetherCore 1
GAME_SUBTITLE2 := AetherOS 9 Runtime

SOURCEDIRS := source
INCLUDEDIRS := include
GFXDIRS :=
BINDIRS := data
AUDIODIRS :=
NITROFATDIR :=

DEFINES := -D__NDS__ -DARM9
LIBS := -ldswifi9 -lnds9 -lc
LIBDIRS := $(BLOCKSDS)/libs/dswifi $(BLOCKSDS)/libs/libnds $(BLOCKSDS)/libs/libc9

BUILDDIR := build
ELF := $(BUILDDIR)/$(NAME).elf
MAP := $(BUILDDIR)/$(NAME).map
ROM := $(NAME).nds
ARM7ELF := $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi.elf

PREFIX := arm-none-eabi-
CC := $(PREFIX)gcc
CXX := $(PREFIX)g++
OBJDUMP := $(PREFIX)objdump
MKDIR := mkdir
RM := rm -rf

ifneq ($(GFXDIRS),)
SOURCES_PNG := $(shell find -L $(GFXDIRS) -name "*.png")
INCLUDEDIRS += $(addprefix $(BUILDDIR)/,$(GFXDIRS))
endif
ifneq ($(BINDIRS),)
SOURCES_BIN := $(shell find -L $(BINDIRS) -name "*.bin")
INCLUDEDIRS += $(addprefix $(BUILDDIR)/,$(BINDIRS))
endif
SOURCES_S := $(shell find -L $(SOURCEDIRS) -name "*.s")
SOURCES_C := $(shell find -L $(SOURCEDIRS) -name "*.c")
SOURCES_CPP := $(shell find -L $(SOURCEDIRS) -name "*.cpp")

ARCH := -march=armv5te -mtune=arm946e-s
WARNFLAGS := -Wall
ifeq ($(SOURCES_CPP),)
LD := $(CC)
else
LD := $(CXX)
endif

INCLUDEFLAGS := $(foreach path,$(INCLUDEDIRS),-I$(path)) $(foreach path,$(LIBDIRS),-I$(path)/include)
LIBDIRSFLAGS := $(foreach path,$(LIBDIRS),-L$(path)/lib)
CFLAGS := -std=gnu11 $(WARNFLAGS) $(DEFINES) $(ARCH) -mthumb -mthumb-interwork $(INCLUDEFLAGS) -O2 -ffunction-sections -fdata-sections -fomit-frame-pointer
CXXFLAGS := -std=gnu++14 $(WARNFLAGS) $(DEFINES) $(ARCH) -mthumb -mthumb-interwork $(INCLUDEFLAGS) -O2 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fomit-frame-pointer
LDFLAGS := -mthumb -mthumb-interwork $(LIBDIRSFLAGS) -Wl,-Map,$(MAP) -Wl,--gc-sections -nostdlib -T$(BLOCKSDS)/sys/crts/ds_arm9.mem -T$(BLOCKSDS)/sys/crts/ds_arm9.ld -Wl,--start-group $(LIBS) -lgcc -Wl,--end-group

OBJS_SOURCES := $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_S))) $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C))) $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))
DEPS := $(OBJS_SOURCES:.o=.d)

.PHONY: all clean
all: $(ROM)

$(ROM): $(ELF)
	@echo "  NDSTOOL $@"
	$(V)$(BLOCKSDS)/tools/ndstool/ndstool -c $@ -7 $(ARM7ELF) -9 $(ELF) -b $(BLOCKSDS)/tools/ndstool/default_icon.bmp "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)"

$(ELF): $(OBJS_SOURCES)
	@echo "  LD      $@"
	$(V)$(LD) -o $@ $(OBJS_SOURCES) $(BLOCKSDS)/sys/crts/ds_arm9_crt0.o $(LDFLAGS)

$(BUILDDIR)/%.s.o: %.s
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.c.o: %.c
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.cpp.o: %.cpp
	@$(MKDIR) -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	@$(RM) $(ROM) $(BUILDDIR)

-include $(DEPS)
