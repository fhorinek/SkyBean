################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/xlib/core/subdir.mk
-include src/xlib/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
vario.lss \

FLASH_IMAGE += \
vario.hex \

EEPROM_IMAGE += \
vario.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: pre-build main-build

# Main-build Target
main-build: vario.elf secondary-outputs

# Tool invocations
vario.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++  -lc -lm -Wl,-Map,vario.map,--cref -Wl,-u,vfprintf -mmcu=atxmega32e5 -o "vario.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lc
	@echo 'Finished building target: $@'
	@echo ' '

vario.lss: vario.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S vario.elf  >"vario.lss"
	@echo 'Finished building: $@'
	@echo ' '

vario.hex: vario.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex vario.elf  "vario.hex"
	@echo 'Finished building: $@'
	@echo ' '

vario.eep: vario.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex vario.elf  "vario.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: vario.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t vario.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) vario.elf
	-@echo ' '

pre-build:
	-/usr/bin/python ../src/inc_build_number.py
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build

-include ../makefile.targets
