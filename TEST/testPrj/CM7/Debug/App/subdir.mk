################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../App/logM7.cpp \
../App/test.cpp 

C_SRCS += \
../App/cSimpleTest.c 

C_DEPS += \
./App/cSimpleTest.d 

OBJS += \
./App/cSimpleTest.o \
./App/logM7.o \
./App/test.o 

CPP_DEPS += \
./App/logM7.d \
./App/test.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o App/%.su App/%.cyclo: ../App/%.c App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../../../../ringBuffer -I../../../../mxLog -I../../../../utils -I../../../../cmdCtrl -I../../ThreadSafe -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/home/gian793/Documents/Projects/stm32h7-cppLibrary/TEST/testPrj/CM7/Core" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
App/%.o App/%.su App/%.cyclo: ../App/%.cpp App/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../../../../ringBuffer -I../../../../mxLog -I../../../../utils -I../../../../cmdCtrl -I../../ThreadSafe -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/home/gian793/Documents/Projects/stm32h7-cppLibrary/TEST/testPrj/CM7/Core" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App

clean-App:
	-$(RM) ./App/cSimpleTest.cyclo ./App/cSimpleTest.d ./App/cSimpleTest.o ./App/cSimpleTest.su ./App/logM7.cyclo ./App/logM7.d ./App/logM7.o ./App/logM7.su ./App/test.cyclo ./App/test.d ./App/test.o ./App/test.su

.PHONY: clean-App

