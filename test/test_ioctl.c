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
    printf("AccX: %f\n", (float)acc_data.x/(float)mpu_data.sensitivity);
    printf("AccY: %f\n", (float)acc_data.y/(float)mpu_data.sensitivity);
    printf("AccZ: %f\n", (float)acc_data.z/(float)mpu_data.sensitivity);
  }
  close(fd);
  return 0;
}
