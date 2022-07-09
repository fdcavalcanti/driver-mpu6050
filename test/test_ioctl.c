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
float accx, accy, accz, sens;

int main(void) {
  xyz_data acc_data;
  mpu6050 mpu_data;
  /* Test opening the device */
  int fd = open(device, O_RDWR);
  if (fd < 0) {
    printf("Failed opening device: %s\n", strerror(errno));
  }

  ioctl(fd, MPU_INFO, &mpu_data);
  sens = mpu_data.sensitivity;
  printf("Sensitivity: %f\n", sens);

  for (int i = 0; i < 3; i++) {
    ioctl(fd, READ_ACCELEROMETER, &acc_data);
    accx = acc_data.x/sens;
    accy = acc_data.y/sens;
    accz = acc_data.z/sens;
    printf("AccX: %f\n", accx);
    printf("AccY: %f\n", accy);
    printf("AccZ: %f\n", accz);
    usleep(10000);
  }
  close(fd);
  return 0;
}
