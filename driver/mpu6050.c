#include <linux/module.h>
#include <linux/i2c.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Filipe do Ó Cavalcanti");
MODULE_DESCRIPTION("MPU6050 Kernel Module -- studying Embedded Linux");
MODULE_VERSION("0.0.1");

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_NAME "MPU6050"
#define MPU6050_ADDR 0x68
#define MPU_ADDR_WHO_AM_I 0x75

static struct i2c_adapter *mpu_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *mpu_i2c_client      = NULL;  // I2C Client Structure


static int I2C_Write(unsigned char *reg, unsigned int value) {
    char buf[2];
    buf[0] = reg;
    buf[1] = value;
    int ret = i2c_master_send(mpu_i2c_client, buf, 2);
    return ret;
}

static int MPU6050_Read_Reg(unsigned char reg) {
    struct i2c_msg msg[2];
    char rec_buf[1];

    msg[0].addr = MPU6050_ADDR;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = &reg;
    msg[1].addr = MPU6050_ADDR;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = rec_buf;

    int ret = i2c_transfer(mpu_i2c_adapter, msg, 2);
    if (ret < 0) {
        pr_info("Erro reading register\n");
    }
    return rec_buf[0];
}

/*
** I2C Board Info strucutre
*/
static struct i2c_board_info mpu_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, MPU6050_ADDR)
};
/*
** Structure that has slave device id
*/
static const struct i2c_device_id mpu_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, mpu_id);


/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int mpu_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    pr_info("Probing\n");
    pr_info("Received: 0x%X\n", MPU6050_Read_Reg(0x75));
    return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int mpu_remove(struct i2c_client *client) {
    pr_info("Removing\n");
    return 0;
}

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver mpu_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = mpu_probe,
        .remove         = mpu_remove,
        .id_table       = mpu_id,
};


static int __init ModuleInitialization(void) {
    int ret = -1;
    mpu_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
    if (mpu_i2c_adapter != NULL) {
        mpu_i2c_client = i2c_new_client_device(mpu_i2c_adapter, &mpu_i2c_board_info);
        if (mpu_i2c_client != NULL) {
            i2c_add_driver(&mpu_driver);
            ret = 0;
        }
        // Get adapter will increment adapter number by 1. This call will go back by one.
        i2c_put_adapter(mpu_i2c_adapter);
    }

    pr_info("Initialized MPU6050 driver\n");
    return ret;
}

static void __exit ModuleExit(void) {
    i2c_unregister_device(mpu_i2c_client);
    i2c_del_driver(&mpu_driver);
    pr_info("Removed MPU6050 driver\n");    
}

module_init(ModuleInitialization);
module_exit(ModuleExit);
