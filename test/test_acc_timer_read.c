// 2022 Filipe Cavalcanti
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SENSITIVIY 8192.0

char device[] = "/sys/fs/mpu6050/xyz_data";

int main(void) {
  int i, ii;
  int test_buf[3];

  /* Test opening the device */
  FILE *fd = fopen(device, "r");
  if (fd == NULL) {
    printf("Failed opening device: %s\n", strerror(errno));
  }

  for (ii = 0; ii < 5; ii++) {
    fscanf(fd, "%d %d %d", &test_buf[0], &test_buf[1], &test_buf[2]);
    for (i = 0; i < 3; i++) {
      float acc;
      acc = test_buf[i]/SENSITIVIY;
      printf("%f\n", acc);
    }
    usleep(10000);
  }

  fclose(fd);
  return 0;
}
