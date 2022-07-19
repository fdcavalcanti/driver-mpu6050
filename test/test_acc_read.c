// 2022 Filipe Cavalcanti
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "../driver/mpu6050.h"

char device[] = "/dev/mpu6050";
int sample_rate = 100;

int main(void) {
  float sensitivity;
  struct xyz_data acc;
  struct mpu6050 mpu;

  /* Test opening the device */
  int fd = open("/dev/mpu6050", O_RDWR);
  if (fd < 0) {
    printf("Failed to open device\n");
    return 0;
  }

  ioctl(fd, MPU_INFO, &mpu);
  if (mpu.sample_rate != sample_rate)
    ioctl(fd, SET_SAMPLE_RATE, &sample_rate);
  sensitivity = mpu.sensitivity;

  printf("Sample\tAccX\tAccY\tAccZ\n");
  for (int i = 0; i < 20; i++) {
    // Maximum sequential read is 170:
    // Each axis is 2 bytes (total 6 bytes) for 1024 in FIFO space;
    // Thats approx. 170 samples available to be read at once.
    // Must wait some time for the buffer to fill up again.
    float accx, accy, accz;
    ioctl(fd, READ_ACCELEROMETER, &acc);
    accx = acc.x/sensitivity;
    accy = acc.y/sensitivity;
    accz = acc.z/sensitivity;
    printf("%d\t%f\t%f\t%f\n", i, accx, accy, accz);
  }

  close(fd);
  return 0;
}
