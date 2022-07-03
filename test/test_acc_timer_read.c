// 2022 Filipe Cavalcanti
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SENSITIVIY 16384.0

char device[] = "/sys/fs/mpu6050/xyz_data";

int main(void) {
  int i;
  int test_buf[3];

  /* Test opening the device */
  FILE *fd = fopen(device, "r");
  if (FILE == NULL) {
    printf("Failed opening device: %s\n", strerror(errno));
  } else {
    printf("Device opened.\n");
  }

  fscanf(fd, "%d %d %d", &test_buf[0], &test_buf[1], &test_buf[2]);
  for (i = 0; i < 3; i++)
    printf("%d\n", test_buf[i]);

  fclose(fd);
  return 0;
}
