#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char device[] = "/dev/mpu6050";

int main(void) {
  int fd;
  int ret = -1;
  int aux = 1;
  unsigned char test_buf[8];

  /* Test opening the device */
  fd = open(device, O_RDWR);
  if (fd < 0)
    printf("Failed opening device: %s\n", strerror(errno));
  else
    printf("Device opened.\n");

  /* Test reading from device
   * Expecting 0x68 (who_am_i).
   */
  ret = read(fd, &test_buf[0], aux);
  if (ret < 0) {
    printf("Failed reading from device: %s\n", strerror(errno));
  } else {
    if (test_buf[0] == 0x68) {
      printf("Read ok: address is 0x%X\n", test_buf[0]);
    }
  }

  // Test writing to device
  test_buf[0] = 0x1A;
  test_buf[1] = 0x00;
  ret = write(fd, test_buf, 2);
  if (ret < 0)
    printf("Failed writing to device: %s\n", strerror(errno));
  else
    printf("Write ok.\n");

  close(fd);
  return 0;
}
