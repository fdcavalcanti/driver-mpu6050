#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


char device[] = "/dev/mpu6050";


int main(void) {
    int fd;
    int ret = -1;
    unsigned char test_buf[8];

    /* Test opening the device */
    fd = open(device, O_RDWR);
    if (fd < 0)
        printf("Failed opening device: %s\n", strerror(errno));
    else
        printf("Device opened.\n");
    
    /* Test reading from device */
    ret = read(fd, &test_buf[0], 1);
    if (ret < 0)
        printf("Failed reading from device: %s\n", strerror(errno));
    else
        printf("Read ok.\n");

    /* Test writing to device */
    ret = write(fd, &test_buf[0], 1);
    if (ret < 0)
        printf("Failed writing to device: %s\n", strerror(errno));
    else
        printf("Write ok.\n");

    close(fd);
    return 0;
}