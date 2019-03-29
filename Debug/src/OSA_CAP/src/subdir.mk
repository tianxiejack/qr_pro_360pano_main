################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OSA_CAP/src/osa.cpp \
../src/OSA_CAP/src/osa_buf.cpp \
../src/OSA_CAP/src/osa_eth_client.cpp \
../src/OSA_CAP/src/osa_eth_server.cpp \
../src/OSA_CAP/src/osa_event.cpp \
../src/OSA_CAP/src/osa_file.cpp \
../src/OSA_CAP/src/osa_i2c.cpp \
../src/OSA_CAP/src/osa_mbx.cpp \
../src/OSA_CAP/src/osa_msgq.cpp \
../src/OSA_CAP/src/osa_mutex.cpp \
../src/OSA_CAP/src/osa_pipe.cpp \
../src/OSA_CAP/src/osa_prf.cpp \
../src/OSA_CAP/src/osa_que.cpp \
../src/OSA_CAP/src/osa_rng.cpp \
../src/OSA_CAP/src/osa_sem.cpp \
../src/OSA_CAP/src/osa_thr.cpp \
../src/OSA_CAP/src/osa_tsk.cpp 

OBJS += \
./src/OSA_CAP/src/osa.o \
./src/OSA_CAP/src/osa_buf.o \
./src/OSA_CAP/src/osa_eth_client.o \
./src/OSA_CAP/src/osa_eth_server.o \
./src/OSA_CAP/src/osa_event.o \
./src/OSA_CAP/src/osa_file.o \
./src/OSA_CAP/src/osa_i2c.o \
./src/OSA_CAP/src/osa_mbx.o \
./src/OSA_CAP/src/osa_msgq.o \
./src/OSA_CAP/src/osa_mutex.o \
./src/OSA_CAP/src/osa_pipe.o \
./src/OSA_CAP/src/osa_prf.o \
./src/OSA_CAP/src/osa_que.o \
./src/OSA_CAP/src/osa_rng.o \
./src/OSA_CAP/src/osa_sem.o \
./src/OSA_CAP/src/osa_thr.o \
./src/OSA_CAP/src/osa_tsk.o 

CPP_DEPS += \
./src/OSA_CAP/src/osa.d \
./src/OSA_CAP/src/osa_buf.d \
./src/OSA_CAP/src/osa_eth_client.d \
./src/OSA_CAP/src/osa_eth_server.d \
./src/OSA_CAP/src/osa_event.d \
./src/OSA_CAP/src/osa_file.d \
./src/OSA_CAP/src/osa_i2c.d \
./src/OSA_CAP/src/osa_mbx.d \
./src/OSA_CAP/src/osa_msgq.d \
./src/OSA_CAP/src/osa_mutex.d \
./src/OSA_CAP/src/osa_pipe.d \
./src/OSA_CAP/src/osa_prf.d \
./src/OSA_CAP/src/osa_que.d \
./src/OSA_CAP/src/osa_rng.d \
./src/OSA_CAP/src/osa_sem.d \
./src/OSA_CAP/src/osa_thr.d \
./src/OSA_CAP/src/osa_tsk.d 


# Each subdirectory must supply rules for building sources it contributes
src/OSA_CAP/src/%.o: ../src/OSA_CAP/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/NetCameraCtl -I../src/opencvCapIP -I../src/RTSPCaptureGroup -I../src/ptzProxy -I../src/realRecord -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/OSA_CAP/src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_CAP/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../src/encTrans -I../src/capture -I../src/GLTools/include -I../src -I../src/buffer -I../src/com -I../src/compress -I../src/config -I../src/detect -I../src/GST -I../src/menu -I../src/lk -I../src/pelco -I../src/stich -I../src/mv -I../src/Osd -I../src/mov/inc -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/file -I../src/gpio -I../src/record -I../src/live555 -I../src/live555/BasicUsageEnvironment -I../src/live555/BasicUsageEnvironment/include -I../src/live555/groupsock -I../src/live555/groupsock/include -I../src/live555/liveMedia -I../src/live555/liveMedia/include -I../src/rtsp -I../src/live555/UsageEnvironment -I../src/live555/UsageEnvironment/include -I../src/msg/include -I../src/status -I../src/manager -I../src/com/include -I../src/DxTimer/include -I../src/store -I../src/configfile -I../src/glcom -I../src/pbo -I../src/NetCameraCtl -I../src/opencvCapIP -I../src/RTSPCaptureGroup -I../src/ptzProxy -I../src/realRecord -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


