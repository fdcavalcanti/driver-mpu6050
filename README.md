# MPU-6050 Linux Device Driver

This project aims to create a very simple kernel module to communicate with
Invensense's MPU-6050 using I2C, focusing on accelerometer data acquisition.

I'm also using this project as a **learning tool** for Linux Device Driver and
Kernel Module development. I encourage this approach to anyone who is also
interested on the subject.

Right now, this module can provide constant accelerometer reads up to 150 Hz,
which can be done through IOCTL calls.

The follow features are under development, such as:
- Sample rate configuration
- Full scale range selection (AFS_SEL)
- Read from FIFO or directly from accelerometer registers
- Temperature monitoring

# Environment
This project is under development on a Raspberry Pi Zero 2W but should work on
other systems, given that the necessary header files are available.

# Usage
To compile and insert this module:
```console
juvenal@pi:~ $ cd driver-mpu6005/driver
juvenal@pi:~/driver-mpu6050/driver $ make
juvenal@pi:~/driver-mpu6050/driver $ sudo insmod mpu6050.ko
```
To remove the module (can be executed anywhere):
```console
juvenal@pi:~/driver-mpu6050/driver $ sudo rmmod mpu6050
```
Recommend checking dmesg for debugging information.

After inserting the module, move to the test folder, compile and run the
accelerometer read test.
```console
juvenal@pi:~/driver-mpu6050/test $ gcc -o test_ioctl test_ioctl.c 
juvenal@pi:~/driver-mpu6050/test $ ./test_ioctl 
```
