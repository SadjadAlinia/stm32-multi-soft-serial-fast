################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/SoftSerial/SoftSerial.c 

OBJS += \
./Core/SoftSerial/SoftSerial.o 

C_DEPS += \
./Core/SoftSerial/SoftSerial.d 


# Each subdirectory must supply rules for building sources it contributes
Core/SoftSerial/%.o: ../Core/SoftSerial/%.c Core/SoftSerial/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-SoftSerial

clean-Core-2f-SoftSerial:
	-$(RM) ./Core/SoftSerial/SoftSerial.d ./Core/SoftSerial/SoftSerial.o

.PHONY: clean-Core-2f-SoftSerial

