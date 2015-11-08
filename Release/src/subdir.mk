################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DeWAFF.cpp \
../src/Laplacian.cpp \
../src/ParallelDeWAFF.cpp \
../src/Tools.cpp 

OBJS += \
./src/DeWAFF.o \
./src/Laplacian.o \
./src/ParallelDeWAFF.o \
./src/Tools.o 

CPP_DEPS += \
./src/DeWAFF.d \
./src/Laplacian.d \
./src/ParallelDeWAFF.d \
./src/Tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fopenmp -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


