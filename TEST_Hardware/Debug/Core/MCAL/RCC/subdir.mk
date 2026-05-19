################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MCAL/RCC/RCC_PROG.c 

OBJS += \
./Core/MCAL/RCC/RCC_PROG.o 

C_DEPS += \
./Core/MCAL/RCC/RCC_PROG.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MCAL/RCC/%.o Core/MCAL/RCC/%.su Core/MCAL/RCC/%.cyclo: ../Core/MCAL/RCC/%.c Core/MCAL/RCC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/MCAL -I../Core/LIB -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-MCAL-2f-RCC

clean-Core-2f-MCAL-2f-RCC:
	-$(RM) ./Core/MCAL/RCC/RCC_PROG.cyclo ./Core/MCAL/RCC/RCC_PROG.d ./Core/MCAL/RCC/RCC_PROG.o ./Core/MCAL/RCC/RCC_PROG.su

.PHONY: clean-Core-2f-MCAL-2f-RCC

