################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/lk/VarFlow.cpp \
../src/lk/flow_clusterer.cpp \
../src/lk/lkdetect.cpp \
../src/lk/optical_flow_calculator.cpp \
../src/lk/optical_flow_visualizer.cpp \
../src/lk/point_cluster.cpp \
../src/lk/slic.cpp \
../src/lk/vector_cluster.cpp 

OBJS += \
./src/lk/VarFlow.o \
./src/lk/flow_clusterer.o \
./src/lk/lkdetect.o \
./src/lk/optical_flow_calculator.o \
./src/lk/optical_flow_visualizer.o \
./src/lk/point_cluster.o \
./src/lk/slic.o \
./src/lk/vector_cluster.o 

CPP_DEPS += \
./src/lk/VarFlow.d \
./src/lk/flow_clusterer.d \
./src/lk/lkdetect.d \
./src/lk/optical_flow_calculator.d \
./src/lk/optical_flow_visualizer.d \
./src/lk/point_cluster.d \
./src/lk/slic.d \
./src/lk/vector_cluster.d 


# Each subdirectory must supply rules for building sources it contributes
src/lk/%.o: ../src/lk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I/usr/include/freetype2 -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/rtsp -I../src/msg/include -I../src/com/include -I../src/status -I../src/manager -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/realRecord -I../src/ptzProxy -I../src/opencvCapIP -O3 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/lk" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I/usr/include/freetype2 -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/rtsp -I../src/msg/include -I../src/com/include -I../src/status -I../src/manager -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/realRecord -I../src/ptzProxy -I../src/opencvCapIP -O3 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


