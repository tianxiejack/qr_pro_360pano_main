################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mv/FrameDifference.cpp \
../src/mv/LBFuzzyGaussian.cpp \
../src/mv/StaticFrameDifference.cpp \
../src/mv/TwoPoints.cpp \
../src/mv/two_points.cpp 

OBJS += \
./src/mv/FrameDifference.o \
./src/mv/LBFuzzyGaussian.o \
./src/mv/StaticFrameDifference.o \
./src/mv/TwoPoints.o \
./src/mv/two_points.o 

CPP_DEPS += \
./src/mv/FrameDifference.d \
./src/mv/LBFuzzyGaussian.d \
./src/mv/StaticFrameDifference.d \
./src/mv/TwoPoints.d \
./src/mv/two_points.d 


# Each subdirectory must supply rules for building sources it contributes
src/mv/%.o: ../src/mv/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -D__CUDABE__ -D__DRIVER_TYPES_H__ -D__CUDANVVM__ -DCUDA_DOUBLE_MATH_FUNCTIONS -D__CUDA_ARCH__=500 -D__CUDA_RUNTIME_H__ -D__USE_FAST_MATH__=0 -D__CUDA_FTZ=0 -D__CUDA_PREC_SQRT=1 -D__CUDA_PREC_DIV=1 -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -O3 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "src/mv" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -D__CUDABE__ -D__DRIVER_TYPES_H__ -D__CUDANVVM__ -DCUDA_DOUBLE_MATH_FUNCTIONS -D__CUDA_ARCH__=500 -D__CUDA_RUNTIME_H__ -D__USE_FAST_MATH__=0 -D__CUDA_FTZ=0 -D__CUDA_PREC_SQRT=1 -D__CUDA_PREC_DIV=1 -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -O3 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

