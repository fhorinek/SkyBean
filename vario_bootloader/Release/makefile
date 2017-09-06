################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
bootloader.lss \

FLASH_IMAGE += \
bootloader.hex \

EEPROM_IMAGE += \
bootloader.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: bootloader.elf secondary-outputs

# Tool invocations
bootloader.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,bootloader.map -lm -Wl,--section-start=.text=0x008000 -mmcu=atxmega32e5 -o"bootloader.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

bootloader.lss: bootloader.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S bootloader.elf  >"bootloader.lss"
	@echo 'Finished building: $@'
	@echo ' '

bootloader.hex: bootloader.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex --set-start 0x00000 bootloader.elf  "bootloader.hex"
	@echo 'Finished building: $@'
	@echo ' '

bootloader.eep: bootloader.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex bootloader.elf  "bootloader.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: bootloader.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=sysv bootloader.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) bootloader.elf
	-@echo ' '

post-build:
	-@echo 'crop'
	-tail -n +2 bootloader.hex > bootloader_tmp.hex
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
