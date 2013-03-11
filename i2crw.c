#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

char i2cwrite(int fd, char chip_address, char reg_address, char *data, int datalen)
{
	char buf[datalen+1];

	buf[0] = reg_address;

	int i;
	for(i = 0; i < datalen; i++)
	{
		buf[i+1] = data[i];
	}

	if(ioctl(fd, I2C_SLAVE, chip_address) < 0)
	{
		perror("Error opening bus");
		return 1;
	}

	if(write(fd, buf, (datalen + 1)) != (datalen + 1))
	{
		perror("Error writing to slave");
		return 1;
	}

	return 0;
}

char i2cread(int fd, char chip_address, char reg_address, char *data, int datalen)
{
	if(ioctl(fd, I2C_SLAVE, chip_address) < 0)
	{
		perror("Error opening bus");
		return 1;
	}

	if(write(fd, &reg_address, 1) != 1)
	{
		perror("Error setting register address");
		return 1;
	}
	
	if(read(fd, data, datalen) != datalen)
	{
		perror("Error reading from slave");
		return 1;
	}
	
	return *data;
}
