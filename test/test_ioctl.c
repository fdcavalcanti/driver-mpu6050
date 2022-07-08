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

int main(void) {
  xyz_data acc_data;
  mpu6050 mpu_data;
  /* Test opening the device */
  int fd = open(device, O_RDWR);
  if (fd < 0) {
    printf("Failed opening device: %s\n", strerror(errno));
  }

  ioctl(fd, MPU_INFO, &mpu_data);
  printf("Sensitivity: %d\n", mpu_data.sensitivity);

  for (int i = 0; i < 3; i++) {
    ioctl(fd, READ_ACCELEROMETER, &acc_data);
    printf("AccX: %d\n", acc_data.x);
    printf("AccY: %d\n", acc_data.y);
    printf("AccZ: %d\n", acc_data.z);
  }
  close(fd);
  return 0;
}
