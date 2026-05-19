################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MCAL/AFIO/AFIO_PROGRAM.c 

OBJS += \
./Core/MCAL/AFIO/AFIO_PROGRAM.o 

C_DEPS += \
./Core/MCAL/AFIO/AFIO_PROGRAM.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MCAL/AFIO/%.o Core/MCAL/AFIO/%.su Core/MCAL/AFIO/%.cyclo: ../Core/MCAL/AFIO/%.c Core/MCAL/AFIO/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/LIB -I../Core/MCAL -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-MCAL-2f-AFIO

clean-Core-2f-MCAL-2f-AFIO:
	-$(RM) ./Core/MCAL/AFIO/AFIO_PROGRAM.cyclo ./Core/MCAL/AFIO/AFIO_PROGRAM.d ./Core/MCAL/AFIO/AFIO_PROGRAM.o ./Core/MCAL/AFIO/AFIO_PROGRAM.su

.PHONY: clean-Core-2f-MCAL-2f-AFIO

