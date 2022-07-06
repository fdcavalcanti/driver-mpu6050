// 2022 Filipe Cavalcanti
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct xyz_data {
  int16_t x;
  int16_t y;
  int16_t z;
}xyz_data;

#define READ_ACCELEROMETER _IOR('a', 'a', struct xyz_data)
char device[] = "/dev/mpu6050";

int main(void) {
  xyz_data acc_data;
  /* Test opening the device */
  int fd = open(device, O_RDWR);
  if (fd < 0) {
    printf("Failed opening device: %s\n", strerror(errno));
  }

  ioctl(fd, READ_ACCELEROMETER, &acc_data);
  printf("AccX: %d\n", acc_data.x);
  printf("AccY: %d\n", acc_data.y);
  printf("AccZ: %d\n", acc_data.z);

  close(fd);
  return 0;
}
