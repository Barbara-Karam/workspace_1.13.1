################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Inc/MCAL/RCC/RCC_PROG.c 

OBJS += \
./Core/Inc/MCAL/RCC/RCC_PROG.o 

C_DEPS += \
./Core/Inc/MCAL/RCC/RCC_PROG.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/MCAL/RCC/%.o Core/Inc/MCAL/RCC/%.su Core/Inc/MCAL/RCC/%.cyclo: ../Core/Inc/MCAL/RCC/%.c Core/Inc/MCAL/RCC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-MCAL-2f-RCC

clean-Core-2f-Inc-2f-MCAL-2f-RCC:
	-$(RM) ./Core/Inc/MCAL/RCC/RCC_PROG.cyclo ./Core/Inc/MCAL/RCC/RCC_PROG.d ./Core/Inc/MCAL/RCC/RCC_PROG.o ./Core/Inc/MCAL/RCC/RCC_PROG.su

.PHONY: clean-Core-2f-Inc-2f-MCAL-2f-RCC

