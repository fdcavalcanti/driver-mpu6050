#define pr_fmt(fmt) "%s %s: " fmt, KBUILD_MODNAME, __func__
#include <linux/module.h>


static int __init mpu_init(void) {
  pr_info("Initializing MPU on\n");
  return 0;
}

static void __exit mpu_exit(void) {
  pr_info("Removing MPU\n");
}

module_init(mpu_init);
module_exit(mpu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Filipe do Ó Cavalcanti");
MODULE_DESCRIPTION("MPU6050 Kernel Module");
MODULE_VERSION("0.1.0");
