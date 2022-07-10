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
int16_t temperature;

int main(void) {
  int test_fail = 0;
  xyz_data acc_data;
  mpu6050 mpu_data;
  /* Test opening the device */
  int fd = open(device, O_RDWR);
  if (fd < 0) {
    printf("Failed opening device: %s\n", strerror(errno));
    test_fail++;
  }

  if (ioctl(fd, READ_TEMPERATURE, &temperature) < 0) {
    printf("Failed IOCTL temp read: %s\n", strerror(errno));
    test_fail++;
  } else {
    printf("Read: %d => Temp in C: %f\n",
           temperature, (temperature/340)+36.53);
  }

  if (ioctl(fd, MPU_INFO, &mpu_data) < 0) {
    printf("Failed IOCTL on mpu_info: %s\n", strerror(errno));
    test_fail++;
  }
  sens = mpu_data.sensitivity;
  printf("Sensitivity: %f\n", sens);

  for (int i = 0; i < 3; i++) {
    if (ioctl(fd, READ_ACCELEROMETER, &acc_data) < 0) {
      printf("Failed IOCTL on acc read: %s\n", strerror(errno));
      test_fail++;
    }
    accx = acc_data.x/sens;
    accy = acc_data.y/sens;
    accz = acc_data.z/sens;
    printf("AccX: %f\n", accx);
    printf("AccY: %f\n", accy);
    printf("AccZ: %f\n", accz);
    usleep(10000);
  }
  close(fd);
  if (test_fail == 0) {
    printf("No errors found.\n");
  } else {
    printf("Got %d errors\n", test_fail);
  }
  return 0;
}
