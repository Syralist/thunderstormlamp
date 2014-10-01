################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cloudtest.cpp 

OBJS += \
./cloudtest.o 

CPP_DEPS += \
./cloudtest.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/home/thomas/workspace" -I"/home/thomas/workspace" -D__IN_ECLIPSE__=1 -DUSB_VID= -DUSB_PID= -DARDUINO=1:105+dfsg2-1 -Wall -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega16 -DF_CPU=1000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


