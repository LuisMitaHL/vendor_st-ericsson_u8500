# Static config that usually doesn't need to be changed when debugging etc

# Definition of names used in output filenames
SECURE=secure
UNSECURE=unsecure
H12=h12
H13=h13
HZ3=hz3
H44=h44
EMU=emu
GLACIER=glacier

# Sign package alias definitions
U8500_Secure=U8500_STE_R1X
U8500_Unsecure=U8500_STE_emu_R1A
U8500_HZ3_Secure=U8500_HZ3_R1A
U8500_H44_Secure=AP9500_H44_R1A
U8500_Snowball_Secure=U8500_Snowball_R1B
U8500_H13_Avalanche_Secure=DB8500_H13_HM1_R1B
ISSW_U8500_Glacier_H12=U8500_GLACIER_H12_R2A
ISSW_U8500_Glacier_H13=U8500_GLACIER_H13_R2A
ISSW_U8500_Glacier_HZ3=U8500_GLACIER_HZ3_R2A
TA_U8500_Glacier=U8500_HZ3_STE_R2A
M7400_Secure=M7400_STE_R1A
M7400_Unsecure=M7400_STE_emu_R1A
U5500_Secure=U5500_STE_R1A
U5500_Unsecure=U5500_STE_emu_R1A
U9540_EMU=AP9540_E1_R2A
U9540_E2=AP9540_E2_R2A
U8540APE_EMU=8540APE_STE_emu_R1A
U8540APE_E2=8540APE_STE_R1A

DEFAULT_TA_SSWNAMES :=

# No usta enabled signpackage available for these targets
ifeq (0,1)
ifneq "$(SKIP_BUILD_7400A0)" "1"
ifdef M7400_Secure
DEFAULT_TA_SSWNAMES += $(M7400_Secure):7400a0_$(SECURE)
endif
ifdef M7400_Unsecure
DEFAULT_TA_SSWNAMES += $(M7400_Unsecure):7400a0_$(UNSECURE)
endif
endif
endif

# No usta enabled signpackage available for these targets
ifeq (0,1)
ifneq "$(SKIP_BUILD_7400B0)" "1"
ifdef M7400_Secure
DEFAULT_TA_SSWNAMES += $(M7400_Secure):7400b0_$(SECURE)
endif
ifdef M7400_Unsecure
DEFAULT_TA_SSWNAMES += $(M7400_Unsecure):7400b0_$(UNSECURE)
endif
endif
endif

ifneq "$(SKIP_BUILD_9540)" "1"
ifdef U9540_EMU
DEFAULT_TA_SSWNAMES += $(U9540_EMU):9540_$(EMU)
endif
ifdef U9540_E2
DEFAULT_TA_SSWNAMES += $(U9540_E2):9540_e2
endif
endif

ifneq "$(SKIP_BUILD_8540APE)" "1"
ifdef U8540APE_EMU
DEFAULT_TA_SSWNAMES += $(U8540APE_EMU):8540ape_$(EMU)
endif
ifdef U8540APE_E2
DEFAULT_TA_SSWNAMES += $(U8540APE_E2):8540ape_e2
endif
endif

