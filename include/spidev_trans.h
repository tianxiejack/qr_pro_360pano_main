

#ifndef _SPIDEV_TRANS_H_
#define _SPIDEV_TRANS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


int spi_init(const char *device, uint8_t bits, uint32_t speed);
int spi_close(int fd);
int spi_transfer(int fd, struct spi_ioc_transfer *tr);

#endif

