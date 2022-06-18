#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>


#define I2C_BUS_AVAILABLE 1
#define KERNEL_BUFFER_SIZE 6
#define SLAVE_DEVICE_NAME "mpu6050"
#define MPU6050_ADDR 0x68
#define MPU_ADDR_WHO_AM_I 0x75
#define PWR_MGMT_ADDR 0x6B      // Power Management Register
#define ACCEL_CONFIG_ADDR 0x1C  // Accelerometer Configuration (AFSEL)
#define TEMP_ADDR 0x41          // Temperature sensor address
#define ACC_XOUT0 0x3B          // First register for Accelerometer X

static struct i2c_adapter *mpu_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *mpu_i2c_client      = NULL;  // I2C Client Structure
static struct class *dev_class;
static struct cdev mpu_cdev;
dev_t devNr = 0;
unsigned char *kernel_buffer;

/**
 * MPU_Write_Reg() - Writes to a register.
 * @reg: Register that will be written.
 * @value: Value that will be written to the register.
 *
 * Return: Number of bytes written.
 */
static int MPU_Write_Reg(unsigned char reg, unsigned int value) {
    unsigned char send_buffer[2] = {reg, value};
    return i2c_master_send(mpu_i2c_client, send_buffer, 2);
}


/**
 * MPU_Read_Reg() - Read a single register.
 * @reg: Register that will be read.
 *
 * Return: Register value.
 */
static unsigned char MPU_Read_Reg(unsigned char reg) {
    struct i2c_msg msg[2];
    unsigned char rec_buf[1];

    msg[0].addr = mpu_i2c_client->addr;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = &reg;
    msg[1].addr = mpu_i2c_client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = rec_buf;

    if (i2c_transfer(mpu_i2c_client->adapter, msg, 2) < 0) {
        pr_err("Erro reading register 0x%X\n", reg);
    }
    return rec_buf[0];
}


/**
 * MPU_Burst_Read() - Read multiple registers in sequence.
 * @reg: First register of the reading sequence.
 * @length: Number of registers to be read.
 * @rec_buffer: Pointer to a buffer that will store the read data.
 *
 */
static void MPU_Burst_Read(unsigned char start_reg, unsigned int length, unsigned char *rec_buffer) {
    struct i2c_msg msg[2];

    msg[0].addr = mpu_i2c_client->addr;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = &start_reg;
    msg[1].addr = mpu_i2c_client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len = length;
    msg[1].buf = rec_buffer;
    if (i2c_transfer(mpu_i2c_client->adapter, msg, 2) < 0) {
        pr_err("Erro burst reading register 0x%X\n", start_reg);
    }
}


/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int mpu_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    unsigned char who_am_i;

    pr_info("Initializing MPU\n");
    who_am_i = MPU_Read_Reg(MPU_ADDR_WHO_AM_I);
    if (who_am_i != MPU6050_ADDR) {
        pr_err("Bad device address: 0x%X\n", who_am_i);
    }
    else {
        pr_info("Found device on: 0x%X\n", who_am_i);
    }
    MPU_Write_Reg(PWR_MGMT_ADDR, 0x00);  // Set PLL with X Gyro Reference
    MPU_Write_Reg(ACCEL_CONFIG_ADDR, 0x00);
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


/**
 * This function is called, when the device file is opened
 */
static int mpu_open(struct inode *deviceFile, struct file *instance) {
	pr_info("MPU Driver - Open was called\n");
	return 0;
}

/**
 * This function is called, when the device file is reading
 */
static ssize_t mpu_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    pr_info("MPU Driver - Read was called\n");
    MPU_Burst_Read(0x3B, 6, kernel_buffer);
    if (copy_to_user(user_buffer, kernel_buffer, count)) {
        pr_err("Cannot copy data to user space\n");
    }
    else {
        pr_info("Copied ok\n");
    }
    return 0;
}

/**
 * This function is called, when the device file is writing
 */
static ssize_t mpu_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	pr_info("MPU Driver - Write was called\n");
    return 0;
}

/**
 * This function is called, when the device file is closing
 */
static int mpu_close(struct inode *deviceFile, struct file *instance) {
    pr_info("MPU Driver - Close was called\n");
    return 0;
}

/* Map the file operations */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mpu_open,
    .read = mpu_read,
    .write = mpu_write,
    .release = mpu_close,
};

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
        {SLAVE_DEVICE_NAME, 0},
        { }
};
MODULE_DEVICE_TABLE(i2c, mpu_id);


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
    /* Allocate Device Nr */
    if (alloc_chrdev_region(&devNr, 0, 1, SLAVE_DEVICE_NAME) < 0) {
        pr_err("Could not allocate chrdev\n");
    }
    else {
        pr_info("Allocated dev nr: %d, %d\n", MAJOR(devNr), MINOR(devNr));
    }

    /* Create the struct class. This is necessary to appear in /dev/ */
    dev_class = class_create(THIS_MODULE, "mpu_class");
    if (dev_class == NULL) {
        pr_err("Cannot create the struct class for device\n");
        unregister_chrdev_region(devNr, 1);
        return -1;
    }
    if (device_create(dev_class, NULL, devNr, NULL, SLAVE_DEVICE_NAME) == NULL){
        pr_err("Cannot create the device\n");
        class_destroy(dev_class);
    }

	/* Initialize Device file */
	cdev_init(&mpu_cdev, &fops);
    if (cdev_add(&mpu_cdev, devNr, 1) < 0) {
        pr_err("Cannot create file operation (cdev)\n");
    }

    /* Allocating memory to use for data transfer between kernel and user spaces */
    kernel_buffer = kmalloc(KERNEL_BUFFER_SIZE, GFP_KERNEL);
    if (kernel_buffer == 0) {
        pr_err("Cannot allocate memory in kernel\n");
        return -1;
    }

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
    cdev_del(&mpu_cdev);
    device_destroy(dev_class, devNr);
    class_destroy(dev_class);
    unregister_chrdev_region(devNr, 1);
    pr_info("Removed MPU6050 driver\n");
}


module_init(ModuleInitialization);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Filipe do Ó Cavalcanti");
MODULE_DESCRIPTION("MPU6050 Kernel Module -- studying Embedded Linux");
MODULE_VERSION("0.0.1");
