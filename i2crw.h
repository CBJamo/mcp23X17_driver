#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>


char i2cwrite(int fd, char chip_address, char reg_address, char *data, int datalen);
char i2cread(int fd, char chip_address, char reg_addres, char *data, int datalen);
