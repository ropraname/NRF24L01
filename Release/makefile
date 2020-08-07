################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include sources/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Nrf24L01_T.lss \

FLASH_IMAGE += \
Nrf24L01_T.hex \

EEPROM_IMAGE += \
Nrf24L01_T.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Nrf24L01_T.elf secondary-outputs

# Tool invocations
Nrf24L01_T.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Nrf24L01_T.map -mmcu=atmega128 -o "Nrf24L01_T.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Nrf24L01_T.lss: Nrf24L01_T.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Nrf24L01_T.elf  >"Nrf24L01_T.lss"
	@echo 'Finished building: $@'
	@echo ' '

Nrf24L01_T.hex: Nrf24L01_T.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Nrf24L01_T.elf  "Nrf24L01_T.hex"
	@echo 'Finished building: $@'
	@echo ' '

Nrf24L01_T.eep: Nrf24L01_T.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Nrf24L01_T.elf  "Nrf24L01_T.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Nrf24L01_T.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128 Nrf24L01_T.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Nrf24L01_T.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
