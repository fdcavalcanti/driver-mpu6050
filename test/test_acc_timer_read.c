#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define SENSITIVIY 16384.

char device[] = "/dev/mpu6050";
int16_t AccX, AccY, AccZ;
float outX, outY, outZ;


int main(void) {
    int fd;
    int i, ii;
    int ret = -1;
    int8_t test_buf[6] = {0};

    /* Test opening the device */
    fd = open(device, O_RDWR);
    if (fd < 0)
        printf("Failed opening device: %s\n", strerror(errno));
    else
        printf("Device opened.\n");

    for (i=0; i<10; i++) {
        /* Test reading from device */
        ret = read(fd, &test_buf[0], 6);
        if (ret < 0)
            printf("Failed reading from device: %s\n", strerror(errno));
        else {
            for (ii=0; ii<6; ii++) {
                printf("0x%X ", (unsigned char)test_buf[ii]);
            }
            printf("\n");
            AccX = (test_buf[0] << 8) + test_buf[1];
            AccY = (test_buf[2] << 8) + test_buf[3];
            AccZ = (test_buf[4] << 8) + test_buf[5];
            outX = AccX / SENSITIVIY;
            outY = AccY / SENSITIVIY;
            outZ = AccZ / SENSITIVIY;
            printf("AccX: %d outX: %f\n", AccX, outX);
            printf("AccY: %d outX: %f\n", AccY, outY);
            printf("AccZ: %d outX: %f\n", AccZ, outZ);
        }
        printf("\n");
        sleep(1);
    }

    close(fd);
    return 0;
}