################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MCAL/GPIO/GPIO_PROG.c 

OBJS += \
./Core/MCAL/GPIO/GPIO_PROG.o 

C_DEPS += \
./Core/MCAL/GPIO/GPIO_PROG.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MCAL/GPIO/%.o Core/MCAL/GPIO/%.su Core/MCAL/GPIO/%.cyclo: ../Core/MCAL/GPIO/%.c Core/MCAL/GPIO/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/LIB -I../Core/MCAL -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-MCAL-2f-GPIO

clean-Core-2f-MCAL-2f-GPIO:
	-$(RM) ./Core/MCAL/GPIO/GPIO_PROG.cyclo ./Core/MCAL/GPIO/GPIO_PROG.d ./Core/MCAL/GPIO/GPIO_PROG.o ./Core/MCAL/GPIO/GPIO_PROG.su

.PHONY: clean-Core-2f-MCAL-2f-GPIO

