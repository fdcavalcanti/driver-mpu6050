#ifndef MPU6050_H_
#define MPU6050_H_

#define pr_fmt(fmt) "%s %s: " fmt, KBUILD_MODNAME, __func__
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/ioctl.h>
#include <linux/kobject.h>
#include <linux/module.h>

#define MPU_NAME "mpu6050"
#define MEM_SIZE 1024
#define MPU_ADDR 0x68
#define WHO_AM_I_ADDR 0x75
#define PWR_MGMT_ADDR 0x6B      // Power Management Register
#define ACCEL_CONFIG_ADDR 0x1C  // Accelerometer Configuration (AFSEL)
#define TEMP_ADDR 0x41          // Temperature sensor address
#define ACC_XOUT0 0x3B          // First register for Accelerometer X
#define FIFO_R_W 0x74           // FIFO buffer
#define FIFO_COUNT_H 0x72       // FIFO Count Register 15:8 (0x73 for 7:0)
#define FIFO_EN 0x23            // Which sensor measurements are loaded
                                // into the FIFO buffer.
#define SMPRT_DIV 0x19          // divider from the gyroscope output rate
                                // used to generate the Sample Rate

const uint32_t sensitivity_afssel[4] = {16384, 8192, 4096, 2048};

typedef struct xyz_data {
  int16_t x;
  int16_t y;
  int16_t z;
}xyz_data;

typedef struct mpu6050 {
  uint32_t sensitivity;
}mpu6050;

#endif /* MPU6050_H_ */
