################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mov/src/BGFGTrack.cpp \
../src/mov/src/Detectfilter.cpp \
../src/mov/src/Kalman.cpp \
../src/mov/src/MOG3.cpp \
../src/mov/src/MovDetector.cpp \
../src/mov/src/mvdectInterface.cpp \
../src/mov/src/postDetector.cpp \
../src/mov/src/psJudge.cpp \
../src/mov/src/vibe-background-sequential.cpp 

OBJS += \
./src/mov/src/BGFGTrack.o \
./src/mov/src/Detectfilter.o \
./src/mov/src/Kalman.o \
./src/mov/src/MOG3.o \
./src/mov/src/MovDetector.o \
./src/mov/src/mvdectInterface.o \
./src/mov/src/postDetector.o \
./src/mov/src/psJudge.o \
./src/mov/src/vibe-background-sequential.o 

CPP_DEPS += \
./src/mov/src/BGFGTrack.d \
./src/mov/src/Detectfilter.d \
./src/mov/src/Kalman.d \
./src/mov/src/MOG3.d \
./src/mov/src/MovDetector.d \
./src/mov/src/mvdectInterface.d \
./src/mov/src/postDetector.d \
./src/mov/src/psJudge.d \
./src/mov/src/vibe-background-sequential.d 


# Each subdirectory must supply rules for building sources it contributes
src/mov/src/%.o: ../src/mov/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -G -g -O0 -gencode arch=compute_50,code=sm_50  -odir "src/mov/src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -G -g -O0 --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


