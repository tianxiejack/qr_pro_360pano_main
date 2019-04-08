################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/com/CClient.cpp \
../src/com/CConnect.cpp \
../src/com/CNetBase.cpp \
../src/com/CNetWork.cpp \
../src/com/CPortBase.cpp \
../src/com/CUartBase.cpp \
../src/com/CUartProcess.cpp \
../src/com/PortFactory.cpp \
../src/com/Udpbase.cpp \
../src/com/comm_main.cpp \
../src/com/globalDate.cpp \
../src/com/statCtrl.cpp 

OBJS += \
./src/com/CClient.o \
./src/com/CConnect.o \
./src/com/CNetBase.o \
./src/com/CNetWork.o \
./src/com/CPortBase.o \
./src/com/CUartBase.o \
./src/com/CUartProcess.o \
./src/com/PortFactory.o \
./src/com/Udpbase.o \
./src/com/comm_main.o \
./src/com/globalDate.o \
./src/com/statCtrl.o 

CPP_DEPS += \
./src/com/CClient.d \
./src/com/CConnect.d \
./src/com/CNetBase.d \
./src/com/CNetWork.d \
./src/com/CPortBase.d \
./src/com/CUartBase.d \
./src/com/CUartProcess.d \
./src/com/PortFactory.d \
./src/com/Udpbase.d \
./src/com/comm_main.d \
./src/com/globalDate.d \
./src/com/statCtrl.d 


# Each subdirectory must supply rules for building sources it contributes
src/com/%.o: ../src/com/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/NetCameraCtl -I../src/opencvCapIP -I../src/RTSPCaptureGroup -I../src/ptzProxy -I../src/realRecord -I../src/gstreamer -I/usr/include/X11 -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/com" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/NetCameraCtl -I../src/opencvCapIP -I../src/RTSPCaptureGroup -I../src/ptzProxy -I../src/realRecord -I../src/gstreamer -I/usr/include/X11 -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


