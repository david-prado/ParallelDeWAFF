################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/bfilterDeceived.cpp \
../src/noAdaptiveLaplacian.cpp \
../src/parallelDeWAFF.cpp \
../src/tools.cpp 

OBJS += \
./src/bfilterDeceived.o \
./src/noAdaptiveLaplacian.o \
./src/parallelDeWAFF.o \
./src/tools.o 

CPP_DEPS += \
./src/bfilterDeceived.d \
./src/noAdaptiveLaplacian.d \
./src/parallelDeWAFF.d \
./src/tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fopenmp -I/usr/local/include/opencv -I"/home/davidp/workspace/ParallelDeWAFF/include" -I/usr/local/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


