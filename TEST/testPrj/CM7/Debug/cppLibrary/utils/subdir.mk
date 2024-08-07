################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/gian793/Documents/Projects/stm32h7-cppLibrary/utils/utils.cpp 

OBJS += \
./cppLibrary/utils/utils.o 

CPP_DEPS += \
./cppLibrary/utils/utils.d 


# Each subdirectory must supply rules for building sources it contributes
cppLibrary/utils/utils.o: /home/gian793/Documents/Projects/stm32h7-cppLibrary/utils/utils.cpp cppLibrary/utils/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../../../../ringBuffer -I../../../../mxLog -I../../../../utils -I../../../../cmdCtrl -I../../ThreadSafe -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/home/gian793/Documents/Projects/stm32h7-cppLibrary/TEST/testPrj/CM7/Core" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-cppLibrary-2f-utils

clean-cppLibrary-2f-utils:
	-$(RM) ./cppLibrary/utils/utils.cyclo ./cppLibrary/utils/utils.d ./cppLibrary/utils/utils.o ./cppLibrary/utils/utils.su

.PHONY: clean-cppLibrary-2f-utils

