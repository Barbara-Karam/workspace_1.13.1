################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MCAL/DMA/program.c 

OBJS += \
./Core/MCAL/DMA/program.o 

C_DEPS += \
./Core/MCAL/DMA/program.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MCAL/DMA/%.o Core/MCAL/DMA/%.su Core/MCAL/DMA/%.cyclo: ../Core/MCAL/DMA/%.c Core/MCAL/DMA/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/LIB -I../Core/MCAL -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-MCAL-2f-DMA

clean-Core-2f-MCAL-2f-DMA:
	-$(RM) ./Core/MCAL/DMA/program.cyclo ./Core/MCAL/DMA/program.d ./Core/MCAL/DMA/program.o ./Core/MCAL/DMA/program.su

.PHONY: clean-Core-2f-MCAL-2f-DMA

