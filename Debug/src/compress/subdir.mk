################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/compress/Gststreamercontrl.cpp \
../src/compress/MP4cabinCapture.cpp \
../src/compress/ReSplic.cpp \
../src/compress/screenCapture.cpp 

OBJS += \
./src/compress/Gststreamercontrl.o \
./src/compress/MP4cabinCapture.o \
./src/compress/ReSplic.o \
./src/compress/screenCapture.o 

CPP_DEPS += \
./src/compress/Gststreamercontrl.d \
./src/compress/MP4cabinCapture.d \
./src/compress/ReSplic.d \
./src/compress/screenCapture.d 


# Each subdirectory must supply rules for building sources it contributes
src/compress/%.o: ../src/compress/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -DUSE_DETECTV2 -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../src -I../src/OSA_CAP/inc -I../src/GLTools/include -I../src/capture -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/NetCameraCtl -I../src/opencvCapIP -I../src/RTSPCaptureGroup -I../src/ptzProxy -I../src/realRecord -I../src/IPC -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/include/X11 -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/compress" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -DUSE_DETECTV2 -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../src -I../src/OSA_CAP/inc -I../src/GLTools/include -I../src/capture -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/NetCameraCtl -I../src/opencvCapIP -I../src/RTSPCaptureGroup -I../src/ptzProxy -I../src/realRecord -I../src/IPC -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/include/X11 -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


