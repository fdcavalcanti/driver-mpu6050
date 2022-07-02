#define pr_fmt(fmt) "%s %s: " fmt, KBUILD_MODNAME, __func__
#include <linux/i2c.h>
#include <linux/module.h>

#define MPU_NAME "mpu6050"
#define MPU_ADDR 0x68
#define WHO_AM_I_ADDR 0x75
#define PWR_MGMT_ADDR 0x6B      // Power Management Register
#define ACCEL_CONFIG_ADDR 0x1C  // Accelerometer Configuration (AFSEL)
#define TEMP_ADDR 0x41          // Temperature sensor address
#define ACC_XOUT0 0x3B          // First register for Accelerometer X
#define FIFO_EN 0x23            // Which sensor measurements are loaded
                                // into the FIFO buffer.
#define SMPRT_DIV 0x19          // divider from the gyroscope output rate
                                // used to generate the Sample Rate

static int I2C_BUS = 1;
static struct i2c_adapter *mpu_adapter;
static struct i2c_client *mpu_client;

/**
 * MPU_Write_Reg() - Writes to a register.
 * @reg: Register that will be written.
 * @value: Value that will be written to the register.
 *
 * Return: Number of bytes written.
 */
static int MPU_Write_Reg(unsigned char reg, unsigned int value) {
  unsigned char send_buffer[2] = {reg, value};
  int ret;

  ret = i2c_master_send(mpu_client, send_buffer, 2);
  if (ret != 2) {
    pr_err("Failed writing register");
  }
  return ret;
}

/**
 * MPU_Read_Reg() - Read a single register.
 * @reg: Register that will be read.
 *
 * Return: bytes read.
 */
static int MPU_Read_Reg(unsigned char reg, unsigned char *rec_buf) {
  struct i2c_msg msg[2];
  int ret;

  msg[0].addr = mpu_client->addr;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;
  msg[1].addr = mpu_client->addr;
  msg[1].flags = I2C_M_RD;
  msg[1].len = 1;
  msg[1].buf = rec_buf;

  ret = i2c_transfer(mpu_client->adapter, msg, 2);
  if (ret < 0) {
    pr_err("Erro reading register 0x%X", reg);
  }
  return ret;
}

/**
 * MPU_Burst_Read() - Read multiple registers in sequence.
 * @reg: First register of the reading sequence.
 * @length: Number of registers to be read.
 * @rec_buffer: Pointer to a buffer that will store the read data.
 *
 * Return: bytes sent and received
 */
static int MPU_Burst_Read(unsigned char start_reg, unsigned int length,
                          unsigned char *rec_buffer) {
  int ret;
  struct i2c_msg msg[2];

  msg[0].addr = mpu_client->addr;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &start_reg;
  msg[1].addr = mpu_client->addr;
  msg[1].flags = I2C_M_RD;
  msg[1].len = length;
  msg[1].buf = rec_buffer;
  ret = i2c_transfer(mpu_client->adapter, msg, 2);
  if (ret < 0) {
    pr_err("Erro burst reading register 0x%X\n", start_reg);
  }
  return ret;
}

static struct i2c_board_info mpu_board_info = {
  I2C_BOARD_INFO(MPU_NAME, MPU_ADDR),
};

static struct i2c_device_id mpu_id[] = {
  {MPU_NAME, 0}, {}
};
MODULE_DEVICE_TABLE(i2c, mpu_id);

static int mpu_probe(struct i2c_client *client,
                     const struct i2c_device_id *id) {
  unsigned char who_am_i;
  pr_info("Initializing driver");
  MPU_Read_Reg(WHO_AM_I_ADDR, &who_am_i);
  if (who_am_i != MPU_ADDR) {
    pr_err("Bad device address: 0x%X", who_am_i);
  } else {
    pr_info("Found device on: 0x%X", who_am_i);
  }
  MPU_Write_Reg(PWR_MGMT_ADDR, 0x01);
  MPU_Write_Reg(ACCEL_CONFIG_ADDR, 0x08);
  MPU_Write_Reg(FIFO_EN, 0x08);
  MPU_Write_Reg(SMPRT_DIV, 0x4F);
  pr_info("Done probing");
  return 0;
}

static int mpu_remove(struct i2c_client *client) {
  pr_info("Removing\n");
  MPU_Write_Reg(PWR_MGMT_ADDR, 0x80);
  return 0;
}

static struct i2c_driver mpu_driver = {
  .driver = {
    .name = MPU_NAME,
    .owner = THIS_MODULE,
  },
  .probe = mpu_probe,
  .remove = mpu_remove,
  .id_table = mpu_id,
};

static int __init mpu_init(void) {
  mpu_adapter = i2c_get_adapter(I2C_BUS);
  if (mpu_adapter != NULL) {
    mpu_client = i2c_new_client_device(mpu_adapter, &mpu_board_info);
    if (mpu_client != NULL) {
      i2c_add_driver(&mpu_driver);
    }
  }
  i2c_put_adapter(mpu_adapter);
  return 0;
}

static void __exit mpu_exit(void) {
  i2c_unregister_device(mpu_client);
  i2c_del_driver(&mpu_driver);
  pr_info("Driver removed\n");
}

// module_param(MPU_ADDR, int, S_IRUSR | S_IWUSR);
// MODULE_PARM_DESC(MPU_ADDR, "MPU I2C address.");

module_init(mpu_init);
module_exit(mpu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Filipe do Ó Cavalcanti");
MODULE_DESCRIPTION("MPU6050 Kernel Module");
MODULE_VERSION("0.1.0");
