#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct spi_config
{
	int32_t fd;
	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	uint16_t delay;
	uint8_t cs_change;
};

struct spi_config spi_setup(char* device, struct spi_config config)
{
	int status;
	
	config.fd = open(device, O_RDWR);
        if (config.fd < 0)
	{
		perror("can't open device");
		//return 1;
	}
	
	/*
         * spi mode
         */
        status = ioctl(config.fd, SPI_IOC_WR_MODE, &config.mode);
        if (status == -1)
	{
                perror("can't set spi mode");
		//return 1;
	}

        status = ioctl(config.fd, SPI_IOC_RD_MODE, &config.mode);
        if (status == -1)
        {
	        perror("can't get spi mode");
		//return 1;
	}

        /*
         * bits per word
         */
        status = ioctl(config.fd, SPI_IOC_WR_BITS_PER_WORD, &config.bits);
        if (status == -1)
	{        
        	perror("can't set bits per word");
		//return 1;
	}

        status = ioctl(config.fd, SPI_IOC_RD_BITS_PER_WORD, &config.bits);
        if (status == -1)
        {
	        perror("can't get bits per word");
		//return 1;
	}

        /*
         * max speed hz
         */
        status = ioctl(config.fd, SPI_IOC_WR_MAX_SPEED_HZ, &config.speed);
        if (status == -1)
        {
	        perror("can't set max speed hz");
		//return 1;
	}

        status = ioctl(config.fd, SPI_IOC_RD_MAX_SPEED_HZ, &config.speed);
        if (status == -1)
        {
	        perror("can't get max speed hz");
		//return 1;
	}

        printf("spi mode: %d\n", config.mode);
        printf("bits per word: %d\n", config.bits);
        printf("max speed: %d Hz (%d KHz)\n", config.speed, config.speed/1000);

	return config;
}


int spi_transact(uint8_t *tx, uint8_t *rx, uint32_t datalen, struct spi_config config)
{
	int status = 0;
	
	struct spi_ioc_transfer tr = 
	{
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = datalen,
                .delay_usecs = config.delay,
                .speed_hz = config.speed,
                .bits_per_word = config.bits,
        };
	
	status = ioctl(config.fd, SPI_IOC_MESSAGE(1), &tr);
	if (status < 1)
	{
		perror("can't send spi message");
	}

	return status;
}

int main ()
{
	char* device = "/dev/spidev0.0";
	uint8_t tx[3]; 
	uint8_t rx[3];

	struct spi_config config =
	{
		.mode = SPI_MODE_0,
		.bits = 8,
		.speed = 5000000,
		.delay = 1,
	};

	config = spi_setup(device, config);

	tx[0] = 0x40;
	tx[1] = 0x00;
	tx[2] = 0x00;
	spi_transact(tx, rx, 3, config);

	printf("rx[0] = %#x, rx[1] = %#x, rx[2] = %#x\n", rx[0], rx[1], rx[2]);

	tx[1] = 0x12;
	
	while(1)
	{
	tx[2] = 0xFF;
	spi_transact(tx, rx, 3, config);
	tx[2] = 0x00;
	spi_transact(tx, rx, 3, config);
	}

	return 0;
}
